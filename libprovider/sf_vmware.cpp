#include "sf_platform.h"
#include "sf_provider.h"
#include <cimple/Buffer.h>
#include <cimple/Strings.h>
#include <cimple/Array.h>
#include <cimple/Ref.h>
#include <cimple.h>
#include "CIM_EthernetPort.h"
#include "CIM_SoftwareIdentity.h"

#include "efx_ioctl.h"
#include "ci/tools/byteorder.h"
#include "ci/tools/bitfield.h"
#include "ci/mgmt/mc_flash_layout.h"
#include "efx_regs_mcdi.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <fcntl.h>
#include <libgen.h>

#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>

#include <errno.h>

#include <pthread.h>

#include "curl/curl.h"

#include <arpa/inet.h>

#include <string>
#include <vector>

// Size of block to be read from NIC NVRAM at once
#define CHUNK_LEN 0x80

// These macros are used for setting proper value
// for SIOCEFX ioctl.
#define SET_PAYLOAD_DWORD(_payload, _ofst, _val) \
CI_POPULATE_DWORD_1((_payload).dword[_ofst], \
                    CI_DWORD_0, (_val))
#define PAYLOAD_DWORD(_payload, _ofst) \
  CI_DWORD_FIELD((_payload).dword[_ofst], CI_DWORD_0)

// Common paths for obtaining information about devices
#define PROC_BUS_PATH       "/proc/bus/pci/"
#define DEV_PATH            "/dev/"
#define DEV_SFC_CONTROL     "/dev/sfc_control"

// Maximum length of path
#define PATH_MAX_LEN        1024
// Maximum length of command line
#define CMD_MAX_LEN         1024
// Maximum length of string of sfupdate output
#define SFU_STR_MAX_LEN     1024
// Maximum length of VPD field
#define VPD_FIELD_MAX_LEN   255
// Maximum length of PCI configuration string
#define PCI_CONF_LEN        12

// These values are used to distinguish our network
// interfaces from other devices
#define CLASS_NET_VALUE     0x20000
#define VENDOR_SF_VALUE     0x1924 

#define EFX_MAX_MTU (9 * 1024)

// Convert character to integer properly
#define CHAR2INT(x)         ((int)(x & 0x000000ff))

// VPD tags (used for VPD processing)
#define VPD_TAG_ID   0x02
#define VPD_TAG_END  0x0f
#define VPD_TAG_R    0x10
#define VPD_TAG_W    0x11

// Macro to disable compiler warning about an unused parameter
#define UNUSED(_x) (void )(_x)

// Default version string
#define DEFAULT_VERSION_STR ""

extern "C" {
    extern int sfupdate_main(int argc, char *argv[]);
}

using namespace std;

namespace solarflare 
{
    using cimple::Instance;
    using cimple::CIM_EthernetPort;
    using cimple::CIM_SoftwareIdentity;
    using cimple::Ref;
    using cimple::cast;

    /**
     * Is a symbol a space?
     *
     * @param c   symbol
     * 
     * @result 1 if space, 0 otherwise
     */
    static int isSpace(const char c)
    {
        if (c == ' ' || c == '\t' || c == '\0')
            return 1;
        else
            return 0;
    }

    /**
     * Remove space symbols from the end of a string.
     *
     * @param s     Char array pointer
     */
    static void trim(char *s)
    {
        int i;

        for (i = strlen(s) - 1; i >= 0; i--)
            if (s[i] == ' ' || s[i] == '\n' ||
                s[i] == '\r')
                s[i] = '\0';
            else
                break;
    }

    /**
     * Compare the beginning of the first string with
     * the second one.
     *
     * @param x   The first string
     * @param y   The second string
     *
     * @return The same as strncmp(x, y, strlen(y))
     */
    static inline int strcmp_start(const char *x, const char *y)
    {
        return strncmp(x, y, strlen(y));
    }

    /**
     * Remove quotation symbols and related escaping
     * from the string.
     *
     * @param s   String to be processed
     */
    static void removeQuotes(char *s)
    {
        int i;
        int j;
        int len;
        int c1 = 0;
        int c2 = 0;
        int single_quote = 0;
        int double_quote = 0;

        if (s == NULL)
            return;

        len = strlen(s);

        for (i = 0; i < len; i++)
        {
            c1 = c2;
            c2 = s[i];

            if ((c1 != '\\' && (c2 == '\'' || c2 == '"')) ||
                (c1 == '\\' && double_quote && c2 == '"'))
            {
                if (c1 != '\\' && c2 == '\'')
                {
                    if (!double_quote)
                        single_quote = !single_quote;
                    else
                        continue;
                }
                else if (c1 != '\\' && c2 == '"')
                {
                    if (!single_quote)
                        double_quote = !double_quote;
                    else
                        continue;
                }

                if (c1 == '\\')
                    i--;

                for (j = i; j < len; j++)
                    s[j] = s[j + 1];
                len--;

                if (c1 == '\\')
                    i++;
                c2 = s[i];
            }
        }
    }

    /**
     * Parse command line string to obtain array of arguments of
     * the form passed to the main function of a Linux program normally.
     *
     * @param s       Command line to be parsed
     * @param argc    [out] Where to save number of arguments
     * @param argv    [out] Where to save pointer to the array of
     *                arguments
     * @param values  [out] Where to save pointer to the buffer
     *                with values of arguments
     *
     * @return 0 on success or value less than 0 on error
     */
    static int parseArgvString(const char *s, int *argc,
                               char ***argv, char **values)
    {
        char  c1 = 0;
        char  c2 = 0;
        int   i;
        int   j;
        int   len;
        int   single_quote = 0;
        int   double_quote = 0;
        char *start = NULL;

        char **args = NULL;
        int    count = 0;
        int    max_count = 0;
        char  *buf;

        int rc = 0;

        if (s == NULL || argc == NULL || argv == NULL || values == NULL)
            return -1;

        len = strlen(s);
        buf = new char[len + 1];
        strcpy(buf, s);

        for (i = 0; i <= len; i++)
        {
            c1 = c2;
            c2 = buf[i];

            if (!single_quote && !double_quote)
            {
                if (!isSpace(c1) && isSpace(c2))
                {
                    if (count >= max_count - 1)
                    {
                        char **p = NULL;

                        max_count = (max_count + 1) * 2;
                        p = new char*[max_count];
                        if (p == NULL)
                        {
                            rc = -1;
                            goto fail;
                        }
                        for (j = 0; j < count; j++)
                            p[j] = args[j];
                        delete[] args;
                        args = p;
                    }

                    args[count] = start;
                    buf[i] = '\0';
                    removeQuotes(args[count]);

                    count++;
                }
                else if (isSpace(c1) && !isSpace(c2))
                    start = buf + i;
            }

            if (c2 == '"' && c1 != '\\' && !single_quote)
            {
                if (!double_quote)
                    double_quote = 1;
                else
                    double_quote = 0;
            }
            if (c2 == '\'' && c1 != '\\' && !double_quote)
            {
                if (!single_quote)
                    single_quote = 1;
                else
                    single_quote = 0;
            }
        }

        if (single_quote || double_quote)
        {
            rc = -1;
            goto fail;
        }

        if (count == 0)
            args = new char *;
        args[count] = NULL;

        *argv = args;
        *values = buf;
        *argc = count;

        return 0;
fail:
        delete[] args;
        delete[] buf;
        return rc;
    }

    /**
     * Get reference to a corresponding CIM_EthernetPort value
     * by device name.
     *
     * @param dev_name    Device name
     *
     * @return Reference to CIM_EthernetPort value
     */
    Ref<CIM_EthernetPort> getCIMEthPort(const char *dev_name)
    {
        Ref<CIM_EthernetPort> cimModel = CIM_EthernetPort::create();
        Ref<Instance>         cimInstance;
        Ref<CIM_EthernetPort> cimEthPort;

        cimple::Instance_Enumerator ie;

        if (cimple::cimom::enum_instances(CIMHelper::baseNS,
                                          cimModel.ptr(), ie) != 0)
        {
            cimEthPort.reset(cast<CIM_EthernetPort *>(NULL));
            return cimEthPort;
        }

        for (cimInstance = ie(); !!cimInstance; ie++, cimInstance = ie())
        {
            cimEthPort.reset(cast<CIM_EthernetPort *>(cimInstance.ptr()));
            if (!(cimEthPort->DeviceID.null) &&
                strcmp(cimEthPort->DeviceID.value.c_str(), dev_name) == 0)
                break;
            cimEthPort.reset(cast<CIM_EthernetPort *>(NULL));
        }

        return cimEthPort;
    }

    /**
     * Debugging function checking number of free FDs.
     *
     * @return number of free FDs
     */
    static int freeFDsNum()
    {
#define _MAX_FDS 1024
        int fds[_MAX_FDS];
        int i = 0;
        int j = 0;

        for (i = 0; i < _MAX_FDS; i++)
        {
            fds[i] = open("/dev/null", O_WRONLY); 
            if (fds[i] < 0)
                break;
        }

        for (j = 0; j < i; j++)
            close(fds[j]);

        return i;
#undef _MAX_FDS
    }

    /**
     * Auxiliary type for MCDI data processing.
     */
    typedef union {
        uint8_t     u8[MCDI_CTL_SDU_LEN_MAX];
        uint16_t    u16[MCDI_CTL_SDU_LEN_MAX/2];
        ci_dword_t  dword[MCDI_CTL_SDU_LEN_MAX/4];
    } payload_t;

    /**
     * Description of Ethernet port.
     */
    class PortDescr
    {
    public:
        int     pci_fn;     /**< PCI function */
        string  dev_file;   /**< Path to device file */
        string  dev_name;   /**< Device name */
    };

    /**
     * Description of NIC.
     */
    class NICDescr
    {
    public:
        int pci_domain;                 /**< PCI domain ID */
        int pci_bus;                    /**< PCI bus ID */
        int pci_device;                 /**< PCI device ID */

        vector<PortDescr> ports;        /**< NIC ports */
    };

    /**
     * Vector of NIC descriptions.
     */
    typedef vector<NICDescr> NICDescrs;

    /**
     * Device description.
     */
    class DeviceDescr
    {
    public:
        int     pci_domain_id;  /**< PCI domain ID */
        int     pci_bus_id;     /**< PCI bus ID */
        int     pci_dev_id;     /**< PCI device ID */
        int     pci_fn_id;      /**< PCI function */
        string  dev_name;       /**< Device name */
        string  dev_file;       /**< Path to device file */
    };

    /**
     * Check whether a given name is correct ESXi
     * interface name.
     *
     * @param name  Name to be checked
     * 
     * @return 0 is name is no correct, 1 otherwise
     */
    static int isPortName(const char *name)
    {
        #define ESXI_PORT_NAME_PREF "vmnic"
        char *endptr;

        if (name == NULL)
            return 0;

        if (strlen(name) > IFNAMSIZ)
            return 0;

        if (strncmp(name, ESXI_PORT_NAME_PREF,
                    strlen(ESXI_PORT_NAME_PREF)) != 0)
            return 0;

        if (strtol(name + strlen(ESXI_PORT_NAME_PREF),
                   &endptr, 10) < 0 ||
            endptr == NULL ||
            *endptr != '\0')
          return 0;

        return 1;
        #undef ESXI_PORT_NAME_PREF
    }

    /**
     * Get descriptions for all Solarflare NICs on the machine.
     *
     * @param nics          [out] Where to save NIC descriptions
     *
     * @return 0 on success or error code
     */
    static int getNICs(NICDescrs &nics)
    {
        DIR                 *bus_dir;
        DIR                 *device_dir;
        struct dirent       *bus;
        struct dirent       *device;
        char                 device_path[PATH_MAX_LEN];
        char                 func_path[PATH_MAX_LEN];
        char                 pci_conf[PCI_CONF_LEN];
        FILE                *f;
        int                  vendor_id;
        int                  device_class;

        vector<DeviceDescr> devs;

        int          rc;

        int     cur_domain;
        int     cur_bus;
        int     cur_dev;
        int     cur_fn;
        int     i;
        int     j;
        int     k;
        int     fd;

        struct ifreq    ifr;

        char    *str;

        struct ethtool_drvinfo drvinfo;

        /**
         * Obtain all available network ports.
         */
        device_dir = opendir(DEV_PATH);
        if (device_dir == NULL)
            return -1;

        for (device = readdir(device_dir);
             device != NULL;
             device = readdir(device_dir))
        {
            if (!isPortName(device->d_name))
                continue;

            rc = snprintf(device_path, PATH_MAX_LEN, "%s/%s",
                          DEV_PATH, device->d_name);

            if (rc < 0 || rc >= PATH_MAX_LEN)
            {
                closedir(device_dir);
                return -1;
            }

            fd = open(device_path, O_RDWR);
            if (fd < 0)
                continue;

            memset(&ifr, 0, sizeof(ifr));
            memset(&drvinfo, 0, sizeof(drvinfo));
            drvinfo.cmd = ETHTOOL_GDRVINFO;
            ifr.ifr_data = (char *)&drvinfo;
            strcpy(ifr.ifr_name, device->d_name);
            if (ioctl(fd, SIOCETHTOOL, &ifr) == 0)
            {
                DeviceDescr tmp_dev;

                close(fd);

                tmp_dev.dev_name = device->d_name;
                str = strchr(drvinfo.bus_info, '.');
                if (str == NULL)
                    continue;
                tmp_dev.pci_fn_id = strtol(str + 1, NULL, 16);
                *str = '\0';
                str = strrchr(drvinfo.bus_info, ':');
                if (str == NULL)
                {
                    closedir(device_dir);
                    return -1;
                }
                tmp_dev.pci_dev_id = strtol(str + 1, NULL, 16);
                *str = '\0';
                str = strrchr(drvinfo.bus_info, ':');
                if (str == NULL)
                {
                    tmp_dev.pci_domain_id = 0;
                    tmp_dev.pci_bus_id = strtol(drvinfo.bus_info,
                                                NULL, 16);
                }
                else
                {
                    tmp_dev.pci_bus_id = strtol(str + 1, NULL, 16);
                    *str = '\0';
                    tmp_dev.pci_domain_id = strtol(drvinfo.bus_info,
                                                   NULL, 16);
                }
                tmp_dev.dev_file = device_path;
                devs.push_back(tmp_dev);
            }
            else
                close(fd);
        }

        closedir(device_dir);

        bus_dir = opendir(PROC_BUS_PATH);
        if (bus_dir == NULL)
            return -1;

        /**
         * Filter out ourt NIC ports, group them by NICs.
         */
        for (bus = readdir(bus_dir);
             bus != NULL;
             bus = readdir(bus_dir))
        {
            if (bus->d_name[0] == '.')
                continue;
            rc = snprintf(device_path, PATH_MAX_LEN, "%s/%s",
                          PROC_BUS_PATH, bus->d_name);
            if (rc < 0 || rc >= PATH_MAX_LEN)
            {
                nics.clear();
                return -1;
            }

            device_dir = opendir(device_path);
            if (device_dir == NULL)
                continue;

            str = strchr(bus->d_name, ':');
            if (str == NULL)
            {
                cur_domain = 0;
                cur_bus = strtol(bus->d_name, NULL, 16);
            }
            else
            {
                cur_domain = strtol(bus->d_name, &str, 16);
                if (str == NULL || *str != ':')
                {
                    closedir(device_dir);
                    closedir(bus_dir);
                    nics.clear();
                    return -1;
                }
                cur_bus = strtol(str + 1, NULL, 16);
            }

            for (device = readdir(device_dir);
                 device != NULL;
                 device = readdir(device_dir))
            {
                NICDescr  tmp_nic;
                PortDescr tmp_port;

                if (device->d_name[0] == '.')
                    continue;

                if (snprintf(func_path, PATH_MAX_LEN, "%s/%s",
                             device_path, device->d_name) >= PATH_MAX_LEN)
                    continue;

                cur_dev = strtol(device->d_name, &str, 16);
                if (str == NULL || *str != '.')
                    continue;
                cur_fn = strtol(str + 1, NULL, 16);

                f = fopen(func_path, "r");
                if (f == NULL)
                    continue;
                if (fread(pci_conf, 1, PCI_CONF_LEN, f) < PCI_CONF_LEN)
                {
                    fclose(f);
                    continue;
                }
                fclose(f);
                vendor_id = CHAR2INT(pci_conf[1]) * 256 +
                            CHAR2INT(pci_conf[0]);
                device_class = (CHAR2INT(pci_conf[11]) << 16) +
                               (CHAR2INT(pci_conf[10]) << 8) +
                               CHAR2INT(pci_conf[9]);

                if (vendor_id != VENDOR_SF_VALUE ||
                    device_class != CLASS_NET_VALUE)
                    break;

                for (i = 0; i < (int)devs.size(); i++)
                {
                    if (cur_domain == devs[i].pci_domain_id &&
                        cur_bus == devs[i].pci_bus_id &&
                        cur_dev == devs[i].pci_dev_id &&
                        cur_fn == devs[i].pci_fn_id)
                        break;
                }
                
                if (i == (int)devs.size())
                {
                    closedir(device_dir);
                    closedir(bus_dir);
                    nics.clear();
                    return -1;
                }

                for (j = 0; j < (int)nics.size(); j++)
                {
                    if (nics[j].pci_domain == cur_domain &&
                        nics[j].pci_bus == cur_bus &&
                        nics[j].pci_device == cur_dev)
                        break;
                }

                if (j == (int)nics.size())
                {
                    tmp_nic.pci_domain = cur_domain;
                    tmp_nic.pci_bus = cur_bus;
                    tmp_nic.pci_device = cur_dev;
                    nics.push_back(tmp_nic);
                }

                tmp_port.pci_fn = cur_fn;
                tmp_port.dev_file = devs[i].dev_file;
                tmp_port.dev_name = devs[i].dev_name;
                nics[j].ports.push_back(tmp_port);
            }

            closedir(device_dir);
        }

        closedir(bus_dir);

        /*
         * Sort ports in ascending order in relation to PCI function.
         */
        for (i = 0; i < (int)nics.size(); i++)
        {
            PortDescr tmp_port;

            do {
                k = 0;
                for (j = 0; j < (int)nics[i].ports.size() - 1; j++)
                {
                    if (nics[i].ports[j].pci_fn >
                            nics[i].ports[j + 1].pci_fn)
                    {
                        tmp_port.pci_fn = nics[i].ports[j].pci_fn;
                        tmp_port.dev_file = nics[i].ports[j].dev_file;
                        tmp_port.dev_name = nics[i].ports[j].dev_name;
                        nics[i].ports[j].pci_fn =
                                        nics[i].ports[j + 1].pci_fn;
                        nics[i].ports[j].dev_file =
                                        nics[i].ports[j + 1].dev_file;
                        nics[i].ports[j].dev_name =
                                        nics[i].ports[j + 1].dev_name;
                        nics[i].ports[j + 1].pci_fn = tmp_port.pci_fn;
                        nics[i].ports[j + 1].dev_file = tmp_port.dev_file;
                        nics[i].ports[j + 1].dev_name = tmp_port.dev_name;
                        k++;
                    }
                }
            } while (k > 0);
        }

        return 0;
    }

    /**
     * Parse VPD tag.
     *
     * @param vpd       Pointer to buffer with VPD data
     * @param len       Length of VPD data
     * @param tag_name  [out] Where to save tag name
     * @param tag_len   [out] Where to save tag data len
     * @param tag_start [out] Where to save address of the first
     *                  byte of tag data
     *
     * @return 0 on success or error code
     */
    static int getVPDTag(uint8_t *vpd, uint32_t len,
                         int *tag_name, unsigned int *tag_len,
                         uint8_t **tag_start)
    {
        /* The first bit shows whether it is large (1) or
         * small (0) resource data type tag bit definition. */
        if (vpd[0] & 0x80) // 10000000b
        {
            if (len < 2)
                return -1;
            /* Get large item name */
            *tag_name = vpd[0] & 0x7f; // 01111111b
            *tag_len = (vpd[1] & 0x000000ff) +
                       ((vpd[2] & 0x000000ff) >> 8);
            *tag_start = vpd + 3;
            if (len < *tag_len + 3)
                return -1;
        }
        else
        {
            /* Small resource data type tag bit definition:
             * 0xxxxyyyb, where xxxx - small item name,
             * yyy - length. */
            *tag_name = ((vpd[0] & 0xf8) >> 3); // 01111000b
            *tag_len = (vpd[0] & 0x07); // 00000111b
            *tag_start = vpd + 1;
            if (len < *tag_len + 1)
                return -1;
        }

        return 0;
    }

    /**
     * Parse VPD data.
     *
     * @param vpd               Buffer with VPD data
     * @param len               Length of VPD data
     * @param product_name      [out] Where to save product name
     * @param product_number    [out] Where to save product number
     * @param serial_number     [out] Where to save serial number
     *
     * @return 0 on success or error code
     */
    static int parseVPD(uint8_t *vpd, uint32_t len,
                        string &product_name, string &product_number,
                        string &serial_number)
    {
        int             tag_name;
        unsigned int    tag_len;
        uint8_t        *tag_start;
        uint8_t        *end = vpd + len;
        int             rc;

        uint8_t        *vpd_field;
        char            field_name[3];
        unsigned int    field_len;
        char           *field_value;

        int i = 0;
        uint8_t sum = 0;
        uint8_t *vpd_start = vpd;

        while (vpd < end)
        {
            if ((rc = getVPDTag(vpd, len, &tag_name,
                                &tag_len, &tag_start)) < 0)
                return rc;

            switch (tag_name)
            {
                case VPD_TAG_ID:
                {
                    char *tmp_product_name = new char[tag_len + 1];

                    memcpy(tmp_product_name, tag_start, tag_len);
                    tmp_product_name[tag_len] = '\0';

                    product_name = tmp_product_name;
                    delete[] tmp_product_name;

                    break;
                }

                case VPD_TAG_R:
                case VPD_TAG_W:

                    sum = 0;
                    for (vpd_field = tag_start;
                         vpd_field < tag_start + tag_len; )
                    {
                        field_name[0] = vpd_field[0];
                        field_name[1] = vpd_field[1];
                        field_name[2] = '\0';
                        field_len = vpd_field[2];
                        field_value = new char[field_len + 1];
                        memcpy(field_value, vpd_field + 3, field_len);
                        field_value[field_len] = '\0';

                        if (strcmp(field_name, "PN") == 0)
                            product_number = field_value;
                        else if (strcmp(field_name, "SN") == 0)
                            serial_number = field_value;
                        else if (strcmp(field_name, "RV") == 0 &&
                            tag_name == VPD_TAG_R)
                        {
                            if (field_len < 1)
                            {
                                delete[] field_value;
                                return -1;
                            }

                            sum = 0;
                            for (i = 0; i < vpd_field + 3 - vpd_start; i++)
                                sum += (vpd_start[i] & 0x000000ff);

                            sum += vpd_field[3];

#if 0
                            if (sum != 0)
                                printf("Invalid checksum!\n");
#endif
                        }

                        delete[] field_value;

                        vpd_field += 3 + field_len;
                    }

                    break;
            }

            if (tag_name == VPD_TAG_END)
                break;
            if (tag_name != VPD_TAG_ID &&
                tag_name != VPD_TAG_R &&
                tag_name != VPD_TAG_W &&
                tag_name != VPD_TAG_END)
                return -1;

            vpd = tag_start + tag_len;
            len = end - vpd;

            if (tag_len == 0 || len == 0)
                break;
        }

        return 0;
    }

    /**
     * Read NVRAM data from NIC.
     *
     * @param fd        File descriptor to be used for ioctl()
     * @param data      Where to save obtained data
     * @param offset    Offset of data to be read
     * @param len       Length of data to be read
     * @param ifname    Interface name
     * @param type      This value is determined by port number
     *
     * @return 0 on success or error code
     */
    static int readNVRAMBytes(int fd, uint8_t *data, unsigned int offset,
                              unsigned int len,
                              const char *ifname, int port_number)
    {
        unsigned int    end = offset + len;
        unsigned int    chunk;
        uint8_t        *ptr = data;

        struct efx_mcdi_request *mcdi_req;
        struct efx_ioctl         ioc;
        payload_t                payload;
        uint32_t                 type;

        if (port_number == 0)
            type = MC_CMD_NVRAM_TYPE_STATIC_CFG_PORT0;
        else
            type = MC_CMD_NVRAM_TYPE_STATIC_CFG_PORT1;

        for ( ; offset < end; )
        {
            chunk = end - offset;
            if (chunk > CHUNK_LEN)
                chunk = CHUNK_LEN;

            memset(&ioc, 0, sizeof(ioc));
            strncpy(ioc.if_name, ifname, sizeof(ioc.if_name));
            ioc.cmd = EFX_MCDI_REQUEST;

            mcdi_req = &ioc.u.mcdi_request;
            mcdi_req->cmd = MC_CMD_NVRAM_READ;
            mcdi_req->len = 12;
            
            memset(&payload, 0, sizeof(payload));
            SET_PAYLOAD_DWORD(payload, 0, type);
            SET_PAYLOAD_DWORD(payload, 1, offset);
            SET_PAYLOAD_DWORD(payload, 2, chunk);
            memcpy(mcdi_req->payload, payload.u8,
                   mcdi_req->len);

            if (ioctl(fd, SIOCEFX, &ioc) < 0)
                return -1;

            memcpy(ptr, mcdi_req->payload, mcdi_req->len);
            ptr += mcdi_req->len;
            offset += mcdi_req->len;
            if (mcdi_req->len == 0)
                return -1;
        }

        return 0;
    }

    /**
     * Get VPD.
     *
     * @param ifname            Interface name
     * @param port_number       Port number
     * @param product_name      Where to save product name
     * @param product_number    Where to save product number
     * @param serial_number     Where to save serial number
     *
     * @return 0 on success or error code
     */
    static int getVPD(const char *ifname, int port_number,
                      string &product_name, string &product_number,
                      string &serial_number)
    {
        int       rc;
        int       fd;
        uint8_t  *vpd;
        int       vpd_off;
        int       vpd_len;

        siena_mc_static_config_hdr_t partial_hdr;

        fd = open(DEV_SFC_CONTROL, O_RDWR);
        if (fd < 0)
            return -1;

        if (readNVRAMBytes(fd, (uint8_t *)&partial_hdr, 0,
                           sizeof(partial_hdr),
                           ifname, port_number) < 0)
        {
            close(fd);
            return -1;
        }

        if (CI_BSWAP_LE32(partial_hdr.magic) !=
                                    SIENA_MC_STATIC_CONFIG_MAGIC)
        {
            close(fd);
            return -1;
        }

        vpd_off = CI_BSWAP_LE32(partial_hdr.static_vpd_offset);
        vpd_len = CI_BSWAP_LE32(partial_hdr.static_vpd_length);

        vpd = new uint8_t[vpd_len];
        if (readNVRAMBytes(fd, vpd, vpd_off, vpd_len,
                           ifname, port_number) < 0)
        {
            close(fd);
            delete[] vpd;
            return -1;
        }

        if ((rc = parseVPD(vpd, vpd_len, product_name,
                           product_number, serial_number)) < 0)
        {
            close(fd);
            delete[] vpd;
            return -1;
        }

        delete[] vpd;
        close(fd);

        return 0;
    }

    /**
     * Wrapper function working like popen("sfupdate ..."). It
     * redirects stdout and stderr to pipe and calls main function
     * of sfupdate with arguments obtained from a given command line.
     * After sfupdate main function terminated, original file
     * descriptors for stdout and stderr are restored.
     *
     * @param cmd_line Command line (should start with "sfupdate",
     *                 for example "sfupdate --adapter=vmnic2")
     *
     * @return FD from which output of sfupdate can be read on
     *         succens or negative value on failure
     */
    static int sfupdatePOpen(const char *cmd_line)
    {
        int     argc;
        char  **argv = NULL;
        char   *values = NULL;
        int     saved_stdout_fd = -1;
        int     saved_stderr_fd = -1;
        int     pipefds[2] = {-1, -1};
        int     rc = 0;
        int     restore_stdout = 0;
        int     restore_stderr = 0;

        static pthread_mutex_t sfupdate_mutex = PTHREAD_MUTEX_INITIALIZER;

        if (parseArgvString(cmd_line, &argc, &argv, &values) < 0 ||
            argv == NULL)
            return -1;

        saved_stdout_fd = dup(STDOUT_FILENO);
        if (saved_stdout_fd < 0)
        {
            rc = -1;
            goto fail;
        }

        saved_stderr_fd = dup(STDERR_FILENO);
        if (saved_stderr_fd < 0)
        {
            rc = -1;
            goto fail;
        }

        if (pipe(pipefds) < 0)
        {
            rc = -1;
            goto fail;
        }

        fflush(stdout);
        fflush(stderr);

        if (dup2(pipefds[1], STDOUT_FILENO) < 0)
        {
            rc = -1;
            goto fail;
        }
        else
            restore_stdout = 1;

        if (dup2(pipefds[1], STDERR_FILENO) < 0)
        {
            rc = -1;
            goto fail;
        }
        else
            restore_stderr = 1;

        pthread_mutex_lock(&sfupdate_mutex);
        if ((rc = sfupdate_main(argc, argv)) != 0)
        {
            pthread_mutex_unlock(&sfupdate_mutex);
            rc = -1;
            goto fail;
        }
        pthread_mutex_unlock(&sfupdate_mutex);

    fail:
        fflush(stdout);
        fflush(stderr);

        delete[] argv;
        delete[] values;

        if (restore_stdout > 0)
            if (dup2(saved_stdout_fd, STDOUT_FILENO) < 0 && rc == 0)
                rc = -1;
        if (restore_stderr > 0)
            if (dup2(saved_stderr_fd, STDERR_FILENO) < 0 && rc == 0)
                rc = -1;
        if (saved_stdout_fd >= 0)
            close(saved_stdout_fd);
        if (saved_stderr_fd >= 0)
            close(saved_stderr_fd);

        close(pipefds[1]);
        if (rc == 0)
            return pipefds[0];
        else
        {
            close(pipefds[0]);
            return rc;
        }
    }

    /* Predeclaration */
    static int vmwareInstallFirmware(const NIC *owner,
                                     const char *fileName);

    /**
     * Perform ethool command.
     *
     * @param dev_file          Path to device file
     * @param dev_name          Device name
     * @param cmd               Ethtool command
     * @param edata             Location for get, data for set method
     *
     * @return zero on success, -1 on error
     */
    static int vmwareEthtoolCmd(const char *dev_file, const char *dev_name,
                                unsigned cmd, void *edata)
    {
        int          fd;
        struct ifreq ifr;
        
        memset(&ifr, 0, sizeof(ifr));
        fd = open(dev_file, O_RDWR);
        if (fd < 0)
            return -1;

        strncpy(ifr.ifr_name, dev_name, sizeof(ifr.ifr_name));
        ifr.ifr_data = (char *)edata;
        ((struct ethtool_value *)edata)->cmd = cmd;
        if (ioctl(fd, SIOCETHTOOL, &ifr) < 0)
        {
            close(fd);
            return -1;
        }

        close(fd);
        return 0;
    }

    class VMWarePort : public Port {
        NIC *owner;
        unsigned pci_fn;

    public:
        String dev_file;
        String dev_name;

        VMWarePort(NIC *up, unsigned i, PortDescr &descr) :
                    Port(i), owner(up), pci_fn(descr.pci_fn),
                    dev_file(descr.dev_file.c_str()),
                    dev_name(descr.dev_name.c_str())
        {} 
       
        virtual bool linkStatus() const;
        virtual Speed linkSpeed() const;
        virtual void linkSpeed(Speed sp);
            
        /// @return full-duplex state
        virtual bool fullDuplex() const;
        /// enable or disable full-duplex mode depending on @p fd
        virtual void fullDuplex(bool fd);
        /// @return true if autonegotiation is available
        virtual bool autoneg() const;
        /// enable/disable autonegotiation according to @p an
        virtual void autoneg(bool an);
        
        /// causes a renegotiation like 'ethtool -r'
        virtual void renegotiate();

        /// @return Manufacturer-supplied MAC address
        virtual MACAddress permanentMAC() const;

        virtual const NIC *nic() const { return owner; }
        virtual PCIAddress pciAddress() const
        {
            return owner->pciAddress().fn(pci_fn);
        }

        virtual void initialize() {};
    };

    bool VMWarePort::linkStatus() const
    {
        struct ethtool_value edata;

        memset(&edata, 0, sizeof(edata));
        if (vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                             ETHTOOL_GLINK, &edata) < 0)
            return true;

        return edata.data == 1 ? true : false;
    }

    Port::Speed VMWarePort::linkSpeed() const
    {
        struct ethtool_cmd edata;

        memset(&edata, 0, sizeof(edata));
        if (vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                             ETHTOOL_GSET, &edata) < 0)
            return Speed(Port::SpeedUnknown);

        switch (edata.speed)
        {
            case SPEED_10: return Speed(Port::Speed10M);
            case SPEED_100: return Speed(Port::Speed100M);
            case SPEED_1000: return Speed(Port::Speed1G);
            case SPEED_10000: return Speed(Port::Speed10G);
            default: return Speed(Port::SpeedUnknown);
        }
    }

    void VMWarePort::linkSpeed(Port::Speed sp)
    {
        struct ethtool_cmd edata;

        memset(&edata, 0, sizeof(edata));
        if (vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                             ETHTOOL_GSET, &edata) < 0)
            return;

        switch (sp)
        {
            case Port::Speed10M: edata.speed = SPEED_10; break;
            case Port::Speed100M: edata.speed = SPEED_100; break;
            case Port::Speed1G: edata.speed = SPEED_1000; break;
            case Port::Speed10G: edata.speed = SPEED_10000; break;
            default: return;
        }

        vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                         ETHTOOL_SSET, &edata);
    }
       
    bool VMWarePort::fullDuplex() const
    { 
        struct ethtool_cmd edata;

        memset(&edata, 0, sizeof(edata));
        if (vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                             ETHTOOL_GSET, &edata) < 0)
            return true;

        if (edata.duplex == DUPLEX_HALF)
            return false;

        return true;
    }

    void VMWarePort::fullDuplex(bool fd)
    { 
        struct ethtool_cmd edata;

        memset(&edata, 0, sizeof(edata));
        if (vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                             ETHTOOL_GSET, &edata) < 0)
            return;

        if (fd)
            edata.duplex = DUPLEX_FULL;
        else
            edata.duplex = DUPLEX_HALF;

        vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                         ETHTOOL_SSET, &edata);
    }

    bool VMWarePort::autoneg() const
    {
         struct ethtool_cmd edata;

        memset(&edata, 0, sizeof(edata));
        if (vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                             ETHTOOL_GSET, &edata) < 0)
            return true;

        if (edata.autoneg == AUTONEG_DISABLE)
            return false;

        return true;          
    }

    void VMWarePort::autoneg(bool an)
    {
         struct ethtool_cmd edata;

        memset(&edata, 0, sizeof(edata));
        if (vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                             ETHTOOL_GSET, &edata) < 0)
            return;

        if (an)
            edata.autoneg = AUTONEG_ENABLE;
        else
            edata.autoneg = AUTONEG_DISABLE;

        vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                         ETHTOOL_SSET, &edata);
    }
    
    void VMWarePort::renegotiate()
    {
        struct ethtool_cmd edata;

        memset(&edata, 0, sizeof(edata));
        vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                         ETHTOOL_NWAY_RST, &edata);
    }

    MACAddress VMWarePort::permanentMAC() const
    {
        /**
         * ETHTOOL_GPERMADDR is not supported on ESXi, and
         * we cannot use popen("esxcli...") since fork() is
         * forbidden, so we need to use vendor-specific
         * interface here.
         */
        int       fd;

        siena_mc_static_config_hdr_t partial_hdr;

        fd = open(DEV_SFC_CONTROL, O_RDWR);
        if (fd < 0)
            return MACAddress(0, 0, 0, 0, 0, 0);

        if (readNVRAMBytes(fd, (uint8_t *)&partial_hdr, 0,
                           sizeof(partial_hdr),
                           dev_name.c_str(), pci_fn) < 0)
        {
            close(fd);
            return MACAddress(0, 0, 0, 0, 0, 0);
        }
        else
        {
            close(fd);
            return MACAddress(partial_hdr.mac_addr_base[0],
                              partial_hdr.mac_addr_base[1],
                              partial_hdr.mac_addr_base[2],
                              partial_hdr.mac_addr_base[3],
                              partial_hdr.mac_addr_base[4],
                              partial_hdr.mac_addr_base[5]);
        }
    }

    class VMWareInterface : public Interface {
        const NIC *owner;
        Port *boundPort;
    public:
        VMWareInterface(const NIC *up, unsigned i) :
            Interface(i),
            owner(up),
            boundPort(NULL) { };

        virtual bool ifStatus() const;
        virtual void enable(bool st);
        virtual uint64 mtu() const;
        virtual void mtu(uint64 u);
        virtual String ifName() const;
        virtual MACAddress currentMAC() const;
        virtual void currentMAC(const MACAddress& mac);
        virtual const NIC *nic() const { return owner; }
        virtual PCIAddress pciAddress() const
        {
            return owner->pciAddress().fn(elementId());
        }

        virtual Port *port() { return boundPort; }
        virtual const Port *port() const { return boundPort; }

        void bindToPort(Port *p) { boundPort = p; }

        virtual void initialize() {};
    };

    bool VMWareInterface::ifStatus() const
    {
        /* Implementation is blocked by SF bug 35613 */

        return false;
    }

    void VMWareInterface::enable(bool st)
    {
        /* Implementation is blocked by SF bug 35613 */

        UNUSED(st);
    }

    uint64 VMWareInterface::mtu() const
    {
        Ref<CIM_EthernetPort> cimEthPort;

        cimEthPort =
          getCIMEthPort(((VMWarePort *)boundPort)->dev_name.c_str());

        if (!cimEthPort)
            return 0;

        if (cimEthPort->ActiveMaximumTransmissionUnit.null)
            return 0;
        else
            return cimEthPort->ActiveMaximumTransmissionUnit.value;
    }

    void VMWareInterface::mtu(uint64 u)
    {
        /*
         * This should not be implemented - MTU
         * is set for vSwitch as a whole, change is
         * propagated to its uplinks automatically.
         */
        UNUSED(u);

        return;
    }

    String VMWareInterface::ifName() const
    {
        if (boundPort == NULL)
            return "";

        return ((VMWarePort *)boundPort)->dev_name;
    }

    MACAddress VMWareInterface::currentMAC() const
    {
        int fd = 0;
        struct ifreq req;
        unsigned char *mac_address;

        if (boundPort == NULL)
            return MACAddress(0, 0, 0, 0, 0, 0);

        fd = open(((VMWarePort *)boundPort)->dev_file.c_str(), O_RDWR);
        if (fd < 0)
            return MACAddress(0, 0, 0, 0, 0, 0);

        memset(&req, 0, sizeof(req));
        strncpy(req.ifr_name,
                ((VMWarePort *)boundPort)->dev_name.c_str(),
                sizeof(req.ifr_name));

        if (ioctl(fd, SIOCGIFHWADDR, &req) != 0)
        {
            close(fd);
            return MACAddress(0, 0, 0, 0, 0, 0);
        }
        close(fd);

        mac_address = (unsigned char *)req.ifr_hwaddr.sa_data;
        return MACAddress(mac_address[0],
                          mac_address[1],
                          mac_address[2],
                          mac_address[3],
                          mac_address[4],
                          mac_address[5]);
    }

    void VMWareInterface::currentMAC(const MACAddress& mac)
    {
        /* How to implement it?! */
        UNUSED(mac);
    }

    class VMWareNICFirmware : public NICFirmware {
        const NIC *owner;
    public:
        VMWareNICFirmware(const NIC *o) :
            owner(o) {}
        virtual const NIC *nic() const { return owner; }
        virtual VersionInfo version() const;
        virtual bool syncInstall(const char *file_name)
        {
            if (vmwareInstallFirmware(owner, file_name) < 0)
                return false;

            return true;
        }
        virtual void initialize() {};
    };

    class VMWareBootROM : public BootROM {
        const NIC *owner;
    public:
        VMWareBootROM(const NIC *o) :
            owner(o) {}
        virtual const NIC *nic() const { return owner; }
        virtual VersionInfo version() const;
        virtual bool syncInstall(const char *file_name)
        {
            if (vmwareInstallFirmware(owner, file_name) < 0)
                return false;

            return true;
        }
        virtual void initialize() {};
    };

    class VMWareDiagnostic : public Diagnostic {
        const NIC *owner;
        Result testPassed;
        static const char sampleDescr[];
        static const String diagGenName;
    public:
        VMWareDiagnostic(const NIC *o) :
            Diagnostic(sampleDescr), owner(o), testPassed(NotKnown) {}
        virtual Result syncTest() 
        {
            /* ETHTOOL_TEST is not available on ESXi -
             * see bug 35580 */
            testPassed = Passed;
            log().logStatus("passed");
            return Passed;
        }
        virtual Result result() const { return testPassed; }
        virtual const NIC *nic() const { return owner; }
        virtual void initialize() {};
        virtual const String& genericName() const { return diagGenName; }
    };

    const char VMWareDiagnostic::sampleDescr[] = "VMWare Diagnostic";
    const String VMWareDiagnostic::diagGenName = "Diagnostic";

    class VMWareNIC : public NIC {
        VMWareNICFirmware nicFw;
        VMWareBootROM rom;
        VMWareDiagnostic diag;

        int pci_domain;
        int pci_bus;
        int pci_device;

    public:

        vector<VMWarePort> ports;
        vector<VMWareInterface> intfs;

    protected:
        virtual void setupPorts()
        {
            int i = 0;

            for (i = 0; i < (int)ports.size(); i++)
                ports[i].initialize();
        }
        virtual void setupInterfaces()
        {
            int i = 0;

            for (i = 0; i < (int)ports.size(); i++)
            {
                intfs[i].initialize();
                intfs[i].bindToPort(&ports[i]);
            }
        }
        virtual void setupFirmware()
        {
            nicFw.initialize();
            rom.initialize();
        }
        virtual void setupDiagnostics()
        {
            diag.initialize();
        }
    public:

        VMWareNIC(unsigned idx, NICDescr &descr) :
            NIC(idx),
            nicFw(this),
            rom(this),
            diag(this), pci_domain(descr.pci_domain),
            pci_bus(descr.pci_bus), pci_device(descr.pci_device)
        {
            int i = 0;

            for (i = 0; i < (int)descr.ports.size(); i++)
            {
                ports.push_back(VMWarePort(this, i, descr.ports[i]));
                intfs.push_back(VMWareInterface(this, i));
            }
        }

        virtual VitalProductData vitalProductData() const;
        Connector connector() const;

        uint64 supportedMTU() const { return EFX_MAX_MTU; }

        virtual bool forAllFw(ElementEnumerator& en)
        {
            if(!en.process(nicFw))
                return false;
            return en.process(rom);
        }
        virtual bool forAllFw(ConstElementEnumerator& en) const
        {
            if(!en.process(nicFw))
                return false;
            return en.process(rom);
        }

        virtual bool forAllPorts(ElementEnumerator& en)
        {
            int i = 0;

            for (i = 0; i < (int)ports.size(); i++)
                if (!en.process(ports[i]))
                    return false;

            return true;
        }
        
        virtual bool forAllPorts(ConstElementEnumerator& en) const
        {
            int i = 0;

            for (i = 0; i < (int)ports.size(); i++)
                if (!en.process(ports[i]))
                    return false;

            return true;
        }

        virtual bool forAllInterfaces(ElementEnumerator& en)
        {
            int i = 0;

            for (i = 0; i < (int)ports.size(); i++)
                if (!en.process(intfs[i]))
                    return false;

            return true;
        }
        
        virtual bool forAllInterfaces(ConstElementEnumerator& en) const
        {
            int i = 0;

            for (i = 0; i < (int)ports.size(); i++)
                if (!en.process(intfs[i]))
                    return false;

            return true;
        }

        virtual bool forAllDiagnostics(ElementEnumerator& en)
        {
            return en.process(diag);
        }
        
        virtual bool forAllDiagnostics(ConstElementEnumerator& en) const
        {
            return en.process(diag);
        }

        virtual PCIAddress pciAddress() const;
    };

    VitalProductData VMWareNIC::vitalProductData() const 
    {
        if (ports.size() <= 0)
            return VitalProductData("", "", "", "", "", "");
        else
        {
            string   p_name;
            string   pn;
            string   sn;

            if (getVPD(ports[0].dev_name.c_str(), 0,
                       p_name, pn, sn) < 0)
                return VitalProductData("", "", "", "", "", "");

            VitalProductData vpd(sn.c_str(), "", sn.c_str(), pn.c_str(),
                                 p_name.c_str(), pn.c_str());

            return vpd;
        }
    }

    NIC::Connector VMWareNIC::connector() const
    {
        VitalProductData        vpd = vitalProductData();
        const char             *part = vpd.part().c_str();
        char                    last = 'T';

        if (*part != '\0')
            last = part[strlen(part) - 1];

        switch (last)
        {
            case 'F': return NIC::SFPPlus;
            case 'K': // fallthrough
            case 'H': return NIC::Mezzanine;
            case 'T': return NIC::RJ45;

            default: return NIC::RJ45;
        }
    }

    PCIAddress VMWareNIC::pciAddress() const
    {
        return PCIAddress(pci_domain, pci_bus, pci_device);
    }

    /**
     * Get data via TFTP protocol.
     *
     * @param uri         Data URI
     * @param f           File to write loaded data
     *
     * @return 0 on success, < 0 on failure
     */
    static int tftp_get_file(const char *uri, FILE *f)
    {
        CURL           *curl;
        int             rc = 0;

        if (uri == NULL || f == NULL)
            return -1;

        curl = curl_easy_init();
        if (curl == NULL)
            return -1;

        if (curl_easy_setopt(curl, CURLOPT_URL, uri) != CURLE_OK)
        {
            rc = -1;
            goto curl_fail;
        }
        if (curl_easy_setopt(curl, CURLOPT_WRITEDATA,
                             f) != CURLE_OK)
        {
            rc = -1;
            goto curl_fail;
        }

        CURLcode rc_curl;
        if ((rc_curl = curl_easy_perform(curl)) != CURLE_OK)
        {
            rc = -1;
            goto curl_fail;
        }

curl_fail:
        curl_easy_cleanup(curl);
        return rc;
    }

    /**
     * Install firmware on a NIC from given image.
     *
     * @param owner       NIC class pointer
     * @param fileName    From where to get firmware image
     *
     * @return 0 on success, < 0 on failure
     */
    static int vmwareInstallFirmware(const NIC *owner,
                                     const char *fileName)
    {
#define FILE_PROTO "file://"
#define TFTP_PROTO "tftp://"
        int   rc = 0;
        char  cmd[CMD_MAX_LEN];
        int   fd = -1;
        char  tmp_file[] = "/tmp/sf_firmware_XXXXXX";
        int   tmp_file_used = 0;

        if (((VMWareNIC *)owner)->ports.size() <= 0)
            return -1;

        if (strcmp_start(fileName, FILE_PROTO) == 0)
        {
            rc = snprintf(cmd, CMD_MAX_LEN, "sfupdate --adapter=%s "
                          "--write --image=%s",
                          ((VMWareNIC *)owner)->ports[0].dev_name.c_str(),
                          fileName + strlen(FILE_PROTO));
            if (rc < 0 || rc >= CMD_MAX_LEN)
            {
                return -1;
            }
        }
        else if (strcmp_start(fileName, TFTP_PROTO) == 0)
        {
            FILE *f;

            fd = mkstemp(tmp_file);
            if (fd < 0)
                return -1;

            f = fdopen(fd, "w");
            if (f == NULL)
            {
                close(fd);
                return -1;
            }

            rc = tftp_get_file(fileName, f);
            if (rc != 0)
            {
                fclose(f);
                return -1;
            }
            fclose(f);

            rc = snprintf(cmd, CMD_MAX_LEN, "sfupdate --adapter=%s "
                          "--write --image=%s",
                          ((VMWareNIC *)owner)->ports[0].dev_name.c_str(),
                          tmp_file);
            if (rc < 0 || rc >= CMD_MAX_LEN)
            {
                unlink(tmp_file);
                return -1;
            }

            tmp_file_used = 1;
        }
        else /* SFTP to be implemented */
            return -1;

        fd = sfupdatePOpen(cmd);
        if (fd < 0)
        {
            if (tmp_file_used)
                unlink(tmp_file);
            return -1;
        }

        close(fd);
        if (tmp_file_used)
            unlink(tmp_file);

        return 0;
#undef FILE_PROTO
#undef TFTP_PROTO
    }

    class VMWareDriver : public Driver {
        const Package *owner;
        static const String drvName;
    public:
        VMWareDriver(const Package *pkg, const String& d,
                     const String& sn) :
            Driver(d, sn), owner(pkg) {}
        virtual VersionInfo version() const;
        virtual void initialize() {};
        virtual bool syncInstall(const char *) { return false; }
        virtual const String& genericName() const { return description(); }
        virtual const Package *package() const { return owner; }
    };

    VersionInfo VMWareDriver::version() const
    {
        DIR                     *device_dir;
        struct dirent           *device;
        struct ethtool_drvinfo   drvinfo;
        struct ifreq             ifr;
        int                      fd;
        char                     device_path[PATH_MAX_LEN];

        device_dir = opendir(DEV_PATH);
        if (device_dir == NULL)
            return VersionInfo(DEFAULT_VERSION_STR);

        for (device = readdir(device_dir);
             device != NULL;
             device = readdir(device_dir))
        {
            if (device->d_name[0] == '.')
                continue;
            if (strlen(device->d_name) > IFNAMSIZ)
                continue;

            if (snprintf(device_path, PATH_MAX_LEN, "%s/%s",
                         DEV_PATH, device->d_name) >= PATH_MAX_LEN)
                continue;
            fd = open(device_path, O_RDWR);
            if (fd < 0)
                continue;

            memset(&ifr, 0, sizeof(ifr));
            memset(&drvinfo, 0, sizeof(drvinfo));
            drvinfo.cmd = ETHTOOL_GDRVINFO;
            ifr.ifr_data = (char *)&drvinfo;
            strcpy(ifr.ifr_name, device->d_name);

            if (ioctl(fd, SIOCETHTOOL, &ifr) == 0)
            {
                close(fd);

                if (strcmp(drvinfo.driver, "sfc") == 0)
                {
                    closedir(device_dir);
                    return VersionInfo(drvinfo.version); 
                }
            }
            else
                close(fd);
        }

        closedir(device_dir);

        /*
         * We failed to get it via ethtool (possible reason: no
         * Solarflare interfaces are presented) - we try to get it
         * from VMWare root/cimv2 standard objects.
         */
        Ref<CIM_SoftwareIdentity> cimModel =
                                      CIM_SoftwareIdentity::create();
        Ref<Instance>             cimInstance;
        Ref<CIM_SoftwareIdentity> cimSoftId;
        String                    strVersion;

        cimple::Instance_Enumerator ie;

        if (cimple::cimom::enum_instances(CIMHelper::baseNS,
                                          cimModel.ptr(), ie) != 0)
            return VersionInfo(DEFAULT_VERSION_STR);

        for (cimInstance = ie(); !!cimInstance; ie++, cimInstance = ie())
        {
            cimSoftId.reset(cast<CIM_SoftwareIdentity *>
                                            (cimInstance.ptr()));
            if (!(cimSoftId->Description.null) &&
                strcmp_start(cimSoftId->Description.value.c_str(),
                            "Solarflare Network Driver") == 0)
                break;
            cimSoftId.reset(cast<CIM_SoftwareIdentity *>(NULL));
        }

        if (!cimSoftId)
            return VersionInfo(DEFAULT_VERSION_STR);

        return VersionInfo(cimSoftId->VersionString.value.c_str());
    }

    class VMWareLibrary : public Library {
        const Package *owner;
        VersionInfo vers;
    public:
        VMWareLibrary(const Package *pkg, const String& d, const String& sn, 
                     const VersionInfo& v) :
            Library(d, sn), owner(pkg), vers(v) {}
        virtual VersionInfo version() const { return vers; }
        virtual void initialize() {};
        virtual bool syncInstall(const char *) { return false; }
        virtual const String& genericName() const { return description(); }
        virtual const Package *package() const { return owner; }
    };


    /// @brief stub-only implementation of a software package
    /// with kernel drivers
    class VMWareKernelPackage : public Package {
        VMWareDriver kernelDriver;
    protected:
        virtual void setupContents() { kernelDriver.initialize(); };
    public:
        VMWareKernelPackage() :
            Package("NET Driver RPM", "sfc"),
            kernelDriver(this, "NET Driver", "sfc") {}
        virtual PkgType type() const { return RPM; }
        virtual VersionInfo version() const { return VersionInfo("3.3"); }
        virtual bool syncInstall(const char *) { return true; }
        virtual bool forAllSoftware(ElementEnumerator& en)
        {
            return en.process(kernelDriver);
        }
        virtual bool forAllSoftware(ConstElementEnumerator& en) const 
        {
            return en.process(kernelDriver);
        }
        virtual const String& genericName() const { return description(); }
    };

    /// @brief stub-only implementation of a software package
    /// with provider library
    class VMWareManagementPackage : public Package {
        VMWareLibrary providerLibrary;
    protected:
        virtual void setupContents() { providerLibrary.initialize(); };
    public:
        VMWareManagementPackage() :
            Package("CIM Provider RPM", "sfcprovider"),
            providerLibrary(this, "CIM Provider library", "libSolarflare.so", "0.1") {}
        virtual PkgType type() const { return RPM; }
        virtual VersionInfo version() const { return VersionInfo("0.1"); }
        virtual bool syncInstall(const char *) { return true; }
        virtual bool forAllSoftware(ElementEnumerator& en)
        {
            return en.process(providerLibrary);
        }
        virtual bool forAllSoftware(ConstElementEnumerator& en) const 
        {
            return en.process(providerLibrary);
        }
        virtual const String& genericName() const { return description(); }
    };

    /// @brief stub-only System implementation
    class VMWareSystem : public System {
        VMWareKernelPackage kernelPackage;
        VMWareManagementPackage mgmtPackage;

        VMWareSystem()
        {
            curl_global_init(CURL_GLOBAL_ALL);
        };

        ~VMWareSystem()
        {
            curl_global_cleanup();
        };

    protected:
        void setupNICs()
        {
            // Do nothing
        };

        void setupPackages()
        {
            kernelPackage.initialize();
            mgmtPackage.initialize();
        };
    public:
        static VMWareSystem target;
        bool is64bit() const { return true; }
        OSType osType() const { return RHEL; }
        bool forAllNICs(ConstElementEnumerator& en) const;
        bool forAllNICs(ElementEnumerator& en);
        bool forAllPackages(ConstElementEnumerator& en) const;
        bool forAllPackages(ElementEnumerator& en);
    };
    
    bool VMWareSystem::forAllNICs(ConstElementEnumerator& en) const
    {
        NICDescrs   nics;
        int         i;
        bool        res;
        bool        ret = true;
        int         rc;

        if ((rc = getNICs(nics)) < 0)
            return false;

        for (i = 0; i < (int)nics.size(); i++)
        {
            VMWareNIC  nic(i, nics[i]);

            nic.initialize();
            res = en.process(nic);
            ret = ret && res;
        }

        return ret;
    }

    bool VMWareSystem::forAllNICs(ElementEnumerator& en)
    {
        return forAllNICs((ConstElementEnumerator&) en); 
    }

    bool VMWareSystem::forAllPackages(ConstElementEnumerator& en) const
    {
        if (!en.process(kernelPackage))
            return false;
        return en.process(mgmtPackage);
    }
    
    bool VMWareSystem::forAllPackages(ElementEnumerator& en)
    {
        if (!en.process(kernelPackage))
            return false;
        return en.process(mgmtPackage);
    }

    VMWareSystem VMWareSystem::target;

    System& System::target = VMWareSystem::target;

    VersionInfo VMWareNICFirmware::version() const
    {
        struct ethtool_drvinfo edata;

        if (((VMWareNIC *)owner)->ports.size() <= 0)
            return VersionInfo(DEFAULT_VERSION_STR);

        if (vmwareEthtoolCmd(((VMWareNIC *)owner)->
                                        ports[0].dev_file.c_str(),
                             ((VMWareNIC *)owner)->
                                        ports[0].dev_name.c_str(),
                             ETHTOOL_GDRVINFO, &edata) < 0)
            return VersionInfo(DEFAULT_VERSION_STR);

        return VersionInfo(edata.fw_version);
    }

    VersionInfo VMWareBootROM::version() const
    {
        int   rc = 0;
        char  cmd[CMD_MAX_LEN];
        char  str[SFU_STR_MAX_LEN];
        char *s = NULL;
        char *version = NULL;
        int   fd = -1;
        FILE *f = NULL;

        rc = snprintf(cmd, CMD_MAX_LEN, "sfupdate --adapter=%s",
                      ((VMWareNIC *)owner)->ports[0].dev_name.c_str());
        if (rc < 0 || rc >= CMD_MAX_LEN)
            return VersionInfo(DEFAULT_VERSION_STR);

        fd = sfupdatePOpen(cmd);
        if (fd < 0)
            return VersionInfo(DEFAULT_VERSION_STR);

        f = fdopen(fd, "r");
        if (f == NULL)
        {
            close(fd);
            return VersionInfo(DEFAULT_VERSION_STR);
        }

        while (1)
        {
            if (fgets(str, SFU_STR_MAX_LEN, f) != str)
                break;

            if (strstr(str, "Boot ROM version:") != NULL)
            {
                s = strstr(str, ":");
                if (s == NULL)
                    continue;
                s++;
                while (*s == ' ')
                    s++;
                trim(s);
                version = s;
                break;
            }
        }

        fclose(f);

        if (version != NULL)
            return VersionInfo(version);
        else
            return VersionInfo(DEFAULT_VERSION_STR);
    }
}
