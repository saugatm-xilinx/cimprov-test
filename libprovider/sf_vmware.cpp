#include "sf_platform.h"
#include "sf_provider.h"
#include <cimple/Buffer.h>
#include <cimple/Strings.h>
#include <cimple/Array.h>
#include <cimple/Ref.h>
#include <cimple.h>
#include "CIM_EthernetPort.h"
#include "CIM_SoftwareIdentity.h"

#define EFX_NOT_UPSTREAM
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

#define CHUNK_LEN 0x80

#define SET_PAYLOAD_DWORD(_payload, _ofst, _val) \
CI_POPULATE_DWORD_1((_payload).dword[_ofst], \
                    CI_DWORD_0, (_val))

#define PAYLOAD_DWORD(_payload, _ofst) \
  CI_DWORD_FIELD((_payload).dword[_ofst], CI_DWORD_0)

#define PROC_BUS_PATH       "/proc/bus/pci/"
#define DEV_PATH            "/dev/"
#define PATH_MAX_LEN        1024
#define CMD_MAX_LEN         1024
#define SFU_STR_MAX_LEN     1024
#define VPD_FIELD_MAX_LEN   255
#define PCI_CONF_LEN        12
#define CLASS_NET_VALUE     0x20000
#define VENDOR_SF_VALUE     0x1924 
#define CHAR2INT(x)         ((int)(x & 0x000000ff))

#define BUF_MAX_LEN                     32

#define VPD_TAG_ID   0x02
#define VPD_TAG_END  0x0f
#define VPD_TAG_R    0x10
#define VPD_TAG_W    0x11

extern "C" {
    extern int sfupdate_main(int argc, char *argv[]);
}

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
    static int isSpace(char c)
    {
        if (c == ' ' || c == '\t' || c == '\0')
            return 1;
        else
            return 0;
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

        if (s == NULL)
            return;

        len = strlen(s);

        for (i = 0; i < len; i++)
        {
            c1 = c2;
            c2 = s[i];

            if ((c1 != '\\' && (c2 == '\'' || c2 == '"')) ||
                (c1 == '\\' && c2 =='\\') ||
                (c1 == '\\' && c2 == '"'))
            {
                if (c1 == '\\' && c2 == '"')
                    i--;

                for (j = i; j < len; j++)
                    s[j] = s[j + 1];
                len--;
                c2 = s[i];
            }
        }
    }

    /**
     * Parse command line string to obtain array of arguments of
     * the form passed to main() normally.
     *
     * @param s     Command line to be parsed
     * @param argc  Where to save number of arguments
     * @param argv  Where to save pointer to the array of
     *              arguments (allocated as a single block in
     *              memory)
     *
     * @return 0 on success or value less than 0 on error
     */
    static int parseArgvString(const char *s, int *argc, char ***argv)
    {
        char  c1 = 0;
        char  c2 = 0;
        int   i;
        int   len;
        int   single_quote = 0;
        int   double_quote = 0;
        char *start;

        char **args = NULL;
        int    count = 0;
        int    max_count = 0;
        char  *buf;

        int rc = 0;

        if (s == NULL || argc == NULL || argv == NULL)
            return -1;

        buf = strdup(s);
        len = strlen(buf);

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
                        void *p = NULL;

                        max_count = (max_count + 1) * 2;
                        p = realloc(args, sizeof(char **) * max_count);
                        if (p == NULL)
                        {
                            rc = -1;
                            goto fail;
                        }
                        args = (char **)p;
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
            rc = -2;
            goto fail;
        }

        args[count] = NULL;

        *argv = (char **)calloc(1, (count + 1) * sizeof(char **) + len);
        memcpy(((char *)*argv) + (count + 1) * sizeof(char **), buf, len);

        for (i = 0; i < count; i++)
            (*argv)[i] = args[i] +
                          (((char *)*argv) +
                                      (count + 1) * sizeof(char **) - buf);

        *argc = count;

fail:

        free(args);
        free(buf);
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
        uint8_t u8[MCDI_CTL_SDU_LEN_MAX];
        uint16_t u16[MCDI_CTL_SDU_LEN_MAX/2];
        ci_dword_t dword[MCDI_CTL_SDU_LEN_MAX/4];
    } payload_t;

    /**
     * Description of Ethernet port.
     */
    typedef struct PortDescr {
        int     pci_fn;     /**< PCI function */
        char   *dev_file;   /**< Path to device file */
        char   *dev_name;   /**< Device name */
    } PortDescr;

    /**
     * Description of NIC.
     */
    typedef struct NICDescr {
        int pci_domain;                 /**< PCI domain ID */
        int pci_bus;                    /**< PCI bus ID */
        int pci_device;                 /**< PCI device ID */

        PortDescr   *ports;             /**< Array with Ethernet ports */
        int          ports_count;       /**< Number of elements in the
                                             array */
        int          ports_max_count;   /**< For how many elements space was
                                             allocated memory in the
                                             array */
    } NICDescr;

    /**
     * Free array with NIC descriptions.
     *
     * @param nics      Array pointer
     * @param count     Number of elements in the array
     */
    static void freeNICs(NICDescr *nics, int count)
    {
        int i;
        int j;

        for (i = 0; i < count; i++)
        {
            for (j = 0; j < nics[i].ports_count; j++)
            {
                free(nics[i].ports[j].dev_file);
                free(nics[i].ports[j].dev_name);
            }
            free(nics[i].ports);
        }
        free(nics);
    }

    /**
     * Get descriptions for all Solarflare NICs on the machine.
     *
     * @param nics          Where to save NIC descriptions array handle
     * @param nics_count    Where to save number of SF NICs
     *
     * @return 0 on success or error code
     */
    static int getNICs(NICDescr **nics, int *nics_count)
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

        int         *pci_domain_id = NULL;
        int         *pci_bus_id = NULL;
        int         *pci_dev_id = NULL;
        int         *pci_fn_id = NULL;
        char       **dev_name = NULL;
        char       **dev_file = NULL;
        int          dev_count = 0;
        int          dev_max_num = 0;

        int     cur_domain;
        int     cur_bus;
        int     cur_dev;
        int     cur_fn;
        int     i;
        int     j;
        int     k;
        int     fd;

#define DEV_ARRAYS_FREE \
        do {                                                            \
            int i_;                                                     \
            free(pci_domain_id);                                        \
            free(pci_bus_id);                                           \
            free(pci_dev_id);                                           \
            free(pci_fn_id);                                            \
            if (dev_name != NULL)                                       \
            {                                                           \
                for (i_ = 0; i_ < dev_count; i_++)                      \
                {                                                       \
                    free(dev_name[i_]);                                 \
                    free(dev_file[i_]);                                 \
                    dev_name[i_] = NULL;                                \
                    dev_file[i_] = NULL;                                \
                }                                                       \
            }                                                           \
            free(dev_name);                                             \
            free(dev_file);                                             \
        } while (0)

#define DEV_ARRAYS_REALLOC \
        do {                                                             \
            void    *tmp_;                                               \
                                                                         \
            if (dev_max_num == 0)                                        \
                dev_max_num = 10;                                        \
            else                                                         \
                dev_max_num *= 2;                                        \
                                                                         \
            if (!((tmp_ = realloc(pci_domain_id,                         \
                                  dev_max_num * sizeof(int))) != NULL && \
                  (pci_domain_id = (int *)tmp_) != NULL &&               \
                  (tmp_ = realloc(pci_bus_id,                            \
                                  dev_max_num * sizeof(int))) != NULL && \
                  (pci_bus_id = (int *)tmp_) != NULL &&                  \
                  (tmp_ = realloc(pci_dev_id,                            \
                                  dev_max_num * sizeof(int))) != NULL && \
                  (pci_dev_id = (int *)tmp_) != NULL &&                  \
                  (tmp_ = realloc(pci_fn_id,                             \
                                  dev_max_num *                          \
                                            sizeof(int))) != NULL &&     \
                  (pci_fn_id = (int *)tmp_) != NULL &&                   \
                  (tmp_ = realloc(dev_name,                              \
                                  dev_max_num *                          \
                                        sizeof(char *))) != NULL &&      \
                  (dev_name = (char **)tmp_) != NULL &&                  \
                  (tmp_ = realloc(dev_file,                              \
                                  dev_max_num *                          \
                                        sizeof(char *))) != NULL &&      \
                  (dev_file = (char **)tmp_) != NULL))                   \
            {                                                            \
                DEV_ARRAYS_FREE;                                         \
                return -2;                                               \
            }                                                            \
        } while (0)

        struct ifconf   ifconf;
        struct ifreq    ifr;
        int             s;

        char    *str;

        struct ethtool_drvinfo drvinfo;

        NICDescr   *nics_arr = NULL;
        int          nics_num = 0;
        int          nics_max_num = 0;
        void        *tmp;

        if (nics == NULL || nics_count == NULL)
            return -1;

        *nics = NULL;
        *nics_count = 0;

        device_dir = opendir(DEV_PATH);
        if (device_dir == NULL)
            return -3;

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

                if (dev_count >= dev_max_num)
                    DEV_ARRAYS_REALLOC;

                dev_name[dev_count] = strdup(device->d_name);
                if (dev_name[dev_count] == NULL)
                {
                    DEV_ARRAYS_FREE;
                    closedir(device_dir);
                    return -4;
                }
                str = strchr(drvinfo.bus_info, '.');
                if (str == NULL)
                    continue;
                pci_fn_id[dev_count] = strtol(str + 1, NULL, 16);
                *str = '\0';
                str = strrchr(drvinfo.bus_info, ':');
                if (str == NULL)
                {
                    DEV_ARRAYS_FREE;
                    closedir(device_dir);
                    return -5;
                }
                pci_dev_id[dev_count] = strtol(str + 1, NULL, 16);
                *str = '\0';
                str = strrchr(drvinfo.bus_info, ':');
                if (str == NULL)
                {
                    pci_domain_id[dev_count] = 0;
                    pci_bus_id[dev_count] = strtol(drvinfo.bus_info,
                                                   NULL, 16);
                }
                else
                {
                    pci_bus_id[dev_count] = strtol(str + 1,
                                                   NULL, 16);
                    *str = '\0';
                    pci_domain_id[dev_count] = strtol(drvinfo.bus_info,
                                                      NULL, 16);
                }
                dev_file[dev_count] = strdup(device_path);
                dev_count++;
            }
            else
                close(fd);
        }

        closedir(device_dir);
        
        bus_dir = opendir(PROC_BUS_PATH);
        if (bus_dir == NULL)
        {
            DEV_ARRAYS_FREE;
            return -6;
        }

        for (bus = readdir(bus_dir);
             bus != NULL;
             bus = readdir(bus_dir))
        {
            if (bus->d_name[0] == '.')
                continue;
            if (snprintf(device_path, PATH_MAX_LEN, "%s/%s",
                         PROC_BUS_PATH, bus->d_name) >= PATH_MAX_LEN)
                continue;

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
                    DEV_ARRAYS_FREE;
                    freeNICs(nics_arr, nics_num);
                    return -7;
                }
                cur_bus = strtol(str + 1, NULL, 16);
            }

            for (device = readdir(device_dir);
                 device != NULL;
                 device = readdir(device_dir))
            {
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
                fread(pci_conf, 1, PCI_CONF_LEN, f);
                fclose(f);
                vendor_id = CHAR2INT(pci_conf[1]) * 256 +
                            CHAR2INT(pci_conf[0]);
                device_class = (CHAR2INT(pci_conf[11]) << 16) +
                               (CHAR2INT(pci_conf[10]) << 8) +
                               CHAR2INT(pci_conf[9]);

                if (vendor_id != VENDOR_SF_VALUE ||
                    device_class != CLASS_NET_VALUE)
                    break;

                for (i = 0; i < dev_count; i++)
                {
                    if (cur_domain == pci_domain_id[i] &&
                        cur_bus == pci_bus_id[i] &&
                        cur_dev == pci_dev_id[i] &&
                        cur_fn == pci_fn_id[i])
                        break;
                }
                
                if (i == dev_count)
                {
                    closedir(device_dir);
                    closedir(bus_dir);
                    DEV_ARRAYS_FREE;
                    freeNICs(nics_arr, nics_num);
                    return -8;
                }

                for (j = 0; j < nics_num; j++)
                {
                    if (nics_arr[j].pci_domain == cur_domain &&
                        nics_arr[j].pci_bus == cur_bus &&
                        nics_arr[j].pci_device == cur_dev)
                        break;
                }

                if (j == nics_num)
                {
                    if (nics_num >= nics_max_num)
                    {
                        nics_max_num = (nics_max_num + 1) * 2;
                        tmp = realloc(nics_arr, nics_max_num *
                                                sizeof(NICDescr));
                        if (tmp == NULL)
                        {
                            closedir(device_dir);
                            closedir(bus_dir);
                            DEV_ARRAYS_FREE;
                            freeNICs(nics_arr, nics_num);
                            return -9;
                        }
                        nics_arr = (NICDescr *)tmp;
                    }

                    nics_arr[nics_num].pci_domain = cur_domain;
                    nics_arr[nics_num].pci_bus = cur_bus;
                    nics_arr[nics_num].pci_device = cur_dev;
                    nics_arr[nics_num].ports = NULL;
                    nics_arr[nics_num].ports_count = 0;
                    nics_arr[nics_num].ports_max_count = 0;
                    nics_num++;
                }

                if (nics_arr[j].ports_count >= nics_arr[j].ports_max_count)
                {
                    nics_arr[j].ports_max_count =
                                    (nics_arr[j].ports_max_count + 1) * 2;
                    tmp = realloc(nics_arr[j].ports,
                                  nics_arr[j].ports_max_count *
                                  sizeof(PortDescr));
                    if (tmp == NULL)
                    {
                        closedir(device_dir);
                        closedir(bus_dir);
                        DEV_ARRAYS_FREE;
                        freeNICs(nics_arr, nics_num);
                        return -10;
                    }
                    nics_arr[j].ports = (PortDescr *)tmp;
                }

                k = nics_arr[j].ports_count;
                nics_arr[j].ports[k].pci_fn = cur_fn;
                nics_arr[j].ports[k].dev_file = strdup(dev_file[i]);
                nics_arr[j].ports[k].dev_name = strdup(dev_name[i]);
                nics_arr[j].ports_count++;
            }

            closedir(device_dir);
        }

        closedir(bus_dir);
        DEV_ARRAYS_FREE;

        /*
         * Sort ports in ascending order in relation to PCI function.
         */
        for (i = 0; i < nics_num; i++)
        {
            PortDescr tmp_port;

            do {
                k = 0;
                for (j = 0; j < nics_arr[i].ports_count - 1; j++)
                {
                    if (nics_arr[i].ports[j].pci_fn >
                            nics_arr[i].ports[j + 1].pci_fn)
                    {
                        tmp_port.pci_fn = nics_arr[i].ports[j].pci_fn;
                        tmp_port.dev_file = nics_arr[i].ports[j].dev_file;
                        tmp_port.dev_name = nics_arr[i].ports[j].dev_name;
                        nics_arr[i].ports[j].pci_fn =
                                        nics_arr[i].ports[j + 1].pci_fn;
                        nics_arr[i].ports[j].dev_file =
                                        nics_arr[i].ports[j + 1].dev_file;
                        nics_arr[i].ports[j].dev_name =
                                        nics_arr[i].ports[j + 1].dev_name;
                        nics_arr[i].ports[j + 1].pci_fn = tmp_port.pci_fn;
                        nics_arr[i].ports[j + 1].dev_file =
                                                        tmp_port.dev_file;
                        nics_arr[i].ports[j + 1].dev_name =
                                                        tmp_port.dev_name;
                        k++;
                    }
                }
            } while (k > 0);
        }

        *nics = nics_arr;
        *nics_count = nics_num;

        return 0;

#undef DEV_ARRAYS_FREE
#undef DEV_ARRAYS_REALLOC
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
     * Parse VPD tag.
     *
     * @param vpd       Pointer to buffer with VPD data
     * @param len       Length of VPD data
     * @param tag_name  Where to save tag name
     * @param tag_len   Where to save tag data len
     * @param tag_start Where to save address of the first
     *                  byte of tag data
     *
     * @return 0 on success or error code
     */
    static int getVPDTag(uint8_t *vpd, uint32_t len,
                         int *tag_name, unsigned int *tag_len,
                         uint8_t **tag_start)
    {
        if (vpd[0] & 0x80) // 10000000b
        {
             if (len < 2)
                return -1;
            *tag_name = vpd[0] & 0x7f; // 01111111b
            *tag_len = (vpd[1] & 0x000000ff) +
                       ((vpd[2] & 0x000000ff) >> 8);
            *tag_start = vpd + 3;
            if (len < *tag_len + 3)
                return -2;
        }
        else
        {
            *tag_name = ((vpd[0] & 0xf8) >> 3); // 01111000b
            *tag_len = (vpd[0] & 0x07); // 00000111b
            *tag_start = vpd + 1;
            if (len < *tag_len + 1)
                return -3;
        }

        return 0;
    }

    /**
     * Parse VPD data.
     *
     * @param vpd               Buffer with VPD data
     * @param len               Length of VPD data
     * @param product_name      Where to save product name
     * @param product_number    Where to save product number
     * @param serial_number     Where to save serial number
     *
     * @return 0 on success or error code
     */
    static int parseVPD(uint8_t *vpd, uint32_t len,
                        char **product_name, char **product_number,
                        char **serial_number)
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

        *product_name = NULL;
        *product_number = NULL;
        *serial_number = NULL;

        while (vpd < end)
        {
            if ((rc = getVPDTag(vpd, len, &tag_name,
                                &tag_len, &tag_start)) < 0)
                return rc;

            switch (tag_name)
            {
                case VPD_TAG_ID:
                    *product_name = (char *)calloc(1, tag_len + 1);
                    memcpy(*product_name, tag_start, tag_len);
                    (*product_name)[tag_len] = '\0';

                    break;

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
                        field_value = (char *)calloc(1, field_len + 1);
                        memcpy(field_value, vpd_field + 3, field_len);
                        field_value[field_len] = '\0';

                        if (strcmp(field_name, "PN") == 0)
                            *product_number = strdup(field_value);
                        else if (strcmp(field_name, "SN") == 0)
                            *serial_number = strdup(field_value);
                        else if (strcmp(field_name, "RV") == 0 &&
                            tag_name == VPD_TAG_R)
                        {
                            if (field_len < 1)
                            {
                                free(field_value);
                                return -10;
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

                        free(field_value);

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
                return -11;

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
                return -2;
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
                      char **product_name, char **product_number,
                      char **serial_number)
    {
        int       rc;
        int       fd;
        uint8_t  *vpd;
        int       vpd_off;
        int       vpd_len;

        struct efx_mcdi_request *mcdi_req;
        struct efx_ioctl         ioc;
        payload_t                payload;

        siena_mc_static_config_hdr_t partial_hdr;

        fd = open("/dev/sfc_control", O_RDWR);
        if (fd < 0)
            return -1;

        if (readNVRAMBytes(fd, (uint8_t *)&partial_hdr, 0,
                           sizeof(partial_hdr),
                           ifname, port_number) < 0)
        {
            close(fd);
            return -2;
        }

        if (CI_BSWAP_LE32(partial_hdr.magic) !=
                                    SIENA_MC_STATIC_CONFIG_MAGIC)
        {
            close(fd);
            return -3;
        }

        vpd_off = CI_BSWAP_LE32(partial_hdr.static_vpd_offset);
        vpd_len = CI_BSWAP_LE32(partial_hdr.static_vpd_length);

        vpd = (uint8_t *)calloc(1, vpd_len);
        if (readNVRAMBytes(fd, vpd, vpd_off, vpd_len,
                           ifname, port_number) < 0)
        {
            close(fd);
            free(vpd);
            return -4;
        }

        if ((rc = parseVPD(vpd, vpd_len, product_name,
                           product_number, serial_number)) < 0)
        {
            close(fd);
            free(product_name);
            free(product_number);
            free(serial_number);
            free(vpd);
            return -5;
        }

        free(vpd);
        close(fd);

        return 0;
    }

    /**
     * Wrapper function working like popen("sfupdate ...").
     *
     * @param cmd_line Command line (should start with "sfupdate")
     *
     * @return FD from which output of sfupdate can be read on
     *         succens or negative value on failure
     */
    static int sfupdatePOpen(const char *cmd_line)
    {
        int     argc;
        char  **argv = NULL;
        int     saved_stdout_fd = -1;
        int     saved_stderr_fd = -1;
        int     pipefds[2] = {-1, -1};
        int     rc = 0;
        int     restore_stdout = 0;
        int     restore_stderr = 0;

        static pthread_mutex_t sfupdate_mutex = PTHREAD_MUTEX_INITIALIZER;

        if (parseArgvString(cmd_line, &argc, &argv) < 0 ||
            argv == NULL)
            return -1;

        saved_stdout_fd = dup(STDOUT_FILENO);
        if (saved_stdout_fd < 0)
        {
            rc = -2;
            goto fail;
        }

        saved_stderr_fd = dup(STDERR_FILENO);
        if (saved_stderr_fd < 0)
        {
            rc = -3;
            goto fail;
        }

        if (pipe(pipefds) < 0)
        {
            rc = -4;
            goto fail;
        }

        fflush(stdout);
        fflush(stderr);

        if (dup2(pipefds[1], STDOUT_FILENO) < 0)
        {
            rc = -7;
            goto fail;
        }
        else
            restore_stdout = 1;

        if (dup2(pipefds[1], STDERR_FILENO) < 0)
        {
            rc = -8;
            goto fail;
        }
        else
            restore_stderr = 1;

        pthread_mutex_lock(&sfupdate_mutex);
        if ((rc = sfupdate_main(argc, argv)) != 0)
        {
            pthread_mutex_unlock(&sfupdate_mutex);
            rc = -9;
            goto fail;
        }
        pthread_mutex_unlock(&sfupdate_mutex);

    fail:
        fflush(stdout);
        fflush(stderr);

        free(argv);

        if (restore_stdout > 0)
            if (dup2(saved_stdout_fd, STDOUT_FILENO) < 0 && rc == 0)
                rc = -15;
        if (restore_stderr > 0)
            if (dup2(saved_stderr_fd, STDERR_FILENO) < 0 && rc == 0)
                rc = -16;
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
                    dev_file(descr.dev_file), dev_name(descr.dev_name)
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

        fd = open("/dev/sfc_control", O_RDWR);
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
        /* How to implement it?! */

        return false;
    }

    void VMWareInterface::enable(bool st)
    {
        /* How to implement it?! */
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
        if (boundPort == NULL)
            return MACAddress(0, 0, 0, 0, 0, 0);

        return boundPort->permanentMAC();
    }

    void VMWareInterface::currentMAC(const MACAddress& mac)
    {
        /* How to implement it?! */
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
        VersionInfo vers;
    public:
        VMWareBootROM(const NIC *o, const VersionInfo &v) :
            owner(o), vers(v) {}
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

        int portNum;
        VMWarePort **ports;
        VMWareInterface **intfs;

    protected:
        virtual void setupPorts()
        {
            int i = 0;

            for (i = 0; i < portNum; i++)
                ports[i]->initialize();
        }
        virtual void setupInterfaces()
        {
            int i = 0;

            for (i = 0; i < portNum; i++)
            {
                intfs[i]->initialize();
                intfs[i]->bindToPort(ports[i]);
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
            rom(this, VersionInfo("2.3.4")),
            diag(this), pci_domain(descr.pci_domain),
            pci_bus(descr.pci_bus), pci_device(descr.pci_device),
            portNum(0)
        {
            int i = 0;

            ports = (VMWarePort **)calloc(descr.ports_count,
                                          sizeof(VMWarePort *));
            if (ports == NULL)
                return;

            intfs = (VMWareInterface **)calloc(descr.ports_count,
                                            sizeof(VMWareInterface *));
            if (intfs == NULL)
            {
                free(ports);
                return;
            }

            for (i = 0; i < descr.ports_count; i++)
            {
                ports[i] = new VMWarePort(this, i, descr.ports[i]);
                if (ports[i] == NULL)
                    break;
                intfs[i] = new VMWareInterface(this, i);
                if (intfs[i] == NULL)
                    break;
            }

            if (i < descr.ports_count)
            {
                int j;

                for (j = 0; j <= i; j++)
                {
                    free(ports[j]);
                    free(intfs[j]);
                }

                free(ports);
                free(intfs);

                return;
            }

            portNum = descr.ports_count;
        }

        ~VMWareNIC()
        {
            int i;

            for (i = 0; i < portNum; i++)
                delete ports[i];

            free(ports);
        }

        virtual VitalProductData vitalProductData() const;
        Connector connector() const;

        uint64 supportedMTU() const { return 9000; }

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

            for (i = 0; i < portNum; i++)
                if (!en.process(*(ports[i])))
                    return false;

            return true;
        }
        
        virtual bool forAllPorts(ConstElementEnumerator& en) const
        {
            int i = 0;

            for (i = 0; i < portNum; i++)
                if (!en.process(*(ports[i])))
                    return false;

            return true;
        }

        virtual bool forAllInterfaces(ElementEnumerator& en)
        {
            int i = 0;

            for (i = 0; i < portNum; i++)
                if (!en.process(*(intfs[i])))
                    return false;

            return true;
        }
        
        virtual bool forAllInterfaces(ConstElementEnumerator& en) const
        {
            int i = 0;

            for (i = 0; i < portNum; i++)
                if (!en.process(*(intfs[i])))
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
        if (portNum <= 0)
            return VitalProductData("", "", "", "", "", "");
        else
        {
            char    *p_name;
            char    *pn;
            char    *sn;

            if (getVPD(ports[0]->dev_name.c_str(), 0,
                       &p_name, &pn, &sn) < 0)
                return VitalProductData("", "", "", "", "", "");

            VitalProductData vpd(sn, "", sn, pn, p_name, pn /* ??? */);

            free(p_name);
            free(pn);
            free(sn);

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
            return -2;

        if (curl_easy_setopt(curl, CURLOPT_URL, uri) != CURLE_OK)
        {
            rc = -3;
            goto curl_fail;
        }
        if (curl_easy_setopt(curl, CURLOPT_WRITEDATA,
                             f) != CURLE_OK)
        {
            rc = -5;
            goto curl_fail;
        }

        CURLcode rc_curl;
        if ((rc_curl = curl_easy_perform(curl)) != CURLE_OK)
        {
            rc = -6;
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
        int   rc = 0;
        char  cmd[CMD_MAX_LEN];
        int   fd = -1;
        char  tmp_file[] = "/tmp/sf_firmware_XXXXXX";
        int   tmp_file_used = 0;

        if (((VMWareNIC *)owner)->portNum <= 0)
            return -1;

        if (strcmp_start(fileName, "file://") == 0)
        {
            rc = snprintf(cmd, CMD_MAX_LEN, "sfupdate --adapter=%s "
                          "--write --image=%s",
                          ((VMWareNIC *)owner)->ports[0]->dev_name.c_str(),
                          fileName + strlen("file://"));
            if (rc < 0 || rc >= CMD_MAX_LEN)
            {
                return -2;
            }
        }
        else if (strcmp_start(fileName, "tftp://") == 0)
        {
            FILE *f;

            fd = mkstemp(tmp_file);
            if (fd < 0)
                return -3;

            f = fdopen(fd, "w");
            if (f == NULL)
            {
                close(fd);
                return -4;
            }

            rc = tftp_get_file(fileName, f);
            if (rc != 0)
            {
                fclose(f);
                return -5;
            }
            fclose(f);

            rc = snprintf(cmd, CMD_MAX_LEN, "sfupdate --adapter=%s "
                          "--write --image=%s",
                          ((VMWareNIC *)owner)->ports[0]->dev_name.c_str(),
                          tmp_file);
            if (rc < 0 || rc >= CMD_MAX_LEN)
            {
                unlink(tmp_file);
                return -6;
            }

            tmp_file_used = 1;
        }
        else /* SFTP to be implemented */
            return -7;

        fd = sfupdatePOpen(cmd);
        if (fd < 0)
        {
            if (tmp_file_used)
                unlink(tmp_file);
            return -8;
        }

        close(fd);
        if (tmp_file_used)
            unlink(tmp_file);

        return 0;
    }

    class VMWareDriver : public Driver {
        const Package *owner;
        VersionInfo vers;
        static const String drvName;
    public:
        VMWareDriver(const Package *pkg, const String& d, const String& sn, 
                     const VersionInfo& v) :
            Driver(d, sn), owner(pkg), vers(v) {}
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
            return VersionInfo("");

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
         * We failed to get it via ethtool - we try to get it
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
            return VersionInfo("");

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
            return VersionInfo("");

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
            kernelDriver(this, "NET Driver", "sfc", "3.3") {}
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
        VMWareNIC **sf_nics;
        int sf_nics_count;

        VMWareKernelPackage kernelPackage;
        VMWareManagementPackage mgmtPackage;

        VMWareSystem()
        {
            sf_nics = NULL;
            sf_nics_count = 0;

            curl_global_init(CURL_GLOBAL_ALL);
        };

        ~VMWareSystem()
        {
            if (sf_nics != NULL)
            {
                int i;

                for (i = 0; i < sf_nics_count; i++)
                    delete sf_nics[i];
                free(sf_nics);
                sf_nics_count = 0;
            }

            curl_global_cleanup();
        };

    protected:
        void setupNICs();

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

    void VMWareSystem::setupNICs()
    {
        NICDescr   *nics;
        int         count;
        int         i;
        int         rc;

        if ((rc = getNICs(&nics, &count)) < 0)
            return;

        if (count != sf_nics_count)
        {
            for (i = 0; i < sf_nics_count; i++)
                delete sf_nics[i];
            free(sf_nics);
            sf_nics_count = 0;

            sf_nics = (VMWareNIC **)calloc(count, sizeof(*sf_nics));
            if (sf_nics == NULL)
            {
                freeNICs(nics, count);
                return;
            }

            for (i = 0; i < count; i++)
            {
                sf_nics[i] = new VMWareNIC(i, nics[i]);
                sf_nics[i]->initialize();
            }

            sf_nics_count = count;
            freeNICs(nics, count);
        }
        else
            freeNICs(nics, count);
    }

    bool VMWareSystem::forAllNICs(ConstElementEnumerator& en) const
    {
        int         i;
        bool        res;
        bool        ret;

        for (i = 0; i < sf_nics_count; i++)
        {
            res = en.process(*(sf_nics[i]));
            ret = ret && res;
        }

        return true;
    }

    bool VMWareSystem::forAllNICs(ElementEnumerator& en)
    {
        setupNICs();

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

        if (((VMWareNIC *)owner)->portNum <= 0)
            return VersionInfo("");

        if (vmwareEthtoolCmd(((VMWareNIC *)owner)->
                                        ports[0]->dev_file.c_str(),
                             ((VMWareNIC *)owner)->
                                        ports[0]->dev_name.c_str(),
                             ETHTOOL_GDRVINFO, &edata) < 0)
            return VersionInfo("");

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
                      ((VMWareNIC *)owner)->ports[0]->dev_name.c_str());
        if (rc < 0 || rc >= CMD_MAX_LEN)
            return VersionInfo("");

        fd = sfupdatePOpen(cmd);
        if (fd < 0)
            return VersionInfo("");

        f = fdopen(fd, "r");
        if (f == NULL)
        {
            close(fd);
            return VersionInfo("");
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
            return VersionInfo("");
    }
}
