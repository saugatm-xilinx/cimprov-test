/***************************************************************************//*! \file liprovider/sf_vmware.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include "sf_platform.h"
#include "sf_provider.h"
#include <cimple/Buffer.h>
#include <cimple/Strings.h>
#include <cimple/Array.h>
#include <cimple/Ref.h>
#include <cimple/Time.h>
#include <cimple.h>
#include "CIM_EthernetPort.h"
#include "CIM_SoftwareIdentity.h"
#include "VMware_KernelModuleService.h"

#if defined(TARGET_CIM_SERVER_esxi_native)
#include "sf_native_vmware.h"
#define VMK_DRIVER_NAME "sfvmk"
#else
#define VMK_DRIVER_NAME "sfc"
#endif

#include <stdint.h>

extern "C" {
#include "efx_ioctl.h"
#include "ci/mgmt/mc_flash_layout.h"
#include "efx_regs_mcdi.h"
#include "ci/tools/byteorder.h"
#include "ci/tools/bitfield.h"
#include "image.h"

#include "tlv_partition.h"
#include "tlv_layout.h"
#include "endian_base.h"

#ifdef TARGET_CIM_SERVER_esxi_native
#include "efx.h"
#endif
}

#include "sf_siocefx_common.h"
#include "sf_siocefx_nvram.h"

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

#include <cimple/Mutex.h>
#include <cimple/Auto_Mutex.h>
#include <cimple/Cond.h>
#include <cimple/CimpleConfig.h>

#include "sf_mcdi_sensors.h"
#include "sf_alerts.h"
#include "sf_pci_ids.h"
#include "sf_locks.h"
#include "sf_native_nv.h"
#include "sf_native_device.h"

#include "sf_base64.h"

#include <sys/syscall.h>

#include "openssl/evp.h"

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

// Length Definitions for VPD Info Structure
#define VPD_KEYWORD_LEN 3
#define VPD_FIELDNAME_LEN 6

// Default version string
#define DEFAULT_VERSION_STR ""

// Protocols to be used to obtain firmware image
#define FILE_PROTO "file://"
#define TFTP_PROTO "tftp://"
#define FTP_PROTO "ftp://"
#define SFTP_PROTO "sftp://"
#define HTTP_PROTO "http://"
#define HTTPS_PROTO "https://"

// Defines for sfu_device array string representation
#define NETIF_NAME_PREF   "netif_name:"
#define MAC_ADDR_PREF     "mac_addr:"
#define SERIAL_NUM_PREF   "serial_number:"
#define PCI_DEVID_PREF    "pci_device_id:"
#define PCI_SUBSYSID_PREF "pci_subsystem_id:"
#define PCI_ADDR_PREF     "pci_addr:"
#define PORT_INDEX_PREF   "port_index:"
#define PHY_TYPE_PREF     "phy_type:"
#define MASTER_PORT_PREF  "master_port:"
#define PORTS_COUNT_PREF  "ports_count:"
// Next sfu_device record
#define NEXT_PREF         "NEXT"

// Maximum number of stored nv_context pointers
#define MAX_NV_CTX_ID 1000

// Maximum MC Reboot Timeout value
#define CIMPROV_MC_REBOOT_TIME_OUT_SEC 30

extern "C" {
    extern int sfupdate_main(int argc, char *argv[]);
}

using namespace std;

namespace solarflare
{
    using cimple::Instance;
    using cimple::CIM_EthernetPort;
    using cimple::CIM_SoftwareIdentity;
    using cimple::VMware_KernelModuleService;
    using cimple::VMware_KernelModuleService_GetModuleLoadParameter_method;
    using cimple::VMware_KernelModuleService_SetModuleLoadParameter_method;
    using cimple::Ref;
    using cimple::Array;
    using cimple::cast;
    using cimple::Mutex;
    using cimple::Auto_Mutex;
    using cimple::Cond;
    using cimple::Time;

    ///
    /// Structure for storing nv_context pointers
    ///
    typedef class NVContextDescr {
    public:
        nv_context   *ctx;
        unsigned int         id;

        // This is required to use cimple::Array
        bool operator== (const NVContextDescr &rhs)
        {
            return (ctx == rhs.ctx &&
                    id == rhs.id);
        }
    } NVContextDescr;

    Array<NVContextDescr> NVCtxArr;

    // Get index of element in NVCtxArray having
    // a given id.
    //
    // @param ctx_id      Id to be found
    //
    // @return index in NVCtxArr on success,
    //         -1 on failure
    static int
    findNVCtxById(unsigned int ctx_id)
    {
        unsigned int i;

        for (i = 0; i < NVCtxArr.size(); i++)
        {
            if (NVCtxArr[i].id == ctx_id)
                return i;
        }

        PROVIDER_LOG_ERR("%s(): failed to find NV context id %d",
                         __FUNCTION__, ctx_id);
        return -1;
    }

    static Mutex NVCtxArrLock(false);

    ///
    /// Temporary file description
    ///
    typedef class TmpFileDescr {
    public:
        String    fileName;       ///< File name
        uint64    creationTime;   ///< Creation time

        // This is required to use cimple::Array
        bool operator== (const TmpFileDescr &rhs)
        {
            return (fileName == rhs.fileName &&
                    creationTime == rhs.creationTime);
        }
    } TmpFileDescr;

    Array<TmpFileDescr> tmpFilesArr;

    static Mutex tmpFilesArrLock(false);

    ///
    /// Is a symbol a space?
    ///
    /// @param c   symbol
    ///
    /// @result 1 if space, 0 otherwise
    ///
    static int isSpace(const char c)
    {
        if (c == ' ' || c == '\t' || c == '\0')
            return 1;
        else
            return 0;
    }

    ///
    /// Remove space symbols from the end of a string.
    ///
    /// @param s     Char array pointer
    ///
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

    ///
    /// Compare the beginning of the first string with
    /// the second one.
    ///
    /// @param x   The first string
    /// @param y   The second string
    ///
    /// @return The same as strncmp(x, y, strlen(y))
    ///
    static inline int strcmp_start(const char *x, const char *y)
    {
        return strncmp(x, y, strlen(y));
    }

    ///
    /// Compare the beginning of the first string with
    /// the second one, ignoring the case of the characters.
    ///
    /// @param x   The first string
    /// @param y   The second string
    ///
    /// @return The same as strncasecmp(x, y, strlen(y))
    ///
    static inline int strcasecmp_start(const char *x, const char *y)
    {
        return strncasecmp(x, y, strlen(y));
    }

    ///
    /// Copies string from souce to destination with
    /// checking of length of destination string to be more than
    //  source string plus one for terminating character '\0'
    ///
    /// @param dest    The destination string
    /// @param src     The source string
    /// @param destLen Length of destination string
    ///
    /// @return        The number of characters copied  or -1 for error
    ///
    static inline int strncpy_check(char *dest, const char *src, int destLen)
    {
        int srcLen = strlen(src);
        if (destLen >= (srcLen + 1))
        {
            strncpy(dest,src,srcLen);
            dest[srcLen] = '\0';
            return srcLen;
        }
        return -1;
    }

    ///
    /// Remove quotation symbols and related escaping
    /// from the string.
    ///
    /// @param s   String to be processed
    ///
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

    ///
    /// Parse command line string to obtain array of arguments of
    /// the form passed to the main function of a Linux program normally.
    ///
    /// @param s       Command line to be parsed
    /// @param argc    [out] Where to save number of arguments
    /// @param argv    [out] Where to save pointer to the array of
    ///                arguments
    /// @param values  [out] Where to save pointer to the buffer
    ///                with values of arguments
    ///
    /// @return 0 on success or value less than 0 on error
    ///
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
        {
            PROVIDER_LOG_ERR("%s(): incorrect arguments", __FUNCTION__);
            return -1;
        }

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
        PROVIDER_LOG_ERR("%s(): failed to parse '%s'",
                         __FUNCTION__, s);
        delete[] args;
        delete[] buf;
        return rc;
    }

    ///
    /// Get reference to a corresponding CIM_EthernetPort value
    /// by device name.
    ///
    /// @param dev_name    Device name
    ///
    /// @return Reference to CIM_EthernetPort value
    ///
    static Ref<CIM_EthernetPort> getCIMEthPort(const char *dev_name)
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

#if 0
    ///
    /// Debugging function checking number of free FDs.
    ///
    /// @return number of free FDs
    ///
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
#endif

    ///
    /// Description of Ethernet port.
    ///
    class PortDescr
    {
    public:
        int     pci_fn;     ///< PCI function
        String  dev_file;   ///< Path to device file
        String  dev_name;   ///< Device name

        // Dummy operator to make possible using of cimple::Array
        bool operator== (const PortDescr &rhs)
        {
            UNUSED(rhs);
            return false;
        }
    };

    ///
    /// Description of NIC.
    ///
    class NICDescr
    {
    public:
        int pci_domain;                 ///< PCI domain number
        int pci_bus;                    ///< PCI bus number
        int pci_device;                 ///< PCI device number
        int pci_device_id;              ///< PCI device ID

        Array<PortDescr> ports;         ///< NIC ports

        // Dummy operator to make possible using of cimple::Array
        bool operator== (const NICDescr &rhs)
        {
            UNUSED(rhs);
            return false;
        }
    };

    ///
    /// Vector of NIC descriptions.
    ///
    typedef Array<NICDescr> NICDescrs;

    ///
    /// Device description.
    ///
    class DeviceDescr
    {
    public:
        int     pci_domain_num; ///< PCI domain number
        int     pci_bus_num;    ///< PCI bus number
        int     pci_dev_num;    ///< PCI device number
        int     pci_fn_num;     ///< PCI function number
        String  dev_name;       ///< Device name
        String  dev_file;       ///< Path to device file

        // Dummy operator to make possible using of cimple::Array
        bool operator== (const DeviceDescr &rhs)
        {
            UNUSED(rhs);
            return false;
        }
    };

    ///
    /// Check whether a given name is correct ESXi
    /// interface name.
    ///
    /// @param name  Name to be checked
    ///
    /// @return 0 is name is no correct, 1 otherwise
    ///
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

    /// Linux directory entry structure
    struct linux_dirent {
        long           d_ino;     /// Inode number
        off_t          d_off;     /// Offset to the next linux_dirent
        unsigned short d_reclen;  /// Lenght of this linux_dirent
        char           d_name[];  /// Name of the entry
    };

    ///
    /// Get entries for a given directory.
    ///
    /// @param dirPath    Path to the directory
    /// @param arr        Array of entries to be filled
    ///
    /// @return 0 on Success, -1 on Failure
    ///
    static int getDirContents(const char *dirPath, Array<String> &arr)
    {
#define INIT_BUF_SIZE 1024

        char   *buf = new char[INIT_BUF_SIZE];
        size_t  bufSize = INIT_BUF_SIZE;
        int     fd = -1;
        int     numRead = 0;

        unsigned int bufPos;

        arr.clear();

        fd = open(dirPath, O_RDONLY | O_DIRECTORY);
        if (fd < 0)
        {
            PROVIDER_LOG_ERR("Failed to open '%s' as a directory", dirPath);
            delete[] buf;
            return -1;
        }

        while (1)
        {
            numRead = syscall(SYS_getdents, fd, buf, bufSize);
            if (numRead < 0)
            {
                delete[] buf;
                if (errno == EINVAL)
                {
                    if (bufSize < INIT_BUF_SIZE * 8)
                    {
                        bufSize *= 2;
                        buf = new char[bufSize];
                        continue;
                    }
                    else
                    {
                        PROVIDER_LOG_ERR("Too much space is required "
                                         "for linux_dirent");
                        close(fd);
                        arr.clear();
                        return -1;
                    }
                }
                else
                {
                    PROVIDER_LOG_ERR("getdents() failed for directory '%s'"
                                     ", errno %d ('%s')",
                                     dirPath, errno, strerror(errno));
                    close(fd);
                    arr.clear();
                    return -1;
                }
            }
            else if (numRead == 0)
                break;

            for (bufPos = 0; bufPos < (unsigned int)numRead; )
            {
                linux_dirent *d =
                      reinterpret_cast<linux_dirent *>(buf + bufPos);

                arr.append(String(d->d_name));
                bufPos += d->d_reclen;
            }
        }

        delete[] buf;
        close(fd);
        return 0;
#undef INIT_BUF_SIZE
    }

    ///
    /// Sort ports in ascending order in relation to PCI function.
    ///
    /// @param nics          [in] NIC descriptors list to sort
    ///
    static void sortNICsPorts(NICDescrs &nics)
    {
        int i=0, j=0;

        for (i = 0; i < (int)nics.size(); i++)
        {
            PortDescr tmp_port;

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
                }
            }
        }
    }

#ifdef TARGET_CIM_SERVER_esxi_native
    ///
    /// Get Names for all  NICs on the machine.
    ///
    /// @param ifaceList          [out] NICs name List
    ///
    /// @return 0 on success or -1 in case of error
    ///
    int getNICNameList(sfvmk_ifaceList_t *ifaceList)
    {
        if ((DrvMgmtCall(NULL, SFVMK_CB_IFACE_LIST_GET, ifaceList)) != VMK_OK)
        {
            PROVIDER_LOG_ERR("%s(): Driver Management Call failed", __FUNCTION__);
            return -1;
        }
        if (ifaceList->ifaceCount > SFVMK_MAX_INTERFACE) {
            PROVIDER_LOG_ERR("%s():Invalid interface count %d\n", __FUNCTION__, ifaceList->ifaceCount);
            return -EINVAL;
        }
        return 0;
    }
#endif

#ifdef TARGET_CIM_SERVER_esxi_native
    ///
    /// Check whether a given device type is supported.
    ///
    /// @param type     Device type.
    ///
    /// @return true if device type is supported, false otherwise.
    ///
    static bool isDevTypeSupported(int type)
    {
        if (type >= SFU_DEVICE_TYPE_MEDFORD)
            return true;

        return false;
    }

    ///
    /// Get descriptions for all Solarflare NICs on the machine.
    ///
    /// @param nics          [out] Where to save NIC descriptions
    ///
    /// @return 0 on success or error code
    ///
    static int getNICs(NICDescrs &nics)
    {
        int                  i = 0, j = 0, k = 0;
        sfvmk_ifaceList_t    ifaceList;
        int                  portCount = 0;
        sfvmk_mgmtCbTypes_t  cmd;
        sfvmk_pciInfo_t      pciInfo;

        if (getNICNameList(&ifaceList) < 0)
        {
            PROVIDER_LOG_ERR("%s(): Getting  NIC Name List Failed", __FUNCTION__);
            return -1;
        }
        portCount = ifaceList.ifaceCount;
        for (i = 0; i < portCount; i++)
        {
            NICDescr  tmp_nic;
            PortDescr tmp_port;
            int cur_domain = 0, cur_bus = 0, cur_dev = 0, cur_fn = 0;

            memset(&pciInfo, 0, sizeof(sfvmk_pciInfo_t));
            cmd = SFVMK_CB_PCI_INFO_GET;
            if (DrvMgmtCall(ifaceList.ifaceArray[i].string, cmd, &pciInfo) != VMK_OK)
                continue;

            if (!isDevTypeSupported(pciInfo.deviceId))
                continue;

            if (sscanf(vmk_NameToString(&pciInfo.pciBDF), "%x:%x:%x.%x",
                &cur_domain, &cur_bus, &cur_dev, &cur_fn) != 4)
            {
                PROVIDER_LOG_ERR("%s(): Incorrect BDF info", __FUNCTION__);
                return -1;
            }

            for (j = 0; j < (int)nics.size(); j++)
            {
                if (nics[j].pci_domain > cur_domain ||
                   (nics[j].pci_domain == cur_domain &&
                   (nics[j].pci_bus > cur_bus ||
                   (nics[j].pci_bus == cur_bus &&
                    nics[j].pci_device >= cur_dev))))
                    break;
            }

            if (j == (int)nics.size() ||
                !(nics[j].pci_domain == cur_domain &&
                nics[j].pci_bus == cur_bus &&
                nics[j].pci_device == cur_dev))
            {
                tmp_nic.pci_domain = cur_domain;
                tmp_nic.pci_bus = cur_bus;
                tmp_nic.pci_device = cur_dev;
                tmp_nic.pci_device_id = pciInfo.deviceId;
                nics.insert(j, tmp_nic);
            }

            tmp_port.pci_fn = cur_fn;
            tmp_port.dev_file = ""; //Set to Empty as file path not to be used in NativeDrvier.
            tmp_port.dev_name = ifaceList.ifaceArray[i].string;
            nics[j].ports.append(tmp_port);
        }

        sortNICsPorts(nics);

        return 0;
    }

#else
    ///
    /// Check whether a given device type is supported.
    ///
    /// @param type     Device type.
    ///
    /// @return true if device type is supported, false otherwise.
    ///
    static bool isDevTypeSupported(int type)
    {
        if (type == SFU_DEVICE_TYPE_SIENA ||
            type == SFU_DEVICE_TYPE_HUNTINGTON ||
            type == SFU_DEVICE_TYPE_MEDFORD)
            return true;
        return false;
    }

    static int getNICs(NICDescrs &nics)
    {
        char                 device_path[PATH_MAX_LEN];
        char                 func_path[PATH_MAX_LEN];
        char                 pci_conf[PCI_CONF_LEN];
        FILE                *f;
        int                  vendor_id;
        int                  device_id;
        int                  device_class;

        Array<DeviceDescr> devs;

        int          rc;

        int     cur_domain;
        int     cur_bus;
        int     cur_dev;
        int     cur_fn;
        int     i;
        int     j;
        int     k;
        int     fd;

        unsigned int     l;
        unsigned int     m;

        struct ifreq    ifr;

        char    *str;

        struct ethtool_drvinfo drvinfo;

        Array<String> devDirList;
        Array<String> busDirList;

        // Obtain all available network ports.
        if (getDirContents(DEV_PATH, devDirList) < 0)
            return -1;

        for (l = 0; l < devDirList.size(); l++)
        {
            const char *devName = devDirList[l].c_str();
            if (!isPortName(devName))
                continue;

            rc = snprintf(device_path, PATH_MAX_LEN, "%s/%s",
                          DEV_PATH, devName);

            if (rc < 0 || rc >= PATH_MAX_LEN)
            {
                PROVIDER_LOG_ERR("Failed to format path to device");
                return -1;
            }

            fd = open(device_path, O_RDWR);
            if (fd < 0)
                continue;

            memset(&ifr, 0, sizeof(ifr));
            memset(&drvinfo, 0, sizeof(drvinfo));
            drvinfo.cmd = ETHTOOL_GDRVINFO;
            ifr.ifr_data = (char *)&drvinfo;
            strcpy(ifr.ifr_name, devName);
            if (ioctl(fd, SIOCETHTOOL, &ifr) == 0)
            {
                DeviceDescr tmp_dev;

                close(fd);

                tmp_dev.dev_name = devName;
                str = strchr(drvinfo.bus_info, '.');
                if (str == NULL)
                    continue;
                tmp_dev.pci_fn_num = strtol(str + 1, NULL, 16);
                *str = '\0';
                str = strrchr(drvinfo.bus_info, ':');
                if (str == NULL)
                {
                    PROVIDER_LOG_ERR("Invalid bus information "
                                     "for device %s: %s",
                                     tmp_dev.dev_name.c_str(),
                                     drvinfo.bus_info);
                    return -1;
                }
                tmp_dev.pci_dev_num = strtol(str + 1, NULL, 16);
                *str = '\0';
                str = strrchr(drvinfo.bus_info, ':');
                if (str == NULL)
                {
                    tmp_dev.pci_domain_num = 0;
                    tmp_dev.pci_bus_num = strtol(drvinfo.bus_info,
                                                 NULL, 16);
                }
                else
                {
                    tmp_dev.pci_bus_num = strtol(str + 1, NULL, 16);
                    *str = '\0';
                    tmp_dev.pci_domain_num = strtol(drvinfo.bus_info,
                                                    NULL, 16);
                }
                tmp_dev.dev_file = device_path;
                devs.append(tmp_dev);
            }
            else
                close(fd);
        }

        if (getDirContents(PROC_BUS_PATH, busDirList) < 0)
            return -1;

        // Filter out our NIC ports, group them by NICs.

        for (m = 0; m < busDirList.size(); m++)
        {
            const char *busName = busDirList[m].c_str();

            if (busName[0] == '.')
                continue;
            if (strcmp(busName, "devices") == 0)
                continue;

            rc = snprintf(device_path, PATH_MAX_LEN, "%s/%s",
                          PROC_BUS_PATH, busName);
            if (rc < 0 || rc >= PATH_MAX_LEN)
            {
                PROVIDER_LOG_ERR("Failed to format path to device");
                nics.clear();
                return -1;
            }

            if (getDirContents(device_path, devDirList) < 0)
                continue;

            str = const_cast<char *>(strchr(busName, ':'));
            if (str == NULL)
            {
                cur_domain = 0;
                cur_bus = strtol(busName, NULL, 16);
            }
            else
            {
                cur_domain = strtol(busName, &str, 16);
                if (str == NULL || *str != ':')
                {
                    PROVIDER_LOG_ERR("Failed to get PCI domain from %s",
                                     busName);
                    nics.clear();
                    return -1;
                }
                cur_bus = strtol(str + 1, NULL, 16);
            }

            for (l = 0; l < devDirList.size(); l++)
            {
                NICDescr  tmp_nic;
                PortDescr tmp_port;

                const char *devName = devDirList[l].c_str();

                if (devName[0] == '.')
                    continue;

                if (snprintf(func_path, PATH_MAX_LEN, "%s/%s",
                             device_path, devName) >= PATH_MAX_LEN)
                    continue;

                cur_dev = strtol(devName, &str, 16);
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
                device_id = CHAR2INT(pci_conf[3]) * 256 +
                            CHAR2INT(pci_conf[2]);
                device_class = (CHAR2INT(pci_conf[11]) << 16) +
                               (CHAR2INT(pci_conf[10]) << 8) +
                               CHAR2INT(pci_conf[9]);

                if (vendor_id != VENDOR_SF_VALUE ||
                    device_class != CLASS_NET_VALUE ||
                    !isDevTypeSupported(SFU_DEVICE_TYPE(device_id)))
                    break;

                for (i = 0; i < (int)devs.size(); i++)
                {
                    if (cur_domain == devs[i].pci_domain_num &&
                        cur_bus == devs[i].pci_bus_num &&
                        cur_dev == devs[i].pci_dev_num &&
                        cur_fn == devs[i].pci_fn_num)
                        break;
                }

                if (i == (int)devs.size())
                {
                    PROVIDER_LOG_ERR("Failed to find device "
                                     "by PCI address %x:%x:%x.%x",
                                     cur_domain, cur_bus,
                                     cur_dev, cur_fn);
#if 0
                    nics.clear();
                    return -1;
#else
                    // Do not break everything due to this issue
                    // - it was seen on an ESXi 5.5 host
                    continue;
#endif
                }

                // If a new NIC was found - insert it so that
                // NICs are still sorted in ascending order
                // relative to their PCI address.
                for (j = 0; j < (int)nics.size(); j++)
                {
                    if (nics[j].pci_domain > cur_domain ||
                        (nics[j].pci_domain == cur_domain &&
                         (nics[j].pci_bus > cur_bus ||
                          (nics[j].pci_bus == cur_bus &&
                           nics[j].pci_device >= cur_dev))))
                        break;
                }

                if (j == (int)nics.size() ||
                    !(nics[j].pci_domain == cur_domain &&
                      nics[j].pci_bus == cur_bus &&
                      nics[j].pci_device == cur_dev))
                {
                    tmp_nic.pci_domain = cur_domain;
                    tmp_nic.pci_bus = cur_bus;
                    tmp_nic.pci_device = cur_dev;
                    tmp_nic.pci_device_id = device_id;
                    nics.insert(j, tmp_nic);
                }

                tmp_port.pci_fn = cur_fn;
                tmp_port.dev_file = devs[i].dev_file;
                tmp_port.dev_name = devs[i].dev_name;
                nics[j].ports.append(tmp_port);
            }
        }

        sortNICsPorts(nics);
        return 0;
    }
#endif

    ///
    /// Parse VPD tag.
    ///
    /// @param vpd       Pointer to buffer with VPD data
    /// @param len       Length of VPD data
    /// @param tag_name  [out] Where to save tag name
    /// @param tag_len   [out] Where to save tag data len
    /// @param tag_start [out] Where to save address of the first
    ///                  byte of tag data
    ///
    /// @return 0 on success or error code
    ///
    static int getVPDTag(uint8_t *vpd, uint32_t len,
                         int *tag_name, unsigned int *tag_len,
                         uint8_t **tag_start)
    {
        // The first bit shows whether it is large (1) or
        // small (0) resource data type tag bit definition.
        if (vpd[0] & 0x80) // 10000000b
        {
            if (len < 2)
            {
                PROVIDER_LOG_ERR("Incorrect length %u of large "
                                 "resource data type tag", len);
                return -1;
            }
            // Get large item name
            *tag_name = vpd[0] & 0x7f; // 01111111b
            *tag_len = (vpd[1] & 0x000000ff) +
                       ((vpd[2] & 0x000000ff) << 8);
            *tag_start = vpd + 3;
            if (len < *tag_len + 3)
            {
                PROVIDER_LOG_ERR("Too small length %u of large "
                                 "resource data type tag; it "
                                 "should be not less than %u",
                                 len, *tag_len + 3);
                return -1;
            }
        }
        else
        {
            // Small resource data type tag bit definition:
            // 0xxxxyyyb, where xxxx - small item name,
            // yyy - length.
            *tag_name = ((vpd[0] & 0xf8) >> 3); // 01111000b
            *tag_len = (vpd[0] & 0x07); // 00000111b
            *tag_start = vpd + 1;
            if (len < *tag_len + 1)
            {
                PROVIDER_LOG_ERR("Too small length %u of small "
                                 "resource data type tag; it "
                                 "should be not less than %u",
                                 len, *tag_len + 1);
                return -1;
            }
        }

        return 0;
    }

    ///
    /// Parse VPD data.
    ///
    /// @param vpd                     Buffer with VPD data
    /// @param len                     Length of VPD data
    /// @param parsedFields      [out] Where to save parsed VPD fields
    ///
    /// @return 0 on success or error code
    ///
    static int parseVPD(uint8_t *vpd, uint32_t len,
                        Array<VPDField> &parsedFields)
    {
        int             tag_name;
        unsigned int    tag_len;
        uint8_t        *tag_start;
        uint8_t        *end = vpd + len;
        int             rc;

        uint8_t        *vpd_field;
        char            field_name[3];
        unsigned int    field_len;

        uint8_t sum = 0;

        parsedFields.clear();

        while (vpd < end)
        {
            if ((rc = getVPDTag(vpd, len, &tag_name,
                                &tag_len, &tag_start)) < 0)
                return rc;

            switch (tag_name)
            {
                case VPD_TAG_ID:
                {
                    VPDField field;

                    field.name = "IDTag";
                    field.data.append((const char *)tag_start, tag_len);
                    parsedFields.append(field);

                    break;
                }

                case VPD_TAG_R:
                case VPD_TAG_W:
                {
                    sum = 0;
                    for (vpd_field = tag_start;
                         vpd_field < tag_start + tag_len; )
                    {
                        VPDField field;

                        field_name[0] = vpd_field[0];
                        field_name[1] = vpd_field[1];
                        field_name[2] = '\0';

                        field.name = String(field_name);

                        field_len = vpd_field[2];

                        field.data.append(
                              reinterpret_cast<char *>(vpd_field + 3),
                              field_len);
                        parsedFields.append(field);

                        vpd_field += 3 + field_len;
                    }

                    break;
                }
            }

            if (tag_name == VPD_TAG_END)
                break;
            if (tag_name != VPD_TAG_ID &&
                tag_name != VPD_TAG_R &&
                tag_name != VPD_TAG_W &&
                tag_name != VPD_TAG_END)
            {
                PROVIDER_LOG_ERR("Unexpected tag name %d(0x%x) encountered",
                                 tag_name, tag_name);
                return -1;
            }

            vpd = tag_start + tag_len;
            len = end - vpd;

            if (tag_len == 0 || len == 0)
                break;
        }

        return 0;
    }

    /// VPD Info structure
    typedef struct
    {
        uint8_t tagId;                        //< Tag Type can be of ID, Read, Write, End
        char    keyWord[VPD_KEYWORD_LEN];     //< Tag keyword
        char    fieldName[VPD_FIELDNAME_LEN]; //< Tag name
    } vpdInfoData;

    ///
    /// Get VPD.
    ///
    /// @param ifname             Interface name
    /// @param port_number        Port number
    /// @param device_type        Device type (Siena, Huntington, etc)
    /// @param staticVPD          Whether to get static or dynamic VPD
    /// @param parsedFields [out] Parsed VPD fields
    ///
    /// @return 0 on success or error code
    ///
#ifdef TARGET_CIM_SERVER_esxi_native
    static int getVPD(const char *ifname, int port_number,
                      int device_type, bool staticVPD,
                      Array<VPDField> &parsedFields)
    {
         UNUSED(device_type);
         UNUSED(staticVPD);

         vpdInfoData      tagArr[] = {{ VPD_TAG_ID, "", "IDTag" },
                                      { VPD_TAG_R, "PN", "PN" },
                                      { VPD_TAG_R, "SN", "SN" }};
         sfvmk_vpdInfo_t  vpdInfo;
         int              tagCount = sizeof(tagArr)/sizeof(vpdInfoData);
         int              i;

         for (i = 0; i < tagCount; i++)
         {
             memset(&vpdInfo,0,sizeof(sfvmk_vpdInfo_t));
             vpdInfo.vpdOp = SFVMK_MGMT_DEV_OPS_GET;
             vpdInfo.vpdTag = tagArr[i].tagId;
             if (!strlen(tagArr[i].keyWord))
                 vpdInfo.vpdKeyword = 0;
             else
                 vpdInfo.vpdKeyword = ( tagArr[i].keyWord[0] | tagArr[i].keyWord[1] << 8);

             if ((DrvMgmtCall(ifname, SFVMK_CB_VPD_REQUEST, &vpdInfo)) != VMK_OK)
             {
                 PROVIDER_LOG_ERR("%s(): Driver Management Call failed", __FUNCTION__);
                 return -1;
             }
             else
             {
                 VPDField field;
                 field.name = tagArr[i].fieldName;
                 field.data.append((const char *)vpdInfo.vpdPayload, vpdInfo.vpdLen);
                 parsedFields.append(field);
             }
         }

         return 0;
    }


    /// See description in libprovider/sf_native_vmware.h
    int DrvMgmtCall(const char *devName, sfvmk_mgmtCbTypes_t cmdCode, void *cmdParam)
    {
        vmk_MgmtUserHandle mgmtHandle;
        sfvmk_mgmtDevInfo_t mgmtParam = {0};
        bool drv_interaction_status = true;

        if (devName != NULL)
        {
            if (strncpy_check((char *)mgmtParam.deviceName, devName,
                                                  SFVMK_DEV_NAME_LEN) < 0)
            {
                PROVIDER_LOG_ERR("Interface name [%s] is too long", devName);
                return VMK_FAILURE;
            }
        }

        if (vmk_MgmtUserInit(&sfvmk_mgmtSig, 0, &mgmtHandle))
        {
            PROVIDER_LOG_ERR("vmk_MgmtUserInit failed");
            return VMK_FAILURE;
        }

        if (vmk_MgmtUserCallbackInvoke(mgmtHandle, VMK_MGMT_NO_INSTANCE_ID,
                                                cmdCode, &mgmtParam, cmdParam))
        {
            PROVIDER_LOG_ERR("vmk_MgmtUserCallbackInvoke failed");
            drv_interaction_status = false;
        }

        if (vmk_MgmtUserDestroy(mgmtHandle))
        {
            PROVIDER_LOG_ERR("vmk_MgmtUserDestroy failed");
            return VMK_FAILURE;
        }

        if (drv_interaction_status)
            return mgmtParam.status;

        return VMK_FAILURE;
    }
 #else
    static int getVPD(const char *ifname, int port_number,
                      int device_type, bool staticVPD,
                      Array<VPDField> &parsedFields)
    {
        int       rc;
        int       fd;
        uint8_t  *nvram_data = NULL;
        uint32_t  nvram_data_len;
        uint8_t  *vpd = NULL;
        int       vpd_off;
        int       vpd_len;
        uint32_t  nvram_type;

        fd = open(DEV_SFC_CONTROL, O_RDWR);
        if (fd < 0)
        {
            PROVIDER_LOG_ERR("Failed to open '%s', errno %d('%s')",
                             DEV_SFC_CONTROL, errno, strerror(errno));
            return -1;
        }

        if (device_type == SFU_DEVICE_TYPE_SIENA)
        {
            if (staticVPD)
            {
                siena_mc_static_config_hdr_t partial_hdr;

                nvram_type = port_number == 0 ?
                                MC_CMD_NVRAM_TYPE_STATIC_CFG_PORT0 :
                                        MC_CMD_NVRAM_TYPE_STATIC_CFG_PORT1;

                if (siocEFXReadNVRAM(fd, false, (uint8_t *)&partial_hdr, 0,
                                     sizeof(partial_hdr),
                                     ifname, nvram_type) < 0)
                {
                    PROVIDER_LOG_ERR("Failed to get header from NVRAM");
                    close(fd);
                    return -1;
                }

                if (CI_BSWAP_LE32(partial_hdr.magic) !=
                                            SIENA_MC_STATIC_CONFIG_MAGIC)
                {
                    PROVIDER_LOG_ERR(
                                  "Unknown NVRAM header magic "
                                  "number %ld(0x%lx)",
                                  static_cast<long int>(partial_hdr.magic),
                                  static_cast<long int>(partial_hdr.magic));
                    close(fd);
                    return -1;
                }

                vpd_off = CI_BSWAP_LE32(partial_hdr.static_vpd_offset);
                vpd_len = CI_BSWAP_LE32(partial_hdr.static_vpd_length);
            }
            else
            {
                siena_mc_dynamic_config_hdr_t partial_hdr;

                nvram_type = port_number == 0 ?
                                MC_CMD_NVRAM_TYPE_DYNAMIC_CFG_PORT0 :
                                        MC_CMD_NVRAM_TYPE_DYNAMIC_CFG_PORT1;

                if (siocEFXReadNVRAM(fd, false, (uint8_t *)&partial_hdr, 0,
                                     sizeof(partial_hdr),
                                     ifname, nvram_type) < 0)
                {
                    PROVIDER_LOG_ERR("Failed to get header from NVRAM");
                    close(fd);
                    return -1;
                }

                if (CI_BSWAP_LE32(partial_hdr.magic) !=
                                            SIENA_MC_DYNAMIC_CONFIG_MAGIC)
                {
                    PROVIDER_LOG_ERR(
                                  "Unknown NVRAM header magic "
                                  "number %ld(0x%lx)",
                                  static_cast<long int>(partial_hdr.magic),
                                  static_cast<long int>(partial_hdr.magic));
                    close(fd);
                    return -1;
                }

                vpd_off = CI_BSWAP_LE32(partial_hdr.dynamic_vpd_offset);
                vpd_len = CI_BSWAP_LE32(partial_hdr.dynamic_vpd_length);
            }

            vpd = new uint8_t[vpd_len];
            if (siocEFXReadNVRAM(fd, false, vpd, vpd_off, vpd_len,
                                 ifname, nvram_type) < 0)
            {
                PROVIDER_LOG_ERR("Failed to read VPD from NVRAM");
                close(fd);
                delete[] vpd;
                return -1;
            }

            nvram_data = vpd;
        }
        else if (device_type == SFU_DEVICE_TYPE_HUNTINGTON ||
                 device_type == SFU_DEVICE_TYPE_MEDFORD)
        {
            nvram_partition_t partition;
            unit32_t tag;

            if (staticVPD)
                nvram_type = NVRAM_PARTITION_TYPE_STATIC_CONFIG;
            else
                nvram_type = NVRAM_PARTITION_TYPE_DYNAMIC_CONFIG;

            if (siocEFXReadNVRAMPartition(fd, false, ifname,
                                          nvram_type, nvram_data,
                                          nvram_data_len) < 0)
            {
                PROVIDER_LOG_ERR("Failed to read VPD from NVRAM");
                close(fd);
                return -1;
            }

            if ((rc = tlv_init_partition_from_buffer(
                                    &partition, nvram_data,
                                    nvram_data_len)) != TLV_OK)
            {
                PROVIDER_LOG_ERR("tlv_init_partition_from_buffer() "
                                 "returned %d", rc);
                close(fd);
                delete[] nvram_data;
                return -1;
            }

            if (device_type == SFU_DEVICE_TYPE_HUNTINGTON)
            {
                tag = (staticVPD ? TLV_TAG_PF_STATIC_VPD(0) :
                                   TLV_TAG_PF_DYNAMIC_VPD(0));
            }
            else
            {
                tag = (staticVPD ? TLV_TAG_GLOBAL_STATIC_VPD :
                                   TLV_TAG_GLOBAL_DYNAMIC_VPD);
            }

            if ((rc = tlv_find(&partition.tlv_cursor, tag)) != TLV_OK)
            {
                PROVIDER_LOG_ERR("tlv_find() returned %d", rc);
                close(fd);
                delete[] nvram_data;
                return -1;
            }

            if (device_type == SFU_DEVICE_TYPE_HUNTINGTON)
            {
                if (staticVPD)
                {
                    struct tlv_pf_static_vpd *p;

                    p = reinterpret_cast<struct tlv_pf_static_vpd *>
                                          (partition.tlv_cursor.current);

                    vpd = p->bytes;
                    vpd_len = le32_to_host(p->length);
                }
                else
                {
                    struct tlv_pf_dynamic_vpd *p;

                    p = reinterpret_cast<struct tlv_pf_dynamic_vpd *>
                                          (partition.tlv_cursor.current);

                    vpd = p->bytes;
                    vpd_len = le32_to_host(p->length);

                }
            }
            else
            {
                if (staticVPD)
                {
                    struct tlv_global_static_vpd *p;
                    p = reinterpret_cast<struct tlv_global_static_vpd *>
                                          (partition.tlv_cursor.current);

                    vpd = p->bytes;
                    vpd_len = le32_to_host(p->length);
                }
                else
                {
                    struct tlv_global_dynamic_vpd *p;

                    p = reinterpret_cast<struct tlv_global_dynamic_vpd *>
                                          (partition.tlv_cursor.current);

                    vpd = p->bytes;
                    vpd_len = le32_to_host(p->length);
                }
            }
        }
        else
        {
            PROVIDER_LOG_ERR("Device type %d seems to be neither of "
                             "Siena nor of EF10 NIC", device_type);
            close(fd);
            return -1;
        }

        if ((rc = parseVPD(vpd, vpd_len, parsedFields)) < 0)
        {
            PROVIDER_LOG_ERR("Failed to parse VPD");
            close(fd);
            delete[] nvram_data;
            return -1;
        }

        delete[] nvram_data;
        close(fd);

        return 0;
    }

#endif
    /// Firmware default file names table entry
    typedef struct {
        UpdatedFirmwareType type;     ///< Firmware type
        unsigned int subtype;         ///< Firmware subtype
        const char *fName;            ///< Default file name
    } FwFileTableEntry;

    ///
    /// Table of correspondence between firmware
    /// types and subtypes and names of files with
    /// firmware images.
    /// Use tools/image_table_gen/create_table.pl
    /// to generate this table from files in
    /// incoming_firmware repository.
    ///
    FwFileTableEntry fwFileTable[] = {
        // Bootrom
        { FIRMWARE_BOOTROM, 2, "SFL9021.dat" },
        { FIRMWARE_BOOTROM, 4, "SFC9120.dat" },
        { FIRMWARE_BOOTROM, 1, "SFC9020.dat" },
        { FIRMWARE_BOOTROM, 0, "FALCON.dat" },
        { FIRMWARE_BOOTROM, 5, "SFC9140.dat" },
        { FIRMWARE_BOOTROM, 6, "SFC9220.dat" },
        { FIRMWARE_BOOTROM, 7, "SFC9250.dat" },

        // MCFW
        { FIRMWARE_MCFW, 42, "BOB.dat" },
        { FIRMWARE_MCFW, 11, "BRIAN.dat" },
        { FIRMWARE_MCFW, 14, "BUXTON.dat" },
        { FIRMWARE_MCFW, 35, "BYBLOS.dat" },
        { FIRMWARE_MCFW, 47, "CROWN.dat" },
        { FIRMWARE_MCFW, 40, "CYCLOPS.dat" },
        { FIRMWARE_MCFW, 10, "DYLAN.dat" },
        { FIRMWARE_MCFW, 9, "ERMINTRUDE.dat" },
        { FIRMWARE_MCFW, 7, "FLORENCE.dat" },
        { FIRMWARE_MCFW, 38, "FLORIN.dat" },
        { FIRMWARE_MCFW, 24, "FORBES.dat" },
        { FIRMWARE_MCFW, 22, "GEHRELS.dat" },
        { FIRMWARE_MCFW, 26, "HERSCHEL.dat" },
        { FIRMWARE_MCFW, 43, "HOG.dat" },
        { FIRMWARE_MCFW, 33, "ICARUS.dat" },
        { FIRMWARE_MCFW, 28, "IKEYA.dat" },
        { FIRMWARE_MCFW, 34, "JERICHO.dat" },
        { FIRMWARE_MCFW, 20, "KAPTEYN.dat" },
        { FIRMWARE_MCFW, 29, "KOWALSKI.dat" },
        { FIRMWARE_MCFW, 25, "LONGMORE.dat" },
        { FIRMWARE_MCFW, 16, "MR_MCHENRY.dat" },
        { FIRMWARE_MCFW, 13, "MR_RUSTY.dat" },
        { FIRMWARE_MCFW, 41, "PENNY.dat" },
        { FIRMWARE_MCFW, 37, "SHILLING.dat" },
        { FIRMWARE_MCFW, 27, "SHOEMAKER.dat" },
        { FIRMWARE_MCFW, 46, "SIXPENCE.dat" },
        { FIRMWARE_MCFW, 45, "SOLIDUS.dat" },
        { FIRMWARE_MCFW, 44, "SOVEREIGN.dat" },
        { FIRMWARE_MCFW, 31, "SPARTA.dat" },
        { FIRMWARE_MCFW, 32, "THEBES.dat" },
        { FIRMWARE_MCFW, 39, "THREEPENCE.dat" },
        { FIRMWARE_MCFW, 18, "TUTTLE.dat" },
        { FIRMWARE_MCFW, 17, "UNCLE_HAMISH.dat" },
        { FIRMWARE_MCFW, 23, "WHIPPLE.dat" },
        { FIRMWARE_MCFW, 8, "ZEBEDEE.dat" },

        // UEFI
        { FIRMWARE_UEFIROM, 0, "MEDFORD_X64.dat" },
        { FIRMWARE_UEFIROM, 1, "MEDFORD_AARCH64.dat" },

        // SUCFW
        { FIRMWARE_SUCFW, 2, "ICARUS.dat" },
        { FIRMWARE_SUCFW, 3, "JERICHO.dat" },
        { FIRMWARE_SUCFW, 4, "BYBLOS.dat" },
        { FIRMWARE_SUCFW, 5, "SHILLING.dat" },
        { FIRMWARE_SUCFW, 6, "FLORIN.dat" },
        { FIRMWARE_SUCFW, 7, "THREEPENCE.dat" },
        { FIRMWARE_SUCFW, 8, "CYCLOPS.dat" },
        { FIRMWARE_SUCFW, 9, "PENNY.dat" },
        { FIRMWARE_SUCFW, 10, "BOB.dat" },
        { FIRMWARE_SUCFW, 11, "HOG.dat" },
        { FIRMWARE_SUCFW, 12, "SOVEREIGN.dat" },
        { FIRMWARE_SUCFW, 13, "SOLIDUS.dat" },
        { FIRMWARE_SUCFW, 14, "SIXPENCE.dat" },
        { FIRMWARE_SUCFW, 15, "CROWN.dat" },



        { FIRMWARE_UNKNOWN, 0, NULL},
    };

    ///
    /// Get subtype of a specific type of firmware installed on NIC.
    ///
    /// @param ifName             Interface name
    /// @param type               Firmware type
    /// @param device_type        Device type (Siena, Huntington, etc)
    /// @param subtype      [out] Firmware subtype
    ///
    /// @return 0 on success, -1 on error
    ///
#ifdef TARGET_CIM_SERVER_esxi_native
    static int getFwSubType(const char *ifName, UpdatedFirmwareType type,
                            int device_type, unsigned int &subtype)
    {
        UNUSED(device_type);

        sfvmk_nvramCmd_t nvram_read_req = {0};

        // NVRAM type field should be selected from the
        // fields that are declared in sfvmk_mgmt_interface.h
        if (type == FIRMWARE_BOOTROM)
            nvram_read_req.type = SFVMK_NVRAM_BOOTROM;
        else if (type == FIRMWARE_MCFW)
            nvram_read_req.type = SFVMK_NVRAM_MC;
        else if (type == FIRMWARE_UEFIROM)
            nvram_read_req.type = SFVMK_NVRAM_UEFIROM;
        else if (type == FIRMWARE_SUCFW)
            nvram_read_req.type = SFVMK_NVRAM_MUM;
        else
        {
            PROVIDER_LOG_ERR("%s(): Unknown Firmware Type", __FUNCTION__);
            return -1;
        }

        //SFVMK_NVRAM_OP_GET_VER returns both subtype and version number
        nvram_read_req.op = SFVMK_NVRAM_OP_GET_VER;

        if (DrvMgmtCall(ifName, SFVMK_CB_NVRAM_REQUEST, &nvram_read_req) != VMK_OK)
        {
            PROVIDER_LOG_ERR("%s(): NVRAM read failed", __FUNCTION__);
            return -1;
        }
        subtype = nvram_read_req.subtype;

        return 0;
    }
#else
    /// Get native firmware type ID used to obtain information about it via
    /// SIOCEFX ioctl().
    ///
    /// @param fwType       Updated firmware type ID
    /// @param device_type  Device type (Siena, Huntington, etc)
    ///
    /// @return Native firmware type
    ///
    int getNativeFirmwareType(UpdatedFirmwareType fwType, int device_type)
    {
        switch (fwType)
        {
            case FIRMWARE_BOOTROM:
                if (device_type == SFU_DEVICE_TYPE_SIENA)
                    return MC_CMD_NVRAM_TYPE_EXP_ROM;
                else
                    return NVRAM_PARTITION_TYPE_EXPANSION_ROM;

            case FIRMWARE_MCFW:
                if (device_type == SFU_DEVICE_TYPE_SIENA)
                    return MC_CMD_NVRAM_TYPE_MC_FW;
                else
                    return NVRAM_PARTITION_TYPE_MC_FIRMWARE;

            default:
                return -1;
        }
        return -1;
    }

    static int getFwSubType(const char *ifName, UpdatedFirmwareType type,
                            int device_type, unsigned int &subtype)
    {
        int       fd;

        struct efx_mcdi_request *mcdi_req;
        struct efx_ioctl         ioc;

        union {
            uint32_t dw;
            uint16_t w[2];
        } buf;

        int nativeType = getNativeFirmwareType(type, device_type);

        if (device_type != SFU_DEVICE_TYPE_SIENA &&
            device_type != SFU_DEVICE_TYPE_HUNTINGTON)
        {
            PROVIDER_LOG_ERR("%s(): incorrect device type %d",
                             __FUNCTION__, device_type);
            return -1;
        }

        fd = open(DEV_SFC_CONTROL, O_RDWR);
        if (fd < 0)
        {
            PROVIDER_LOG_ERR("Failed to open '%s', errno %d ('%s')",
                             DEV_SFC_CONTROL, errno,
                             strerror(errno));
            return -1;
        }

        memset(&ioc, 0, sizeof(ioc));
        strncpy(ioc.if_name, ifName, sizeof(ioc.if_name));
        ioc.cmd = EFX_MCDI_REQUEST;

        mcdi_req = &ioc.u.mcdi_request;

        if (device_type == SFU_DEVICE_TYPE_SIENA)
        {
            mcdi_req->cmd = MC_CMD_GET_BOARD_CFG;
            mcdi_req->len = 0;
        }
        else
        {
            mcdi_req->cmd = MC_CMD_NVRAM_METADATA;
            mcdi_req->payload[
              MC_CMD_NVRAM_METADATA_IN_TYPE_OFST / 4] = nativeType;
            mcdi_req->len = 4;
        }

        if (ioctl(fd, SIOCEFX, &ioc) < 0)
        {
            PROVIDER_LOG_ERR("ioctl(SIOCEFX) failed, errno %d ('%s')",
                             errno, strerror(errno));
            close(fd);
            return -1;
        }

        if (device_type == SFU_DEVICE_TYPE_SIENA)
        {
            buf.dw = mcdi_req->payload[
                MC_CMD_GET_BOARD_CFG_OUT_FW_SUBTYPE_LIST_OFST / 4 +
                (nativeType >> 1)];
            subtype =  buf.w[nativeType & 1] & 0x0000ffff;
        }
        else
        {
            buf.dw = mcdi_req->payload[
                MC_CMD_NVRAM_METADATA_OUT_SUBTYPE_OFST / 4];
            subtype = buf.dw;
        }

        close(fd);
        return 0;
    }
#endif

    ///
    /// Check firmware path, fix it if necessary and return it.
    ///
    /// @param path                 Path to firmware image
    /// @param defPath              What to add to the path if it
    ///                             does not end with firmware image
    ///                             file name (*.dat)
    /// @param was_completed  [out] Whether path completion was actually
    ///                             performed
    ///
    /// @return Fixed firmware image path
    ///
    static String fixFwImagePath(const char *path,
                                 const char *defPath,
                                 bool &was_completed)
    {
        char    *fn = strdup(path);
        String   strPath;
        bool     local_path = false;
        bool     dir_path = false;

        was_completed = false;

        if (fn == NULL)
            return String("");
        trim(fn);
        strPath = String(fn);

        if (strcasecmp_start(fn, FILE_PROTO) == 0)
        {
            local_path = true;
            int fd = open(fn + strlen(FILE_PROTO),
                          O_RDONLY | O_DIRECTORY);
            if (fd >= 0)
            {
                dir_path = true;
                close(fd);
            }
        }

        if (!(local_path && !dir_path) &&
            strstr(fn, ".dat") != fn + (strlen(fn) - 4))
        {
            strPath.append(String(defPath));
            was_completed = true;
        }
        free(fn);

        return strPath;
    }

    // Forward-declaration
    static SWElement::InstallRC vmwareInstallFirmware(
                                        const NIC *owner,
                                        const char *fileName,
                                        bool pathCompleted,
                                        UpdatedFirmwareType fwType,
                                        unsigned int subType,
                                        const VersionInfo &curVersion,
                                        bool force,
                                        const char *base64_hash);

    // Forward-declaration
    static String getDefaultFwPath(const NIC *owner,
                                   UpdatedFirmwareType fwType,
                                   const String &fName);

    // Forward-declaration
    static int getFwImageInfo(const NIC *owner,
                              UpdatedFirmwareType fwType,
                              unsigned int &subType, String &fName);

    // Forward-declaration
    static int vmwareGetLocalFwImageVersion(const char *path,
                                            const NIC *owner,
                                            UpdatedFirmwareType fwType,
                                            const VersionInfo &curVersion,
                                            bool &applicable,
                                            VersionInfo &imgVersion);

    ///
    /// Install firmware of a given type on a given NIC
    /// from a specified location.
    ///
    /// @param path         Where to find firmware image
    /// @param owner        NIC on which to install firmware
    /// @param fwType       Firmware type
    /// @param curVersion   Version of the currently installed firmware
    /// @param base64_hash  SHA-1 hash of firmware image,
    ///                     encoded in Base64
    ///
    /// @return Install_OK on sucess, error code on failure
    ///
    static SWElement::InstallRC vmwareInstallFirmwareType(
                                              const char *path,
                                              const NIC *owner,
                                              UpdatedFirmwareType fwType,
                                              const VersionInfo &curVersion,
                                              bool force,
                                              const char *base64_hash)
    {
        String          strPath;
        String          strDefPath;
        unsigned int    subType;
        String          fName;
        bool            was_completed;

        if (getFwImageInfo(owner, fwType, subType, fName) < 0)
            return SWElement::Install_Error;

        strDefPath = getDefaultFwPath(owner, fwType, fName);
        strPath = fixFwImagePath(path,
                                 strDefPath.c_str(),
                                 was_completed);

        return vmwareInstallFirmware(owner, strPath.c_str(),
                                     was_completed,
                                     fwType, subType,
                                     curVersion,
                                     force, base64_hash);
    }


    ///
    /// Log sfupdate output for debugging purposes.
    ///
    /// @param fd     Read end of pipe to which
    ///               sfupdate output was redirected
    /// @param debug  Whether to write sfupdate output
    ///               to debug or to error log
    ///
    static void sfupdateLogOutput(int fd, bool debug = true)
    {
        char  str[SFU_STR_MAX_LEN];
        FILE *f = fdopen(fd, "r");

        if (f == NULL)
            return;
        while (1)
        {
            if (fgets(str, SFU_STR_MAX_LEN, f) != str)
                break;
            if (debug)
                PROVIDER_LOG_DBG("%s", str);
            else
                PROVIDER_LOG_ERR("%s", str);
        }
        fclose(f);
    }

    ///
    /// Wrapper function working like popen("sfupdate ..."). It
    /// redirects stdout and stderr to pipe and calls main function
    /// of sfupdate with arguments obtained from a given command line.
    /// After sfupdate main function terminated, original file
    /// descriptors for stdout and stderr are restored.
    ///
    /// @param cmd_line Command line (should start with "sfupdate",
    ///                 for example "sfupdate --adapter=vmnic2")
    ///
    /// @return FD from which output of sfupdate can be read on
    ///         succens or negative value on failure
    ///
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

        static Mutex  lock(false);
        Auto_Mutex    auto_lock(lock);

        if (parseArgvString(cmd_line, &argc, &argv, &values) < 0 ||
            argv == NULL)
            return -1;

        saved_stdout_fd = dup(STDOUT_FILENO);
        if (saved_stdout_fd < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to duplicate stdout fd, "
                             "errno %d ('%s')",
                             __FUNCTION__, errno, strerror(errno));
            rc = -1;
            goto fail;
        }

        saved_stderr_fd = dup(STDERR_FILENO);
        if (saved_stderr_fd < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to duplicate stderr fd, "
                             "errno %d ('%s')",
                             __FUNCTION__, errno, strerror(errno));
            rc = -1;
            goto fail;
        }

        if (pipe(pipefds) < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to create a pipe, "
                             "errno %d ('%s')",
                             __FUNCTION__, errno, strerror(errno));
            rc = -1;
            goto fail;
        }

        fflush(stdout);
        fflush(stderr);

        if (dup2(pipefds[1], STDOUT_FILENO) < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to dulplicate write "
                             "end of pipe, errno %d ('%s')",
                             __FUNCTION__, errno, strerror(errno));
            rc = -1;
            goto fail;
        }
        else
            restore_stdout = 1;

        if (dup2(pipefds[1], STDERR_FILENO) < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to dulplicate write "
                             "end of pipe, errno %d ('%s')",
                             __FUNCTION__, errno, strerror(errno));
            rc = -1;
            goto fail;
        }
        else
            restore_stderr = 1;

        if ((rc = sfupdate_main(argc, argv)) != 0)
        {
            rc = -1;
            goto fail;
        }

    fail:
        fflush(stdout);
        fflush(stderr);

        delete[] argv;
        delete[] values;

        if (restore_stdout > 0)
            if (dup2(saved_stdout_fd, STDOUT_FILENO) < 0 && rc == 0)
            {
                PROVIDER_LOG_ERR("%s(): failed to restore stdout, "
                                 "errno %d ('%s')",
                                 __FUNCTION__, errno, strerror(errno));
                rc = -1;
            }
        if (restore_stderr > 0)
            if (dup2(saved_stderr_fd, STDERR_FILENO) < 0 && rc == 0)
            {
                PROVIDER_LOG_ERR("%s(): failed to restore stderr, "
                                 "errno %d ('%s')",
                                 __FUNCTION__, errno, strerror(errno));
                rc = -1;
            }
        if (saved_stdout_fd >= 0)
            close(saved_stdout_fd);
        if (saved_stderr_fd >= 0)
            close(saved_stderr_fd);

        close(pipefds[1]);
        if (rc == 0)
            return pipefds[0];
        else
        {
            PROVIDER_LOG_ERR("sfupdate failed:");
            sfupdateLogOutput(pipefds[0], false);
            close(pipefds[0]);
            return rc;
        }
    }

    ///
    /// Perform ethool command.
    ///
    /// @param dev_file          Path to device file
    /// @param dev_name          Device name
    /// @param cmd               Ethtool command
    /// @param edata             Location for get, data for set method
    ///
    /// @return zero on success, -1 on error
    ///
    static int vmwareEthtoolCmd(const char *dev_file, const char *dev_name,
                                unsigned cmd, void *edata)
    {
        int          fd;
        struct ifreq ifr;

        memset(&ifr, 0, sizeof(ifr));
        fd = open(dev_file, O_RDWR);
        if (fd < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to open '%s', "
                             "errno %d ('%s')",
                             __FUNCTION__, dev_file,
                             errno, strerror(errno));
            return -1;
        }

        strncpy(ifr.ifr_name, dev_name, sizeof(ifr.ifr_name));
        ifr.ifr_data = (char *)edata;
        ((struct ethtool_value *)edata)->cmd = cmd;
        if (ioctl(fd, SIOCETHTOOL, &ifr) < 0)
        {
            PROVIDER_LOG_ERR("%s(): ioctl(SIOCETHTOOL cmd=%u) failed, "
                             "errno %d ('%s')",
                             __FUNCTION__, cmd,
                             errno, strerror(errno));
            close(fd);
            return -1;
        }

        close(fd);
        return 0;
    }

    ///
    /// Get link status.
    ///
    /// @param devFile    Device file name
    /// @param devName    Device name
    ///
    /// @return link status (true if it is up, false otherwise)
    ///
#ifdef TARGET_CIM_SERVER_esxi_native
    static bool getLinkStatus(const String &devFile,
                              const String &devName)
    {
        UNUSED(devFile);

        bool    linkStatus = false;
        char    deviceName[SFVMK_DEV_NAME_LEN];

        if (strncpy_check(deviceName, devName.c_str(), SFVMK_DEV_NAME_LEN) < 0)
        {
            PROVIDER_LOG_ERR("%s(): Interface name is too long", __FUNCTION__);
            return true;
        }
        if (DrvMgmtCall(deviceName, SFVMK_CB_LINK_STATUS_GET, &linkStatus) != VMK_OK)
        {
            PROVIDER_LOG_ERR("%s(): DrvMgmtCall failed", __FUNCTION__);
            return true;
        }
        else
            return linkStatus;
    }
#else
    static bool getLinkStatus(const String &devFile,
                              const String &devName)
    {
        struct ethtool_value edata;

        memset(&edata, 0, sizeof(edata));
        if (vmwareEthtoolCmd(devFile.c_str(), devName.c_str(),
                             ETHTOOL_GLINK, &edata) < 0)
            return true;

        return edata.data == 1 ? true : false;
    }
#endif

    class VMwarePort : public Port {
        NIC *owner;
        unsigned pci_fn;

    public:
        String dev_file;
        String dev_name;

        VMwarePort(NIC *up, unsigned i, PortDescr &descr) :
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

        static int doGetPrivileges(
                    const Property<String> &PCIAddr,
                    Property<Array_String> &privilegeNames,
                    Property<Array_uint32> &privileges);
        static int doModifyPrivileges(
                    const Property<String> &PCIAddr,
                    const Property<String> &addedMask,
                    const Property<String> &removedMask);

        virtual int getIntrModeration(const Array_String &paramNames,
                                      Array_uint32 &paramValues) const;
        virtual int setIntrModeration(const Array_String &paramNames,
                                      const Array_uint32 &paramValues);

        // Dummy operator to make possible using of cimple::Array
        bool operator== (const VMwarePort &rhs)
        {
            UNUSED(rhs);
            return false;
        }
    };

    bool VMwarePort::linkStatus() const
    {
        return getLinkStatus(dev_file, dev_name);
    }

#ifndef TARGET_CIM_SERVER_esxi_native
    Port::Speed VMwarePort::linkSpeed() const
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

    void VMwarePort::linkSpeed(Port::Speed sp)
    {
        struct ethtool_cmd edata;

        memset(&edata, 0, sizeof(edata));
        if (vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                             ETHTOOL_GSET, &edata) < 0)
            THROW_PROVIDER_EXCEPTION;

        switch (sp)
        {
            case Port::Speed10M: edata.speed = SPEED_10; break;
            case Port::Speed100M: edata.speed = SPEED_100; break;
            case Port::Speed1G: edata.speed = SPEED_1000; break;
            case Port::Speed10G: edata.speed = SPEED_10000; break;
            default:
                THROW_PROVIDER_EXCEPTION_FMT("Nonstandard speed specified");
        }

        if (vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                             ETHTOOL_SSET, &edata) < 0)
            THROW_PROVIDER_EXCEPTION;
    }
#else
    Port::Speed VMwarePort::linkSpeed() const
    {
        sfvmk_linkSpeed_t linkSpeed;

        linkSpeed.type = SFVMK_MGMT_DEV_OPS_GET;

        if (DrvMgmtCall(dev_name.c_str(), SFVMK_CB_LINK_SPEED_REQUEST, &linkSpeed) != VMK_OK)
            return Speed(Port::SpeedUnknown);

        switch (linkSpeed.speed)
        {
            case SFVMK_LINK_SPEED_10_MBPS: return Speed(Port::Speed10M);
            case SFVMK_LINK_SPEED_100_MBPS: return Speed(Port::Speed100M);
            case SFVMK_LINK_SPEED_1000_MBPS: return Speed(Port::Speed1G);
            case SFVMK_LINK_SPEED_10000_MBPS: return Speed(Port::Speed10G);
            case SFVMK_LINK_SPEED_25000_MBPS: return Speed(Port::Speed25G);
            case SFVMK_LINK_SPEED_40000_MBPS: return Speed(Port::Speed40G);
            case SFVMK_LINK_SPEED_50000_MBPS: return Speed(Port::Speed50G);
            case SFVMK_LINK_SPEED_100000_MBPS: return Speed(Port::Speed100G);
            default: return Speed(Port::SpeedUnknown);
        }
    }

    void VMwarePort::linkSpeed(Port::Speed sp)
    {
        sfvmk_linkSpeed_t linkSpeed;

        linkSpeed.type = SFVMK_MGMT_DEV_OPS_SET;
        linkSpeed.autoNeg = VMK_FALSE;

        switch (sp)
        {
            case Port::Speed10M: linkSpeed.speed = SFVMK_LINK_SPEED_10_MBPS; break;
            case Port::Speed100M: linkSpeed.speed = SFVMK_LINK_SPEED_100_MBPS; break;
            case Port::Speed1G: linkSpeed.speed = SFVMK_LINK_SPEED_1000_MBPS; break;
            case Port::Speed10G: linkSpeed.speed = SFVMK_LINK_SPEED_10000_MBPS; break;
            case Port::Speed25G: linkSpeed.speed = SFVMK_LINK_SPEED_25000_MBPS; break;
            case Port::Speed40G: linkSpeed.speed = SFVMK_LINK_SPEED_40000_MBPS; break;
            case Port::Speed50G: linkSpeed.speed = SFVMK_LINK_SPEED_50000_MBPS; break;
            case Port::Speed100G: linkSpeed.speed = SFVMK_LINK_SPEED_100000_MBPS; break;
            default:
                THROW_PROVIDER_EXCEPTION_FMT("Nonstandard speed specified");
        }

        if (DrvMgmtCall(dev_name.c_str(), SFVMK_CB_LINK_SPEED_REQUEST, &linkSpeed) != VMK_OK)
            THROW_PROVIDER_EXCEPTION_FMT("Link Speed Configuration Failed");
    }
#endif
#ifndef TARGET_CIM_SERVER_esxi_native
    bool VMwarePort::fullDuplex() const
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

    void VMwarePort::fullDuplex(bool fd)
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

#else
    bool VMwarePort::fullDuplex() const
    {
        /* For Medford family boards, the full duplex is always enabled*/
        return true;
    }

    void VMwarePort::fullDuplex(bool fd)
    {
        UNUSED(fd);
    }
#endif
#ifndef TARGET_CIM_SERVER_esxi_native
    bool VMwarePort::autoneg() const
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

    void VMwarePort::autoneg(bool an)
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
#else
    bool VMwarePort::autoneg() const
    {
        sfvmk_linkSpeed_t speedNeg;

        speedNeg.type = SFVMK_MGMT_DEV_OPS_GET;

        if (DrvMgmtCall(dev_name.c_str(), SFVMK_CB_LINK_SPEED_REQUEST,
                        &speedNeg) == VMK_OK)
            return (speedNeg.autoNeg == VMK_TRUE);

        return true;
    }

    void VMwarePort::autoneg(bool an)
    {
        sfvmk_linkSpeed_t speedNeg;

        speedNeg.type = SFVMK_MGMT_DEV_OPS_GET;

        if ((DrvMgmtCall(dev_name.c_str(), SFVMK_CB_LINK_SPEED_REQUEST,
                         &speedNeg) != VMK_OK) || (speedNeg.autoNeg == an))
            return;

        speedNeg.autoNeg = an;
        speedNeg.type = SFVMK_MGMT_DEV_OPS_SET;

        DrvMgmtCall(dev_name.c_str(), SFVMK_CB_LINK_SPEED_REQUEST, &speedNeg);
    }
#endif

    void VMwarePort::renegotiate()
    {
        struct ethtool_cmd edata;

        memset(&edata, 0, sizeof(edata));
        vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                         ETHTOOL_NWAY_RST, &edata);
    }

    MACAddress VMwarePort::permanentMAC() const
    {
        // ETHTOOL_GPERMADDR is not supported on ESXi, and
        // we cannot use popen("esxcli...") since fork() is
        // forbidden, so we need to get them from standard
        // objects in root/cimv2 namespace of VMware CIM server.

        Ref<CIM_EthernetPort> cimEthPort;

        cimEthPort = getCIMEthPort(dev_name.c_str());

        if (!cimEthPort)
            return MACAddress(0, 0, 0, 0, 0, 0);

        if (cimEthPort->PermanentAddress.null)
            return MACAddress(0, 0, 0, 0, 0, 0);
        else
        {
#define MAC_STR_SIZE (6 * 3)
            unsigned int  a0;
            unsigned int  a1;
            unsigned int  a2;
            unsigned int  a3;
            unsigned int  a4;
            unsigned int  a5;
            const char   *mac_val;
            char          mac_str[MAC_STR_SIZE];

            mac_val = cimEthPort->PermanentAddress.value.c_str();
            if (strstr(mac_val, ":") == NULL)
            {
                // MAC address is represented as hexadecimal number
                // without ':' separators - fix this.
                if (strlen(mac_val) < 12)
                    return MACAddress(0, 0, 0, 0, 0, 0);
                snprintf(mac_str, MAC_STR_SIZE,
                         "%c%c:%c%c:%c%c:%c%c:%c%c:%c%c",
                         mac_val[0], mac_val[1], mac_val[2], mac_val[3],
                         mac_val[4], mac_val[5], mac_val[6], mac_val[7],
                         mac_val[8], mac_val[9], mac_val[10], mac_val[11]);
                mac_val = mac_str;
            }

            if (sscanf(mac_val,
                       "%x:%x:%x:%x:%x:%x",
                       &a0, &a1, &a2, &a3, &a4, &a5) != 6)
                return MACAddress(0, 0, 0, 0, 0, 0);

            return MACAddress(a0, a1, a2, a3, a4, a5);
#undef MAC_STR_SIZE
        }
    }

    /// Get string name of a given privilege flag.
    ///
    /// @param flag       Privilege flag
    ///
    /// @return String name ("<UNKNOWN>" if flag is not known)
    static const char *privilege_flag2name(unsigned int flag)
    {
#define PRIVILEGE_FLAG2NAME(_p) \
    case MC_CMD_PRIVILEGE_MASK_IN_GRP_ ## _p: \
        return #_p;

        switch (flag)
        {
            PRIVILEGE_FLAG2NAME(ADMIN);
            PRIVILEGE_FLAG2NAME(LINK);
            PRIVILEGE_FLAG2NAME(ONLOAD);
            PRIVILEGE_FLAG2NAME(PTP);
            PRIVILEGE_FLAG2NAME(INSECURE_FILTERS);
            PRIVILEGE_FLAG2NAME(MAC_SPOOFING);
            PRIVILEGE_FLAG2NAME(UNICAST);
            PRIVILEGE_FLAG2NAME(MULTICAST);
            PRIVILEGE_FLAG2NAME(BROADCAST);
            PRIVILEGE_FLAG2NAME(ALL_MULTICAST);
            PRIVILEGE_FLAG2NAME(PROMISCUOUS);
            PRIVILEGE_FLAG2NAME(MAC_SPOOFING_TX);
            PRIVILEGE_FLAG2NAME(CHANGE_MAC);
            PRIVILEGE_FLAG2NAME(UNRESTRICTED_VLAN);
            PRIVILEGE_FLAG2NAME(INSECURE);
        }

        return "<UNKNOWN>";
    }

    ///
    /// Convert privilege name to flag.
    ///
    /// @param name     Name to convert
    ///
    /// @return Flag value (0 if name is not known
    static unsigned int privilege_name2flag(const char *name)
    {
#define PRIVILEGE_NAME2FLAG(priv_) \
    if (strcmp(name, #priv_) == 0) \
        return MC_CMD_PRIVILEGE_MASK_IN_GRP_ ## priv_;

        PRIVILEGE_NAME2FLAG(ADMIN);
        PRIVILEGE_NAME2FLAG(LINK);
        PRIVILEGE_NAME2FLAG(ONLOAD);
        PRIVILEGE_NAME2FLAG(PTP);
        PRIVILEGE_NAME2FLAG(INSECURE_FILTERS);
        PRIVILEGE_NAME2FLAG(MAC_SPOOFING);
        PRIVILEGE_NAME2FLAG(UNICAST);
        PRIVILEGE_NAME2FLAG(MULTICAST);
        PRIVILEGE_NAME2FLAG(BROADCAST);
        PRIVILEGE_NAME2FLAG(ALL_MULTICAST);
        PRIVILEGE_NAME2FLAG(PROMISCUOUS);
        PRIVILEGE_NAME2FLAG(MAC_SPOOFING_TX);
        PRIVILEGE_NAME2FLAG(CHANGE_MAC);
        PRIVILEGE_NAME2FLAG(UNRESTRICTED_VLAN);
        PRIVILEGE_NAME2FLAG(INSECURE);

        PROVIDER_LOG_ERR("Unknown privilege '%s'", name);
        return 0;
    }

    ///
    /// Convert string representation of privileges mask to
    /// numeric representation.
    ///
    /// @param str          String to convert
    /// @param mask   [out] Where to save mask
    ///
    /// @note See comments for modifyPrivileges() method in
    ///       sf_platform.h for mask format description.
    ///
    /// @return 0 on success, -1 on failure
    static int privileges_str2mask(const char *str, uint32_t &mask)
    {
#define MAX_PRIV_LEN 512
        unsigned int i = 0;
        unsigned int j = 0;
        char         priv_name[MAX_PRIV_LEN];
        bool         is_num = false;
        unsigned int flag = 0;

        mask = 0;

        for (i = 0; ; i++)
        {
            if (str[i] == ':')
            {
                is_num = true;
            }
            else if (str[i] == '|' || str[i] == '\0')
            {
                priv_name[j] = '\0';
                j = 0;

                if (is_num)
                {
                    char      *endp = NULL;
                    long int   value = 0;

                    value = strtol(priv_name, &endp, 10);
                    if (value < 0 || value > 0xffff ||
                        *endp != '\0')
                    {
                        PROVIDER_LOG_ERR("Failed to convert '%s' to number",
                                         priv_name);
                        return -1;
                    }

                    flag = 1 << value;
                }
                else
                {
                    flag = privilege_name2flag(priv_name);
                    if (flag == 0)
                        return -1;
                }

                mask = mask | flag;
                is_num = false;
            }
            else
            {
                if (j >= MAX_PRIV_LEN - 2)
                {
                    PROVIDER_LOG_ERR("Too long privilege name in '%s'",
                                     str);
                    return -1;
                }
                priv_name[j] = str[i];
                j++;
            }

            if (str[i] == '\0')
                break;
        }

        return 0;
    }

#ifndef TARGET_CIM_SERVER_esxi_native

    /// Set interface name for SIOCEFX call.
    ///
    /// @param ioc      efx_ioctl structure
    /// @param ifName   Interface name
    ///
    /// @return 0 on success, -1 on failure
    static int setIoctlIf(struct efx_ioctl &ioc,
                          const char *ifName)
    {
        if (strlen(ifName) > sizeof(ioc.if_name) - 1)
        {
            PROVIDER_LOG_ERR("Too long interface name %s",
                             ifName);
            return -1;
        }

        strncpy(ioc.if_name, ifName, sizeof(ioc.if_name));

        return 0;
    }

    /// Prepare parameters for SIOCEFX MCDI call.
    ///
    /// @param ifName         Interface name
    /// @param fd       [out] File descriptor for ioctl()
    /// @param ioc      [out] efx_ioctl structure where to
    ///                       fill if_name and cmd fields
    ///
    /// @return 0 on success, -1 on failure
    static int prepareMCDICall(const char *ifName,
                               int &fd,
                               struct efx_ioctl &ioc)
    {
        memset(&ioc, 0, sizeof(ioc));
        if (setIoctlIf(ioc, ifName) < 0)
            return -1;

        fd = open(DEV_SFC_CONTROL, O_RDWR);
        if (fd < 0)
        {
            PROVIDER_LOG_ERR("Failed to open '%s', errno %d ('%s')",
                             DEV_SFC_CONTROL, errno,
                             strerror(errno));
            return -1;
        }

        return 0;
    }

    /// Execute MCDIv2 command, check results.
    ///
    /// @param fd       File descriptor on which to call ioctl()
    /// @param ioc      efx_ioctl structure
    ///
    /// @return 0 on success, -1 on failure
    static int doMCDI2Call(int fd,
                          struct efx_ioctl &ioc)
    {
        struct efx_mcdi_request2 *mcdi_req = NULL;

        mcdi_req = &ioc.u.mcdi_request2;
        ioc.cmd = EFX_MCDI_REQUEST2;

        if (ioctl(fd, SIOCEFX, &ioc) < 0)
        {
            PROVIDER_LOG_ERR("ioctl(SIOCEFX:mcdi_cmd=0x%x) "
                             "failed, errno %d ('%s')",
                             (unsigned int)mcdi_req->cmd, errno,
                             strerror(errno));
            return -1;
        }

        if (mcdi_req->flags & EFX_MCDI_REQUEST_ERROR)
        {
            PROVIDER_LOG_ERR("MCDIv2 request 0x%x failed, errno %u (%s)",
                             (unsigned int)mcdi_req->cmd,
                             (unsigned int)mcdi_req->host_errno,
                             strerror(mcdi_req->host_errno));
            return -1;
        }

        return 0;
    }

    ///
    /// Do MCDI proxied call on specified PF/VF.
    ///
    /// @note This function expects MCDIv2 request in @p ioc,
    ///       it will not work correctly with MCDIv1.
    ///
    /// @param fd       File descriptor on which to call ioctl().
    /// @param ioc      Pointer to filled efx_ioctl structure.
    /// @param pf       PF for which to perform MCDI call.
    /// @param vf       VF for which to perform MCDI call
    ///                 (may be MC_CMD_PROXY_CMD_IN_VF_NULL).
    ///
    /// @return 0 on success, -1 on failure
    static int doMCDIProxyCall(int fd,
                               struct efx_ioctl &ioc,
                               uint32_t pf,
                               uint32_t vf)
    {
        struct efx_mcdi_request2  *mcdi_req;
        struct efx_mcdi_request2   proxy_req;
        struct efx_mcdi_request2   saved_req;

        size_t    req_size;
        uint32_t  function;

        uint32_t  hdr[2] = { 0, };
        uint32_t  error;

        mcdi_req = &ioc.u.mcdi_request2;
        req_size = mcdi_req->inlen + sizeof(hdr);

        if (req_size + MC_CMD_PROXY_CMD_IN_LEN >
                                    sizeof(proxy_req.payload))
        {
            PROVIDER_LOG_ERR("%s(): MCDI request is too big to "
                             "be encapsulated", __FUNCTION__);
            return -1;
        }

        memset(&proxy_req, 0, sizeof(proxy_req));
        proxy_req.cmd = MC_CMD_PROXY_CMD;
        proxy_req.inlen = MC_CMD_PROXY_CMD_IN_LEN + req_size;
        proxy_req.outlen = sizeof(proxy_req.payload);

        try {
            function = 0;
            SET_MCDI_FIELD_PART(PROXY_CMD_IN_TARGET, PF,
                                function, pf);
            SET_MCDI_FIELD_PART(PROXY_CMD_IN_TARGET, VF,
                                function, vf);
            SET_MCDI_FIELD(proxy_req.payload, PROXY_CMD_IN_TARGET,
                           function);
        }
        catch (ProviderException &)
        {
            return -1;
        }

        memset(hdr, 0, sizeof(hdr));
        hdr[0] = (1 << MCDI_HEADER_RESYNC_LBN |
                  MC_CMD_V2_EXTN << MCDI_HEADER_CODE_LBN);
        hdr[1] = (mcdi_req->cmd << MC_CMD_V2_EXTN_IN_EXTENDED_CMD_LBN |
                  mcdi_req->inlen << MC_CMD_V2_EXTN_IN_ACTUAL_LEN_LBN);

        memcpy((uint8_t *)proxy_req.payload + MC_CMD_PROXY_CMD_IN_LEN,
               hdr, sizeof(hdr));
        memcpy((uint8_t *)proxy_req.payload + MC_CMD_PROXY_CMD_IN_LEN +
               sizeof(hdr),
               mcdi_req->payload, mcdi_req->inlen);

        memcpy(&saved_req, mcdi_req, sizeof(saved_req));
        memcpy(mcdi_req, &proxy_req, sizeof(proxy_req));

        if (doMCDI2Call(fd, ioc) < 0)
        {
            memcpy(mcdi_req, &saved_req, sizeof(saved_req));
            return -1;
        }

         memcpy(hdr, mcdi_req->payload, sizeof(hdr));
        memcpy(&proxy_req.payload, mcdi_req->payload,
               sizeof(mcdi_req->payload));
        memcpy(mcdi_req, &saved_req, sizeof(saved_req));
        memcpy(mcdi_req->payload,
               (uint8_t *)proxy_req.payload + sizeof(hdr),
               sizeof(proxy_req.payload) - sizeof(hdr));

        error = ((hdr[0] >> MCDI_HEADER_ERROR_LBN) &
                 ((1 << MCDI_HEADER_ERROR_WIDTH) - 1));
        if (error != 0)
        {
            PROVIDER_LOG_ERR("%s(): proxied MCDI command 0x%x failed",
                             __FUNCTION__, mcdi_req->cmd);
            return -1;
        }

        return 0;
    }

    /// Execute MCDI command, check results.
    ///
    /// @param fd       File descriptor on which to call ioctl()
    /// @param ioc      efx_ioctl structure
    ///
    /// @return 0 on success, -1 on failure
    static int doMCDICall(int fd,
                          struct efx_ioctl &ioc)
    {
        struct efx_mcdi_request *mcdi_req = NULL;

        mcdi_req = &ioc.u.mcdi_request;
        ioc.cmd = EFX_MCDI_REQUEST;

        if (ioctl(fd, SIOCEFX, &ioc) < 0)
        {
            PROVIDER_LOG_ERR("ioctl(SIOCEFX:mcdi_cmd=0x%x) "
                             "failed, errno %d ('%s')",
                             (unsigned int)mcdi_req->cmd, errno,
                             strerror(errno));
            return -1;
        }

        if (mcdi_req->rc != 0)
        {
            PROVIDER_LOG_ERR("MCDI request 0x%x returned %u",
                             (unsigned int)mcdi_req->cmd,
                             (unsigned int)mcdi_req->rc);
            return -1;
        }

        return 0;
    }

    ///
    /// Get Current number of Physical Functions
    ///
    /// @param fd       File descriptor on which to call ioctl().
    /// @param ioc      Filled efx_ioctl structure.
    /// @param num      Where to save number of PFs.
    ///
    /// @return 0 on success, -1 on failure.
    static int getPFsNum(int fd,
                         struct efx_ioctl &ioc,
                         unsigned int &num)
    {
        struct efx_mcdi_request2 *mcdi_req = NULL;

        mcdi_req = &ioc.u.mcdi_request2;
        memset(mcdi_req, 0, sizeof(*mcdi_req));

        mcdi_req->cmd = MC_CMD_GET_PF_COUNT;
        mcdi_req->inlen = MC_CMD_GET_PF_COUNT_IN_LEN;
        mcdi_req->outlen = MC_CMD_GET_PF_COUNT_OUT_LEN;

        if (doMCDI2Call(fd, ioc) < 0)
            return -1;

        try {
            num = GET_MCDI_FIELD(mcdi_req->payload,
                                 GET_PF_COUNT_OUT_PF_COUNT);
        }
        catch (const ProviderException &)
        {
            return -1;
        }

        return 0;
    }

    ///
    /// Data obtained with MC_CMD_GET_SRIOV_CFG.
    ///
    typedef struct sriov_info {
        uint32_t vf_current;    ///< Number of VFs configured.
        uint32_t vf_offset;     ///< RID offset of the first VF.
        uint32_t vf_stride;     ///< RID stride between consecutive VFs.
    } sriov_info;

    ///
    /// Get SRIOV configuration of a given PF.
    ///
    /// @param fd         File descriptor on which to call ioctl().
    /// @param ioc        Filled efx_ioctl structure.
    /// @param use_proxy  If true, use MC_CMD_PROXY_CMD to call
    ///                   MC_CMD_GET_SRIOV_CFG on a specified PF.
    ///                   If false, MC_CMD_GET_SRIOV_CFG will be
    ///                   called for an interface specified in @p ioc.
    /// @param pf         For which PF to obtain configuration
    ///                   (should be set if use_proxy is true).
    /// @param info       Where to save SRIOV configuration.
    ///
    /// @return 0 on success, -1 on failure.
    static int getPFSRIOVInfo(int fd,
                              struct efx_ioctl &ioc,
                              bool use_proxy,
                              uint32_t pf,
                              sriov_info &info)
    {
        struct efx_mcdi_request2 *mcdi_req = NULL;

        memset(&info, 0, sizeof(info));

        mcdi_req = &ioc.u.mcdi_request2;
        memset(mcdi_req, 0, sizeof(*mcdi_req));

        mcdi_req->cmd = MC_CMD_GET_SRIOV_CFG;
        mcdi_req->inlen = MC_CMD_GET_SRIOV_CFG_IN_LEN;
        mcdi_req->outlen = MC_CMD_GET_SRIOV_CFG_OUT_LEN;

        if (use_proxy)
        {
            if (doMCDIProxyCall(fd, ioc, pf,
                                MC_CMD_PROXY_CMD_IN_VF_NULL) < 0)
                return -1;
        }
        else
        {
            if (doMCDI2Call(fd, ioc) < 0)
                return -1;
        }

        try {
            info.vf_current = GET_MCDI_FIELD(mcdi_req->payload,
                                             GET_SRIOV_CFG_OUT_VF_CURRENT);
            info.vf_offset = GET_MCDI_FIELD(mcdi_req->payload,
                                            GET_SRIOV_CFG_OUT_VF_OFFSET);
            info.vf_stride = GET_MCDI_FIELD(mcdi_req->payload,
                                            GET_SRIOV_CFG_OUT_VF_STRIDE);
        }
        catch (const ProviderException &)
        {
            return -1;
        }

        return 0;
    }

    ///
    /// Get PF/VF of an interface.
    ///
    /// @param fd         File descriptor on which to call ioctl().
    /// @param ioc        Filled efx_ioctl structure.
    /// @param pf         Where to save PF.
    /// @param vf         Where to save VF.
    ///
    /// @return 0 on success, -1 on failure.
    static int getPFVF(int fd,
                       struct efx_ioctl &ioc,
                       uint32_t &pf,
                       uint32_t &vf)
    {
        struct efx_mcdi_request2 *mcdi_req = NULL;

        mcdi_req = &ioc.u.mcdi_request2;
        memset(mcdi_req, 0, sizeof(*mcdi_req));

        mcdi_req->cmd = MC_CMD_GET_FUNCTION_INFO;
        mcdi_req->inlen = MC_CMD_GET_FUNCTION_INFO_IN_LEN;
        mcdi_req->outlen = MC_CMD_GET_FUNCTION_INFO_OUT_LEN;

        if (doMCDI2Call(fd, ioc) < 0)
            return -1;

        try {
            pf = GET_MCDI_FIELD(mcdi_req->payload,
                                GET_FUNCTION_INFO_OUT_PF);
            vf = GET_MCDI_FIELD(mcdi_req->payload,
                                GET_FUNCTION_INFO_OUT_VF);
        }
        catch (const ProviderException &)
        {
            return -1;
        }

        return 0;
    }

    ///
    /// Check whether a given number is PCIAddress::unknown.
    /// If it is, return 0, otherwise return the given number.
    ///
    /// @param n      Number to process.
    ///
    /// @return Result.
    static uint32_t PCIUnknownToZero(uint32_t n)
    {
        if (n == PCIAddress::unknown)
            return 0;

        return n;
    }

    ///
    /// Convert PCI address to RID.
    ///
    /// @param addr     PCI address.
    /// @param rid      Where to save RID.
    ///
    /// @return 0 on success, -1 on failure,
    static int PCIAddrToRID(const PCIAddress addr,
                            uint32_t &rid)
    {
        unsigned int dom = PCIUnknownToZero(addr.domain());
        unsigned int bus = PCIUnknownToZero(addr.bus());
        unsigned int dev = PCIUnknownToZero(addr.device());
        unsigned int fn = PCIUnknownToZero(addr.fn());

        if ((fn > 7 && dev != 0) || fn > 255 || dev > 0x1f || bus > 255 ||
            dom > 0xffff)
        {
            PROVIDER_LOG_ERR("Cannot convert %x:%x:%x.%x to RID",
                             dom, bus, dev, fn);
            return -1;
        }

        rid = (dom << 16) + (bus << 8) + (dev << 3) + fn;

        return 0;
    }

    ///
    /// Get PF/VF corresponding to a given PCI address.
    ///
    ///
    /// @param fd             File descriptor on which to call ioctl().
    /// @param ioc            Filled efx_ioctl structure.
    /// @param dev_name       Device name (will be set in @p ioc if
    ///                        not NULL).
    /// @param devPCIAddr     PCI address of a device on which this function
    ///                       is called.
    /// @param searchPCIAddr  PCI address for which PF/VF should be found.
    /// @param found          Will be set to TRUE if PF/VF corresponding to
    ///                       @p searchPCIAddr were found on the device.
    /// @param pf_out         Where to save PF.
    /// @param vf_out         Where to save VF (may be
    ///                       MC_CMD_PRIVILEGE_MASK_IN_VF_NULL).
    ///
    /// @return 0 on success, -1 on failure.
    static int PCIToPFVF(int fd,
                         struct efx_ioctl &ioc,
                         const char *dev_name,
                         const PCIAddress &devPCIAddr,
                         const PCIAddress &searchPCIAddr,
                         bool &found,
                         uint32_t &pf_out,
                         uint32_t &vf_out)
    {
        int                 rc;
        uint32_t            pf;
        uint32_t            vf;
        uint32_t            rid;
        uint32_t            cur_rid;
        uint32_t            pfs_num;
        uint32_t            pf0_rid;
        uint32_t            vf0_rid;
        uint32_t            i;

        sriov_info info;

        found = false;
        memset(&info, 0, sizeof(info));

        if (dev_name != NULL)
        {
            if (setIoctlIf(ioc, dev_name) < 0)
                return -1;
        }

        if (getPFVF(fd, ioc, pf, vf) < 0)
            return -1;

        if (getPFsNum(fd, ioc, pfs_num) < 0)
            return -1;

        if (PCIAddrToRID(devPCIAddr, cur_rid) < 0)
            return -1;
        if (PCIAddrToRID(searchPCIAddr, rid) < 0)
            return -1;

        if (vf != MC_CMD_PRIVILEGE_MASK_IN_VF_NULL)
        {
            if (getPFSRIOVInfo(fd, ioc, true, pf, info) < 0)
                return -1;

            pf0_rid = cur_rid - vf * info.vf_stride - info.vf_offset - pf;
        }
        else
        {
            pf0_rid = cur_rid - pf;
        }

        if (rid >= pf0_rid && rid < pf0_rid + pfs_num)
        {
            found = true;
            pf_out = rid - pf0_rid;
            vf_out = MC_CMD_PRIVILEGE_MASK_IN_VF_NULL;
            return 0;
        }

        for (i = 0; i < pfs_num; i++)
        {
            if (getPFSRIOVInfo(fd, ioc, true, i, info) < 0)
                return -1;
        vf0_rid = pf0_rid + i + info.vf_offset;

            if (rid >= vf0_rid &&
                rid < vf0_rid + info.vf_current * info.vf_stride &&
                (rid - vf0_rid) % info.vf_stride == 0)
            {
                pf_out = i;
                vf_out = (rid - vf0_rid) / info.vf_stride;
                found = true;
                break;
            }
        }

        return 0;
    }

    /// Get privileges of a given physical or virtual function.
    ///
    /// @param fd           File descriptor on which to call ioctl()
    /// @param ioc          efx_ioctl structure
    /// @param pf           Physical Function number
    /// @param vf           Virtual Function number
    /// @param privs  [out] Where to save obtained privileges mask
    ///
    /// @return 0 on success, -1 on error
    static int getFunctionPrivileges(int fd,
                                     struct efx_ioctl &ioc,
                                     uint32_t pf,
                                     uint32_t vf,
                                     unsigned int &privs)
    {
        struct efx_mcdi_request *mcdi_req = NULL;

        uint32_t function;

        mcdi_req = &ioc.u.mcdi_request;
        memset(mcdi_req, 0, sizeof(*mcdi_req));

        mcdi_req->cmd = MC_CMD_PRIVILEGE_MASK;
        mcdi_req->len = MC_CMD_PRIVILEGE_MASK_IN_LEN;

        try {
            function = 0;
            SET_MCDI_FIELD_PART(PRIVILEGE_MASK_IN_FUNCTION, PF,
                                function, pf);
            SET_MCDI_FIELD_PART(PRIVILEGE_MASK_IN_FUNCTION, VF,
                                function, vf);
            SET_MCDI_FIELD(mcdi_req->payload, PRIVILEGE_MASK_IN_FUNCTION,
                           function);
        }
        catch (ProviderException &)
        {
            return -1;
        }

        if (doMCDICall(fd, ioc) < 0)
            return -1;

        try {
            privs = GET_MCDI_FIELD(mcdi_req->payload,
                                   PRIVILEGE_MASK_OUT_OLD_MASK);
        }
        catch (const ProviderException &)
        {
            return -1;
        }

        return 0;
    }

    ///
    /// Get privileges of a function associated with a given interface.
    ///
    /// @param fd       File descriptor on which to call ioctl().
    /// @param ioc      Pointer to filled efx_ioctl structure.
    /// @param ifName   Interface name (will be set in @p ioc if not NULL).
    /// @param privs    Where to save obtained privileges.
    ///
    /// @return 0 on success, -1 on failure.
    static int getIntfFuncPrivileges(int fd,
                                     struct efx_ioctl &ioc,
                                     const char *ifName,
                                     unsigned int &privs)
    {
        uint32_t    pf;
        uint32_t    vf;

        if (ifName != NULL)
        {
            if (setIoctlIf(ioc, ifName) < 0)
                return -1;
        }

        if (getPFVF(fd, ioc, pf, vf) < 0)
            return -1;

        if (getFunctionPrivileges(fd, ioc, pf, vf, privs) < 0)
            return -1;

        return 0;
    }

    ///
    /// Get privileges of a NIC function.
    ///
    /// @param portName       Port on which to call ioctl().
    /// @param pf             Physical function.
    /// @param vf             Virtual function (may be not set).
    /// @param privilegeNames Where to save privilege names.
    /// @param privileges     Where to save privilege values.
    ///
    /// @return 0 on success, -1 on failure.
    static int doGetPrivileges(
                    const char *portName,
                    const Property<uint32> &pf,
                    const Property<uint32> &vf,
                    Property<Array_String> &privilegeNames,
                    Property<Array_uint32> &privileges)
    {
        uint32_t privs = 0;
        uint32_t flag = 0;
        uint32_t i = 0;

        unsigned int        pf_value;
        unsigned int        vf_value;
        struct efx_ioctl    ioc;
        int                 fd = -1;
        int                 rc = -1;

        if (prepareMCDICall(portName, fd, ioc) < 0)
            goto cleanup;

        if (pf.null)
        {
            if (getPFVF(fd, ioc, pf_value, vf_value) < 0)
                goto cleanup;
        }
        else
        {
            pf_value = pf.value;

            if (vf.null)
            {
                vf_value = MC_CMD_PRIVILEGE_MASK_IN_VF_NULL;
            }
            else
            {
                vf_value = vf.value;
            }
        }

        if (getFunctionPrivileges(fd, ioc, pf_value, vf_value, privs) < 0)
            goto cleanup;

        privileges.null = false;
        privilegeNames.null = false;

        for (i = 0; i < 32; i++)
        {
            flag = (1 << i);
            if (privs & flag)
            {
                privileges.value.append(i);
                privilegeNames.value.append(privilege_flag2name(flag));
            }
        }

        rc = 0;

cleanup:

        if (fd >= 0 && close(fd) < 0)
            PROVIDER_LOG_ERR("close() failed, errno %d ('%s')",
                             errno, strerror(errno));

        return rc;
    }

    ///
    /// Modify privileges of a physical or virtual function.
    ///
    /// @param fd             File descriptor on which to call ioctl()
    /// @param ioc            efx_ioctl structure
    /// @param pf             Physical Function number
    /// @param vf             Virtual Function number
    /// @param added_privs    Which privileges to add
    /// @param removed_privs  Which privileges to remove
    ///
    /// @return 0 on success, -1 on error
    static int modifyFunctionPrivileges(int fd,
                                        struct efx_ioctl &ioc,
                                        uint32_t pf,
                                        uint32_t vf,
                                        uint32_t added_privs,
                                        uint32_t removed_privs)
    {
        struct efx_mcdi_request *mcdi_req = NULL;

        uint32_t function;

        mcdi_req = &ioc.u.mcdi_request;
        memset(mcdi_req, 0, sizeof(*mcdi_req));

        mcdi_req->cmd = MC_CMD_PRIVILEGE_MODIFY;
        mcdi_req->len = MC_CMD_PRIVILEGE_MODIFY_IN_LEN;

        try {
            SET_MCDI_FIELD(mcdi_req->payload, PRIVILEGE_MODIFY_IN_FN_GROUP,
                           MC_CMD_PRIVILEGE_MODIFY_IN_ONE);

            function = 0;
            SET_MCDI_FIELD_PART(PRIVILEGE_MODIFY_IN_FUNCTION, PF,
                                function, pf);
            SET_MCDI_FIELD_PART(PRIVILEGE_MODIFY_IN_FUNCTION, VF,
                                function, vf);
            SET_MCDI_FIELD(mcdi_req->payload, PRIVILEGE_MODIFY_IN_FUNCTION,
                           function);

            SET_MCDI_FIELD(mcdi_req->payload, PRIVILEGE_MODIFY_IN_ADD_MASK,
                           added_privs);
            SET_MCDI_FIELD(mcdi_req->payload,
                           PRIVILEGE_MODIFY_IN_REMOVE_MASK,
                           removed_privs);
        }
        catch (const ProviderException &)
        {
            return -1;
        }

        if (doMCDICall(fd, ioc) < 0)
            return -1;

        return 0;
    }

    ///
    /// Modify privileges of a NIC function.
    ///
    /// @param portName       Port on which to call ioctl().
    /// @param pf             Physical function.
    /// @param vf             Virtual function (may be not set).
    /// @param addedMask      Privileges to be added.
    /// @param removedMask    Privileges to be removed.
    ///
    /// @return 0 on success, -1 on failure.
    static int doModifyPrivileges(
                    const char *portName,
                    const Property<uint32> &pf,
                    const Property<uint32> &vf,
                    const Property<String> &addedMask,
                    const Property<String> &removedMask)
    {
        uint32_t added_privs = 0;
        uint32_t removed_privs = 0;
        uint32_t pf_value = 0;
        uint32_t vf_value = 0;

        struct efx_ioctl  ioc;
        int               fd = -1;
        int               rc = -1;

        if (prepareMCDICall(portName, fd, ioc) < 0)
            goto cleanup;

        if (pf.null)
        {
            if (getPFVF(fd, ioc, pf_value, vf_value) < 0)
                goto cleanup;
        }
        else
        {
            pf_value = pf.value;

            if (vf.null)
            {
                vf_value = MC_CMD_PRIVILEGE_MASK_IN_VF_NULL;
            }
            else
            {
                vf_value = vf.value;
            }
        }

        if (!addedMask.null)
        {
            if (privileges_str2mask(addedMask.value.c_str(),
                                    added_privs) < 0)
                goto cleanup;
        }

        if (!removedMask.null)
        {
            if (privileges_str2mask(removedMask.value.c_str(),
                                    removed_privs) < 0)
                goto cleanup;
        }

        if (modifyFunctionPrivileges(fd, ioc, pf_value, vf_value,
                                     added_privs, removed_privs) < 0)
            goto cleanup;

        rc = 0;
cleanup:
        if (fd >= 0 && close(fd) < 0)
            PROVIDER_LOG_ERR("close() failed, errno %d ('%s')",
                             errno, strerror(errno));
        return rc;
    }

    int VMwarePort::getIntrModeration(const Array_String &paramNames,
                                      Array_uint32 &paramValues) const
    {
        struct ethtool_coalesce cmd;

        unsigned int i;

#define GCOALESCE_IF(x_) \
    if (strcasecmp(paramNames[i].c_str(), \
                   #x_) == 0)             \
        paramValues.append(cmd.x_)

        memset(&cmd, 0, sizeof(cmd));
        cmd.cmd = ETHTOOL_GCOALESCE;
        if (vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                             ETHTOOL_GCOALESCE, &cmd) < 0)
            return -1;

        for (i = 0; i < paramNames.size(); i++)
        {
            GCOALESCE_IF(rx_coalesce_usecs);
            else GCOALESCE_IF(rx_max_coalesced_frames);
            else GCOALESCE_IF(rx_coalesce_usecs_irq);
            else GCOALESCE_IF(rx_max_coalesced_frames_irq);
            else GCOALESCE_IF(tx_coalesce_usecs);
            else GCOALESCE_IF(tx_max_coalesced_frames);
            else GCOALESCE_IF(tx_coalesce_usecs_irq);
            else GCOALESCE_IF(tx_max_coalesced_frames_irq);
            else GCOALESCE_IF(stats_block_coalesce_usecs);
            else GCOALESCE_IF(use_adaptive_rx_coalesce);
            else GCOALESCE_IF(use_adaptive_tx_coalesce);
            else GCOALESCE_IF(pkt_rate_low);
            else GCOALESCE_IF(rx_coalesce_usecs_low);
            else GCOALESCE_IF(rx_max_coalesced_frames_low);
            else GCOALESCE_IF(tx_coalesce_usecs_low);
            else GCOALESCE_IF(tx_max_coalesced_frames_low);
            else GCOALESCE_IF(pkt_rate_high);
            else GCOALESCE_IF(rx_coalesce_usecs_high);
            else GCOALESCE_IF(rx_max_coalesced_frames_high);
            else GCOALESCE_IF(tx_coalesce_usecs_high);
            else GCOALESCE_IF(tx_max_coalesced_frames_high);
            else GCOALESCE_IF(rate_sample_interval);
            else
            {
                PROVIDER_LOG_ERR("%s(): unknown interrupt moderation "
                                 "parameter %s", __FUNCTION__,
                                 paramNames[i].c_str());
                paramValues.clear();
                return -1;
            }
        }

        return 0;
    }

    int VMwarePort::setIntrModeration(const Array_String &paramNames,
                                      const Array_uint32 &paramValues)
    {
        struct ethtool_coalesce cmd;

        unsigned int i;

#define SCOALESCE_IF(x_) \
    if (strcasecmp(paramNames[i].c_str(), \
                   #x_) == 0)             \
        cmd.x_ = paramValues[i]

        memset(&cmd, 0, sizeof(cmd));
        cmd.cmd = ETHTOOL_GCOALESCE;
        if (vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                             ETHTOOL_GCOALESCE, &cmd) < 0)
            return -1;

        for (i = 0; i < paramNames.size(); i++)
        {
            SCOALESCE_IF(rx_coalesce_usecs);
            else SCOALESCE_IF(rx_max_coalesced_frames);
            else SCOALESCE_IF(rx_coalesce_usecs_irq);
            else SCOALESCE_IF(rx_max_coalesced_frames_irq);
            else SCOALESCE_IF(tx_coalesce_usecs);
            else SCOALESCE_IF(tx_max_coalesced_frames);
            else SCOALESCE_IF(tx_coalesce_usecs_irq);
            else SCOALESCE_IF(tx_max_coalesced_frames_irq);
            else SCOALESCE_IF(stats_block_coalesce_usecs);
            else SCOALESCE_IF(use_adaptive_rx_coalesce);
            else SCOALESCE_IF(use_adaptive_tx_coalesce);
            else SCOALESCE_IF(pkt_rate_low);
            else SCOALESCE_IF(rx_coalesce_usecs_low);
            else SCOALESCE_IF(rx_max_coalesced_frames_low);
            else SCOALESCE_IF(tx_coalesce_usecs_low);
            else SCOALESCE_IF(tx_max_coalesced_frames_low);
            else SCOALESCE_IF(pkt_rate_high);
            else SCOALESCE_IF(rx_coalesce_usecs_high);
            else SCOALESCE_IF(rx_max_coalesced_frames_high);
            else SCOALESCE_IF(tx_coalesce_usecs_high);
            else SCOALESCE_IF(tx_max_coalesced_frames_high);
            else SCOALESCE_IF(rate_sample_interval);
            else
            {
                PROVIDER_LOG_ERR("%s(): unknown interrupt moderation "
                                 "parameter %s", __FUNCTION__,
                                 paramNames[i].c_str());
                return -1;
            }
        }

        cmd.cmd = ETHTOOL_SCOALESCE;
        if (vmwareEthtoolCmd(dev_file.c_str(), dev_name.c_str(),
                             ETHTOOL_SCOALESCE, &cmd) < 0)
            return -1;

        return 0;
    }
#else

    ///
    /// Get privileges of a NIC function.
    ///
    /// @param PCIAddr        PCI address on which to do operations.
    /// @param privilegeNames Where to save privilege names.
    /// @param privileges     Where to save privilege values.
    ///
    /// @return 0 on success, -1 on failure.
    static int doGetPrivileges(
                    const Property<String> &PCIAddr,
                    Property<Array_String> &privilegeNames,
                    Property<Array_uint32> &privileges)
    {
        uint32_t flag = 0;
        uint32_t i = 0;

        int rc = -1;

        sfvmk_privilege_t privs;

        memset(&privs, 0, sizeof(sfvmk_privilege_t));

        privs.type = SFVMK_MGMT_DEV_OPS_GET;
        memcpy(&privs.pciSBDF, PCIAddr.value.c_str(),
                                strlen(PCIAddr.value.c_str()));

        if (DrvMgmtCall(NULL,  SFVMK_CB_PRIVILEGE_REQUEST,
                        &privs) != VMK_OK)
        {
            PROVIDER_LOG_ERR("Privilege Retrieval failed");
            goto cleanup;
        }

        privileges.null = false;
        privilegeNames.null = false;

        for (i = 0; i < 32; i++)
        {
            flag = (1 << i);
            if (privs.privMask & flag)
            {
                privileges.value.append(i);
                privilegeNames.value.append(privilege_flag2name(flag));
            }
        }

        rc = 0;

cleanup:
        if (rc < 0)
            PROVIDER_LOG_ERR("getFuncPrivileges failed");

        return rc;
    }

    static int doModifyPrivileges(
                    const Property<String> &PCIAddr,
                    const Property<String> &addedMask,
                    const Property<String> &removedMask)
    {
        uint32_t added_privs = 0;
        uint32_t removed_privs = 0;

        int      rc = -1;

        sfvmk_privilege_t privs;

        memset(&privs, 0, sizeof(sfvmk_privilege_t));

        privs.type = SFVMK_MGMT_DEV_OPS_SET;
        memcpy(&privs.pciSBDF, PCIAddr.value.c_str(),
                                   strlen(PCIAddr.value.c_str()));

        if (!addedMask.null)
        {
            if (privileges_str2mask(addedMask.value.c_str(),
                                    added_privs ) < 0)
                goto cleanup;
        }

        if (!removedMask.null)
        {
            if (privileges_str2mask(removedMask.value.c_str(),
                                    removed_privs) < 0)
                goto cleanup;
        }

        privs.privMask = added_privs;
        privs.privRemoveMask = removed_privs;

        if (DrvMgmtCall(NULL,  SFVMK_CB_PRIVILEGE_REQUEST,
                        &privs) != VMK_OK)
        {
            PROVIDER_LOG_ERR("Privilege Modifation failed");
            return -1;
        }

        rc = 0;
cleanup:
        if (rc < 0)
            PROVIDER_LOG_ERR("modifyPrivileges failed");

        return rc;
    }

    int VMwarePort::getIntrModeration(const Array_String &paramNames,
                                      Array_uint32 &paramValues) const
    {
        sfvmk_intrCoalsParam_t intCoalsParam;
        unsigned int i;

        intCoalsParam.type = SFVMK_MGMT_DEV_OPS_GET;

#define GCOALESCE_IF(x_) \
    if (strcasecmp(paramNames[i].c_str(), \
                   #x_) == 0)             \
        paramValues.append(intCoalsParam.x_)

        if (DrvMgmtCall(dev_name.c_str(), SFVMK_CB_INTR_MODERATION_REQUEST,
                        &intCoalsParam) != VMK_OK)
        {
            PROVIDER_LOG_ERR("Interrupt Moderation parameter retrieval failed");
            return -1;
        }
        for (i = 0; i < paramNames.size(); i++)
        {
            GCOALESCE_IF(rxUsecs);
            else GCOALESCE_IF(rxMaxFrames);
            else GCOALESCE_IF(txUsecs);
            else GCOALESCE_IF(txMaxFrames);
            else GCOALESCE_IF(useAdaptiveRx);
            else GCOALESCE_IF(useAdaptiveTx);
            else GCOALESCE_IF(rateSampleInterval);
            else GCOALESCE_IF(pktRateLowWatermark);
            else GCOALESCE_IF(pktRateHighWatermark);
            else GCOALESCE_IF(rxUsecsLow);
            else GCOALESCE_IF(rxFramesLow);
            else GCOALESCE_IF(txUsecsLow);
            else GCOALESCE_IF(txFramesLow);
            else GCOALESCE_IF(rxUsecsHigh);
            else GCOALESCE_IF(rxFramesHigh);
            else GCOALESCE_IF(txUsecsHigh);
            else GCOALESCE_IF(txFramesHigh);
            else
            {
                PROVIDER_LOG_ERR("%s(): unknown interrupt moderation "
                                 "parameter %s", __FUNCTION__,
                                 paramNames[i].c_str());
                paramValues.clear();
                return -1;
            }
        }

        return 0;
    }

    int VMwarePort::setIntrModeration(const Array_String &paramNames,
                                      const Array_uint32 &paramValues)
    {
        sfvmk_intrCoalsParam_t intCoalsParam;
        unsigned int i;

        intCoalsParam.type = SFVMK_MGMT_DEV_OPS_GET;

#define SCOALESCE_IF(x_) \
    if (strcasecmp(paramNames[i].c_str(), \
                   #x_) == 0)             \
        intCoalsParam.x_ = paramValues[i]

        if (DrvMgmtCall(dev_name.c_str(), SFVMK_CB_INTR_MODERATION_REQUEST,
                        &intCoalsParam) != VMK_OK)
        {
            PROVIDER_LOG_ERR("%s(): Interrupt Moderation parameter"
                             "retrieval failed", __FUNCTION__);
            return -1;
        }

        for (i = 0; i < paramNames.size(); i++)
        {
            SCOALESCE_IF(rxUsecs);
            else SCOALESCE_IF(rxMaxFrames);
            else SCOALESCE_IF(txUsecs);
            else SCOALESCE_IF(txMaxFrames);
            else SCOALESCE_IF(useAdaptiveRx);
            else SCOALESCE_IF(useAdaptiveTx);
            else SCOALESCE_IF(rateSampleInterval);
            else SCOALESCE_IF(pktRateLowWatermark);
            else SCOALESCE_IF(pktRateHighWatermark);
            else SCOALESCE_IF(rxUsecsLow);
            else SCOALESCE_IF(rxFramesLow);
            else SCOALESCE_IF(txUsecsLow);
            else SCOALESCE_IF(txFramesLow);
            else SCOALESCE_IF(rxUsecsHigh);
            else SCOALESCE_IF(rxFramesHigh);
            else SCOALESCE_IF(txUsecsHigh);
            else SCOALESCE_IF(txFramesHigh);
            else
            {
                PROVIDER_LOG_ERR("%s(): unknown interrupt moderation "
                                 "parameter %s", __FUNCTION__,
                                 paramNames[i].c_str());
                return -1;
            }
        }

        intCoalsParam.type = SFVMK_MGMT_DEV_OPS_SET;

        if (DrvMgmtCall(dev_name.c_str(), SFVMK_CB_INTR_MODERATION_REQUEST,
                        &intCoalsParam) != VMK_OK)
        {
            PROVIDER_LOG_ERR("Interrupt Coalesce parameter configuration failed");
            return -1;
        }
        return 0;
    }
#endif
    class VMwareInterface : public Interface {
        const NIC *owner;
        Port *boundPort;
    public:
        VMwareInterface(const NIC *up, unsigned i) :
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

        // Dummy operator to make possible using of cimple::Array
        bool operator== (const VMwareInterface &rhs)
        {
            UNUSED(rhs);
            return false;
        }
    };

    bool VMwareInterface::ifStatus() const
    {
        // Implementation is blocked by SF bug 35613
        // However it seems that link status on ESXi
        // is the same as its status accessible via
        // esxcli.
        if (boundPort != NULL)
            return boundPort->linkStatus();

        THROW_PROVIDER_EXCEPTION;
        return false;
    }

    void VMwareInterface::enable(bool st)
    {
        // Implementation is blocked by SF bug 35613

        UNUSED(st);

        THROW_PROVIDER_EXCEPTION_FMT("Changing interface state "
                                     "is not implemented");
    }

    uint64 VMwareInterface::mtu() const
    {
        Ref<CIM_EthernetPort> cimEthPort;

        cimEthPort =
          getCIMEthPort(((VMwarePort *)boundPort)->dev_name.c_str());

        if (!cimEthPort)
            return 0;

        if (cimEthPort->ActiveMaximumTransmissionUnit.null)
            return 0;
        else
            return cimEthPort->ActiveMaximumTransmissionUnit.value;
    }

    void VMwareInterface::mtu(uint64 u)
    {
        // This should not be implemented - MTU
        // is set for vSwitch as a whole, change is
        // propagated to its uplinks automatically.

        UNUSED(u);

        return;
    }

    String VMwareInterface::ifName() const
    {
        if (boundPort == NULL)
            return "";

        return ((VMwarePort *)boundPort)->dev_name;
    }

#if defined(TARGET_CIM_SERVER_esxi_native)
    MACAddress VMwareInterface::currentMAC() const
    {
        char                 devName[SFVMK_DEV_NAME_LEN];
        sfvmk_macAddress_t   macAddress;

        if (boundPort == NULL)
            return MACAddress(0, 0, 0, 0, 0, 0);

        strncpy(devName, ((VMwarePort *)boundPort)->dev_name.c_str(), sizeof(devName));

        if (DrvMgmtCall(devName, SFVMK_CB_MAC_ADDRESS_GET, &macAddress) != VMK_OK)
            PROVIDER_LOG_ERR("%s(): DrvMgmtCall failed", __FUNCTION__);
        else
            return MACAddress(macAddress.macAddress[0],
                              macAddress.macAddress[1],
                              macAddress.macAddress[2],
                              macAddress.macAddress[3],
                              macAddress.macAddress[4],
                              macAddress.macAddress[5]);

        return MACAddress(0, 0, 0, 0, 0, 0);
     }
#else
    MACAddress VMwareInterface::currentMAC() const
    {
        int fd = 0;
        struct ifreq req;
        unsigned char *mac_address;

        if (boundPort == NULL)
            return MACAddress(0, 0, 0, 0, 0, 0);

        fd = open(((VMwarePort *)boundPort)->dev_file.c_str(), O_RDWR);
        if (fd < 0)
            return MACAddress(0, 0, 0, 0, 0, 0);

        memset(&req, 0, sizeof(req));
        strncpy(req.ifr_name,
                ((VMwarePort *)boundPort)->dev_name.c_str(),
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
#endif

    void VMwareInterface::currentMAC(const MACAddress& mac)
    {
        // See SF bug 35613

        UNUSED(mac);
    }

    class VMwareNICFirmware : public NICFirmware {
        const NIC *owner;
    public:
        VMwareNICFirmware(const NIC *o) :
            owner(o) {}
        virtual const NIC *nic() const { return owner; }
        virtual VersionInfo version() const;

        virtual InstallRC syncInstall(const char *file_name,
                                      bool force,
                                      const char *base64_hash)
        {
            return vmwareInstallFirmwareType(file_name, owner,
                                             FIRMWARE_MCFW,
                                             version(),
                                             force,
                                             base64_hash);
        }

        virtual void initialize() {};
    };

    class VMwareBootROM : public BootROM {
        const NIC *owner;
    public:
        VMwareBootROM(const NIC *o) :
            owner(o) {}
        virtual const NIC *nic() const { return owner; }
        virtual VersionInfo version() const;

        virtual InstallRC syncInstall(const char *file_name,
                                      bool force,
                                      const char *base64_hash)
        {
            return vmwareInstallFirmwareType(file_name, owner,
                                             FIRMWARE_BOOTROM,
                                             version(),
                                             force,
                                             base64_hash);
        }

        virtual void initialize() {};
    };

    class VMwareUEFIROM : public UEFIROM {
        const NIC *owner;
    public:
        VMwareUEFIROM(const NIC *o) :
            owner(o) {}
        virtual const NIC *nic() const { return owner; }
        virtual VersionInfo version() const;

        virtual InstallRC syncInstall(const char *file_name,
                                      bool force,
                                      const char *base64_hash)
        {
            return vmwareInstallFirmwareType(file_name, owner,
                                             FIRMWARE_UEFIROM,
                                             version(),
                                             force,
                                             base64_hash);
        }

        virtual void initialize() {};
    };

    class VMwareSUCFW : public SUCFW {
        const NIC *owner;
    public:
        VMwareSUCFW(const NIC *o) :
            owner(o) {}
        virtual const NIC *nic() const { return owner; }
        virtual VersionInfo version() const;

        virtual InstallRC syncInstall(const char *file_name,
                                      bool force,
                                      const char *base64_hash)
        {
            // Work around for Bug 84271
            if (!strcmp(version().string().c_str(), "0.0.0.0"))
                     return Install_OK;
            else
                return vmwareInstallFirmwareType(file_name, owner,
                                                 FIRMWARE_SUCFW,
                                                 version(),
                                                 force,
                                                 base64_hash);
        }

        virtual void initialize() {};
    };

    class VMwareDiagnostic : public Diagnostic {
        const NIC *owner;
        Result testPassed;
        static const char sampleDescr[];
        static const String diagGenName;
    public:
        VMwareDiagnostic(const NIC *o) :
            Diagnostic(sampleDescr), owner(o), testPassed(NotKnown) {}
        virtual Result syncTest()
        {
            // ETHTOOL_TEST is not available on ESXi -
            // see bug 35580
            testPassed = Passed;
            log().logStatus("passed");
            return Passed;
        }
        virtual Result result() const { return testPassed; }
        virtual const NIC *nic() const { return owner; }
        virtual void initialize() {};
        virtual const String& genericName() const { return diagGenName; }
    };

    const char VMwareDiagnostic::sampleDescr[] = "VMware Diagnostic";
    const String VMwareDiagnostic::diagGenName = "Diagnostic";

    class VMwareNIC : public NIC {
        VMwareNICFirmware nicFw;
        VMwareBootROM rom;
        VMwareUEFIROM uefirom;
        VMwareSUCFW sucfw;
        VMwareDiagnostic diag;

        int pci_domain;
        int pci_bus;
        int pci_device;
        int pci_device_id;

    public:

        Array<VMwarePort> ports;
        Array<VMwareInterface> intfs;

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
            uefirom.initialize();
            sucfw.initialize();
        }
        virtual void setupDiagnostics()
        {
            diag.initialize();
        }
    public:

        VMwareNIC(unsigned idx, NICDescr &descr) :
            NIC(idx),
            nicFw(this),
            rom(this),
            uefirom(this),
            sucfw(this),
            diag(this), pci_domain(descr.pci_domain),
            pci_bus(descr.pci_bus), pci_device(descr.pci_device),
            pci_device_id(descr.pci_device_id)
        {
            int i = 0;

            for (i = 0; i < (int)descr.ports.size(); i++)
            {
                ports.append(VMwarePort(this, i, descr.ports[i]));
                intfs.append(VMwareInterface(this, i));
            }
        }

        virtual int getFullVPD(bool staticVPD,
                               Array<VPDField> &parsedFields) const;
        virtual VitalProductData vitalProductData() const;
        Connector connector() const;

        uint64 supportedMTU() const { return EFX_MAX_MTU; }

#ifdef TARGET_CIM_SERVER_esxi_native
        virtual bool forAllFw(ElementEnumerator& en)
        {
            if (!en.process(nicFw))
                return false;
            if (!en.process(rom))
                return false;
            if (!en.process(uefirom))
                return false;
            return en.process(sucfw);
        }
        virtual bool forAllFw(ConstElementEnumerator& en) const
        {
            if (!en.process(nicFw))
                return false;
            if (!en.process(rom))
                return false;
            if (!en.process(uefirom))
                return false;
            return en.process(sucfw);
        }
#else
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
#endif
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
        virtual uint32_t getPCIDeviceID() const
        {
            return pci_device_id;
        }
#ifndef TARGET_CIM_SERVER_esxi_native
        virtual int getPFVFByPCIAddr(const PCIAddress &searchPCIAddr,
                                     bool &found, uint32 &pf_out,
                                     uint32 &vf_out,
                                     bool &vf_null) const;

        virtual int getAdminInterface(String &ifName) const;
#endif
    };

    int VMwareNIC::getFullVPD(bool staticVPD,
                              Array<VPDField> &parsedFields) const
    {
        if (ports.size() <= 0)
        {
            PROVIDER_LOG_ERR("%s(): NIC has no ports", __FUNCTION__);
            return -1;
        }
        else
            return getVPD(ports[0].dev_name.c_str(), 0,
                          SFU_DEVICE_TYPE(pci_device_id),
                          staticVPD, parsedFields);
    }

    VitalProductData VMwareNIC::vitalProductData() const
    {
        Array<VPDField> parsedFields;

        if (getFullVPD(true, parsedFields) < 0)
            return VitalProductData("", "", "", "", "", "");
        else
        {
            String   p_name;
            String   pn;
            String   sn;

            unsigned int i;

            for (i = 0; i < parsedFields.size(); i++)
            {
                if (strcmp(parsedFields[i].name.c_str(), "IDTag") == 0)
                    p_name = String(parsedFields[i].data.data());
                else if (strcmp(parsedFields[i].name.c_str(), "PN") == 0)
                    pn = String(parsedFields[i].data.data());
                else if (strcmp(parsedFields[i].name.c_str(), "SN") == 0)
                    sn = String(parsedFields[i].data.data());
            }

            VitalProductData vpd(sn.c_str(), "", sn.c_str(), pn.c_str(),
                                 p_name.c_str(), pn.c_str());
            return vpd;
        }
    }

    NIC::Connector VMwareNIC::connector() const
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

    PCIAddress VMwareNIC::pciAddress() const
    {
        return PCIAddress(pci_domain, pci_bus, pci_device);
    }

#ifndef TARGET_CIM_SERVER_esxi_native
    int VMwareNIC::getPFVFByPCIAddr(const PCIAddress &searchPCIAddr,
                                    bool &found, uint32 &pf_out,
                                    uint32 &vf_out,
                                    bool &vf_null) const
    {
        unsigned int      i;
        int               rc = -1;
        int               fd;
        struct efx_ioctl  ioc;
        unsigned int      privs;

        if (prepareMCDICall("", fd, ioc) < 0)
            goto cleanup;

        found = false;
        for (i = 0; i < this->ports.size(); i++)
        {
            rc = PCIToPFVF(fd, ioc, this->ports[i].dev_name.c_str(),
                           this->ports[i].pciAddress(),
                           searchPCIAddr, found, pf_out, vf_out);

            if (rc == 0)
                break;
        }

        rc = 0;

cleanup:

        if (fd >= 0 && close(fd) < 0)
            PROVIDER_LOG_ERR("close() failed, errno %d ('%s')",
                             errno, strerror(errno));

        if (vf_out == MC_CMD_PRIVILEGE_MASK_IN_VF_NULL)
            vf_null = true;
        else
            vf_null = false;

        return rc;
    }

    int VMwareNIC::getAdminInterface(String &ifName) const
    {
        unsigned int      i;
        int               rc = -1;
        int               fd;
        struct efx_ioctl  ioc;
        unsigned int      privs;

        ifName = "";

        if (prepareMCDICall("", fd, ioc) < 0)
            goto cleanup;

        for (i = 0; i < this->ports.size(); i++)
        {
            if (getIntfFuncPrivileges(fd, ioc,
                                      this->ports[i].dev_name.c_str(),
                                      privs) < 0)
                continue;

            if (privs & MC_CMD_PRIVILEGE_MASK_IN_GRP_ADMIN)
            {
                ifName = this->ports[i].dev_name;
                rc = 0;
                goto cleanup;
            }
        }

        PROVIDER_LOG_ERR("Failed to find interface with ADMIN privilege on "
                         "NIC '%s'", this->tag().c_str());

cleanup:

        if (fd >= 0 && close(fd) < 0)
            PROVIDER_LOG_ERR("close() failed, errno %d ('%s')",
                             errno, strerror(errno));

        return rc;
    }

#endif

    ///
    /// Get information about required firmware image.
    ///
    /// @param owner          NIC object pointer
    /// @param fwType         Firmware type
    /// @param subType  [out] Firmware subtype
    /// @param fName    [out] Firmware image default file name
    ///
    /// @return 0 on success, -1 on failure
    ///
    static int getFwImageInfo(const NIC *owner, UpdatedFirmwareType fwType,
                              unsigned int &subType, String &fName)
    {
        String defPath;
        int    i;

        const VMwareNIC *vmwareNIC = dynamic_cast<const VMwareNIC *>(owner);

        fName = String("");
        subType = 0;

        if (vmwareNIC == NULL || vmwareNIC->ports.size() <= 0)
            return -1;

        if (getFwSubType(vmwareNIC->ports[0].dev_name.c_str(),
                         fwType,
                         SFU_DEVICE_TYPE(vmwareNIC->getPCIDeviceID()),
                         subType) != 0)
            return -1;

        for (i = 0; fwFileTable[i].fName != NULL; i++)
            if (fwFileTable[i].type == fwType &&
                fwFileTable[i].subtype == subType)
                break;

        if (fwFileTable[i].fName != NULL)
            fName = String(fwFileTable[i].fName);

        return 0;
    }

#ifdef TARGET_CIM_SERVER_esxi_native
    ///
    /// Get default path for a given firmware type.
    ///
    /// @param owner    NIC object
    /// @param fwType   Firmware type
    /// @param fName    Firmware image file name
    ///
    /// @return default path
    ///
    static String getDefaultFwPath(const NIC *owner,
                                   UpdatedFirmwareType fwType,
                                   const String &fName)
    {
        String          defPath;

        if (!fName.empty())
        {
            defPath = String("/image/");
            if (fwType == FIRMWARE_BOOTROM)
                defPath.append("bootrom/");
            else if (fwType == FIRMWARE_MCFW)
                defPath.append("mcfw/");
            else if (fwType == FIRMWARE_UEFIROM)
                defPath.append("uefirom/");
            else
                defPath.append("sucfw/");

            defPath.append(fName);
        }

        return defPath;
    }
#else
    static String getDefaultFwPath(const NIC *owner,
                                   UpdatedFirmwareType fwType,
                                   const String &fName)
    {
        String          defPath;

        if (!fName.empty())
        {
            defPath = String("/image/");
            if (fwType == FIRMWARE_BOOTROM)
                defPath.append("bootrom/");
            else
                defPath.append("mcfw/");
            defPath.append(fName);
        }

        return defPath;
    }
#endif

    ///
    /// Download file from remote location specified by URI.
    ///
    /// @param uri         Data URI
    /// @param passwd      Password (NULL if not required)
    /// @param f           File to write loaded data
    ///
    /// @return Install_OK on success, error code on failure
    ///
    static SWElement::InstallRC uri_get_file(const char *uri,
                                             const char *passwd,
                                             FILE *f)
    {
        CURL          *curl = NULL;
        CURLcode       rc_curl;
        long int       http_code = 0;

        SWElement::InstallRC  rc = SWElement::Install_OK;

        if (uri == NULL || f == NULL)
        {
            PROVIDER_LOG_ERR("%s(): incorrect arguments", __FUNCTION__);
            return SWElement::Install_Error;
        }

        curl = curl_easy_init();
        if (curl == NULL)
        {
            PROVIDER_LOG_ERR("curl_easy_init() failed, errno %d ('%s')",
                             errno, strerror(errno));
            return SWElement::Install_Error;
        }

        if ((rc_curl = curl_easy_setopt(curl, CURLOPT_URL,
                                        uri)) != CURLE_OK)
        {
            PROVIDER_LOG_ERR("curl_easy_setopt(CURLOPT_URL) failed: %s",
                             curl_easy_strerror(rc_curl));
            rc = SWElement::Install_Error;
            goto curl_fail;
        }
        if ((rc_curl =
              curl_easy_setopt(curl, CURLOPT_WRITEDATA,
                               f)) != CURLE_OK)
        {
            PROVIDER_LOG_ERR("curl_easy_setopt(CURLOPT_WRITEDATA) "
                             "failed: %s",
                             curl_easy_strerror(rc_curl));
            rc = SWElement::Install_Error;
            goto curl_fail;
        }

        if (passwd != NULL)
        {
            if ((rc_curl = curl_easy_setopt(curl, CURLOPT_PASSWORD,
                                            passwd)) != CURLE_OK)
            {
                PROVIDER_LOG_ERR("curl_easy_setopt(CURLOPT_PASSWORD) "
                                 "failed: %s",
                                 curl_easy_strerror(rc_curl));
                rc = SWElement::Install_Error;
                goto curl_fail;
            }
        }

        // Disable SSL certificates checking
        if ((rc_curl = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER,
                                        0L)) != CURLE_OK)
        {
            PROVIDER_LOG_ERR("curl_easy_setopt(CURLOPT_SSL_VERIFYPEER) "
                             "failed: %s",
                             curl_easy_strerror(rc_curl));
            rc = SWElement::Install_Error;
            goto curl_fail;
        }
        if ((rc_curl = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST,
                                        0L)) != CURLE_OK)
        {
            PROVIDER_LOG_ERR("curl_easy_setopt(CURLOPT_SSL_VERIFYHOST) "
                             "failed: %s",
                             curl_easy_strerror(rc_curl));
            rc = SWElement::Install_Error;
            goto curl_fail;
        }

        if ((rc_curl = curl_easy_perform(curl)) != CURLE_OK)
        {
            PROVIDER_LOG_ERR("curl_easy_perform() failed: %s",
                             curl_easy_strerror(rc_curl));
            if (rc_curl == CURLE_TFTP_NOTFOUND ||
                rc_curl == CURLE_REMOTE_FILE_NOT_FOUND)
                rc = SWElement::Install_Not_Found;
            else
                rc = SWElement::Install_Error;
            goto curl_fail;
        }

        if (strcasecmp_start(uri, HTTP_PROTO) == 0 ||
            strcasecmp_start(uri, HTTPS_PROTO) == 0)
        {
            rc_curl = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,
                                        &http_code);
            if (rc_curl != CURLE_OK)
            {
                PROVIDER_LOG_ERR("curl_easy_getinfo(CURLINFO_RESPONSE_CODE)"
                                 " failed: %s",
                                 curl_easy_strerror(rc_curl));
                rc = SWElement::Install_Error;
                goto curl_fail;
            }
            if (http_code >= 400)
            {
                PROVIDER_LOG_ERR("Trying to obtain '%s' resulted in %ld "
                                 "HTTP status code", uri, http_code);
                if (http_code == 404)
                    rc = SWElement::Install_Not_Found;
                else
                    rc = SWElement::Install_Error;
                goto curl_fail;
            }
        }

curl_fail:
        curl_easy_cleanup(curl);
        return rc;
    }

    ///
    /// Check file SHA-1 hash.
    ///
    /// @param file_name    File to be checked
    /// @param base64_hash  Expected hash, encoded in Base64
    ///
    /// @return true on success, false on failure
    ///
    static bool checkFileHash(const char *file_name,
                              const char *base64_hash)
    {
#define CHUNK_SIZE 10000
        ssize_t  dec_size;
        char    *hash = NULL;
        FILE    *f = NULL;
        char     chunk[CHUNK_SIZE];
        size_t   read_len;
        bool     result = true;

        EVP_MD_CTX    mdctx;
        unsigned char md_hash[EVP_MAX_MD_SIZE];
        unsigned int  md_len = 0;

        if (base64_hash == NULL || strlen(base64_hash) == 0)
            return true;

        dec_size = base64_dec_size(base64_hash);
        if (dec_size < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to determine decoded "
                             "hash size", __FUNCTION__);
            return false;
        }

        hash = new char[dec_size];
        if (hash == NULL)
        {
            PROVIDER_LOG_ERR("%s(): failed to allocate memory "
                             "for decoded hash", __FUNCTION__);
            return false;
        }

        if (base64_decode(hash, base64_hash) < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to decode hash",
                             __FUNCTION__);
            result = false;
            goto cleanup;
        }

        f = fopen(file_name, "r");
        if (f == NULL)
        {
            PROVIDER_LOG_ERR("%s(): failed to open '%s' for reading",
                             __FUNCTION__, file_name);
            result = false;
            goto cleanup;
        }

        EVP_MD_CTX_init(&mdctx);
        if (!EVP_DigestInit_ex(&mdctx, EVP_sha1(), NULL))
        {
            PROVIDER_LOG_ERR("%s(): EVP_DigestInit_ex() failed",
                             __FUNCTION__);
            result = false;
            EVP_MD_CTX_cleanup(&mdctx);
            goto cleanup;
        }

        while (!feof(f))
        {
            int ferr;

            read_len = fread(chunk, 1, CHUNK_SIZE, f);
            ferr = ferror(f);
            if (ferr != 0)
            {
                PROVIDER_LOG_ERR("%s(): read error encountered",
                                 __FUNCTION__);
                result = false;
                goto cleanup;
            }

            if (read_len > 0)
            {
                if (!EVP_DigestUpdate(&mdctx, chunk, read_len))
                {
                    PROVIDER_LOG_ERR("%s(): EVP_DigestUpdate() failed",
                                     __FUNCTION__);
                    result = false;
                    EVP_MD_CTX_cleanup(&mdctx);
                    goto cleanup;
                }
            }
        }

        if (!EVP_DigestFinal_ex(&mdctx, md_hash, &md_len))
        {
            PROVIDER_LOG_ERR("%s(): EVP_DigestFinal_ex() failed",
                             __FUNCTION__);
            result = false;
            EVP_MD_CTX_cleanup(&mdctx);
            goto cleanup;
        }
        if (!EVP_MD_CTX_cleanup(&mdctx))
        {
            PROVIDER_LOG_ERR("%s(): EVP_MD_CTX_cleanup() failed",
                             __FUNCTION__);
            result = false;
            goto cleanup;
        }

        if ((ssize_t)md_len != dec_size ||
            memcmp(md_hash, hash, md_len) != 0)
            result = false;

cleanup:

        delete[] hash;
        if (f != NULL)
          fclose(f);
        return result;
    }

    ///
    /// Check that firmware image is applicable according
    /// to firmware type, subtype and version.
    ///
    /// @param filename         Where firmware image is stored
    /// @param fwType           Firmware type
    /// @param subType          Firmware subtype
    /// @param curVersion       Version of the currently installed firmware
    /// @param imgVersion [out] Version of the firmware image
    /// @param force            Will --force option be specified for
    ///                         sfupdate
    ///
    /// @return true if firmware is applicable, false otherwise
    ///
    static bool checkImageApplicability(const char *filename,
                                        UpdatedFirmwareType fwType,
                                        unsigned int subType,
                                        const VersionInfo &curVersion,
                                        VersionInfo *imgVersion,
                                        bool force)
    {
#ifdef TARGET_CIM_SERVER_esxi_native
        FILE                  *f;
        VersionInfo           newVersion;
        efx_image_header_t    *header;
        size_t                fileSize;
        char                  *pBuffer = NULL;
        efx_image_info_t      imageInfo;
        bool                  result = false;

        f = fopen(filename, "r");
        if (f == NULL)
        {
            PROVIDER_LOG_ERR("%s(): attempt to open '%s' failed",
                             __FUNCTION__, filename);
            goto cleanup;
        }
        if (fseek(f, 0, SEEK_END) != 0)
        {
            PROVIDER_LOG_ERR("%s(): Failed to reach end of file",
                             __FUNCTION__);
            goto cleanup;
        }
        if ((fileSize = ftell(f)) == -1)
        {
            PROVIDER_LOG_ERR("%s(): Failed to read filesize",
                             __FUNCTION__);
            goto cleanup;
        }
        rewind(f);
        pBuffer = (char*) calloc(fileSize, sizeof(char));
        if (pBuffer == NULL)
        {
            PROVIDER_LOG_ERR("%s(): Memory allocation failed.", __FUNCTION__);
            goto cleanup;
        }
        if (fread(pBuffer, 1, fileSize, f) != fileSize)
        {
            PROVIDER_LOG_ERR("%s(): Failed to read firmware image.", __FUNCTION__);
            goto cleanup;
        }
        if ((efx_check_reflash_image(pBuffer, fileSize, &imageInfo)) != 0)
        {
            PROVIDER_LOG_ERR("%s(): Check Reflash Image Failed.", __FUNCTION__);
            goto cleanup;
        }
        header = imageInfo.eii_headerp;

        if (header->eih_magic != IMAGE_HEADER_MAGIC)
        {
            PROVIDER_LOG_ERR("%s(): firmware image header magic mismatch",
                             __FUNCTION__);
            goto cleanup;
        }
        if (!((header->eih_type == IMAGE_TYPE_BOOTROM &&
               fwType == FIRMWARE_BOOTROM) ||
              (header->eih_type == IMAGE_TYPE_MCFW &&
               fwType == FIRMWARE_MCFW) ||
              (header->eih_type == IMAGE_TYPE_UEFIROM &&
               fwType == FIRMWARE_UEFIROM) ||
              (header->eih_type == IMAGE_TYPE_MUMFW &&
               fwType == FIRMWARE_SUCFW)))
        {
            PROVIDER_LOG_ERR("%s(): firmware image type mismatch",
                             __FUNCTION__);
            goto cleanup;
        }
        if (header->eih_subtype != subType)
        {
            PROVIDER_LOG_ERR("%s(): firmware image subtype mismatch",
                             __FUNCTION__);
            goto cleanup;
        }
        newVersion = VersionInfo(header->eih_code_version_a,
                                 header->eih_code_version_b,
                                 header->eih_code_version_c,
                                 header->eih_code_version_d);
        if (imgVersion != NULL)
            *imgVersion = newVersion;

        if (!force && newVersion <= curVersion)
        {
            PROVIDER_LOG_ERR("%s(): firmware of the same or newer "
                             "version is installed already",
                             __FUNCTION__);
            goto cleanup;
        }
        result = true;

cleanup:
        if (f != NULL)
            fclose(f);
        if (pBuffer != NULL)
            free(pBuffer);

        return result;

#else
        image_header_t   header;
        FILE             *f;
        VersionInfo      newVersion;

        memset(&header, 0, sizeof(header));

        f = fopen(filename, "r");
        if (f == NULL)
        {
            PROVIDER_LOG_ERR("%s(): attempt to open '%s' failed",
                             __FUNCTION__, filename);
            return false;
        }
        if (fread(&header, 1, sizeof(header), f) != sizeof(header))
        {
            PROVIDER_LOG_ERR("%s(): failed to read firmware image "
                             "header from '%s'",
                             __FUNCTION__, filename);
            fclose(f);
            return false;
        }
        fclose(f);

        if (header.ih_magic != IMAGE_HEADER_MAGIC)
        {
            PROVIDER_LOG_ERR("%s(): firmware image header magic mismatch",
                             __FUNCTION__);
            return false;
        }
        if (!((header.ih_type == IMAGE_TYPE_BOOTROM &&
               fwType == FIRMWARE_BOOTROM) ||
              (header.ih_type == IMAGE_TYPE_MCFW &&
               fwType == FIRMWARE_MCFW)))
        {
            PROVIDER_LOG_ERR("%s(): firmware image type mismatch",
                             __FUNCTION__);
            return false;
        }

        if (header.ih_subtype != subType)
        {
            PROVIDER_LOG_ERR("%s(): firmware image subtype mismatch",
                             __FUNCTION__);
            return false;
        }

        newVersion = VersionInfo(header.ih_code_version_a,
                                 header.ih_code_version_b,
                                 header.ih_code_version_c,
                                 header.ih_code_version_d);
        if (imgVersion != NULL)
            *imgVersion = newVersion;

        if (!force && newVersion <= curVersion)
        {
            PROVIDER_LOG_ERR("%s(): firmware of the same or newer "
                             "version is installed already",
                             __FUNCTION__);
            return false;
        }

        return true;
#endif
    }

    ///
    /// Check whether local file contains an applicable firmware image,
    /// and if it does, return its version.
    ///
    /// @param path               Where to find firmware image
    /// @param owner              NIC on which firmware resides
    /// @param fwType             Firmware type
    /// @param curVersion         Version of the currently installed
    ///                           firmware
    /// @param applicable   [out] Whether firmware image is applicable or
    ///                           not
    /// @param imgVersion   [out] Version of the firmware image (if it is
    ///                           applicable)
    ///
    /// @return 0 on sucess, -1 on failure
    ///
    static int vmwareGetLocalFwImageVersion(const char *path,
                                            const NIC *owner,
                                            UpdatedFirmwareType fwType,
                                            const VersionInfo &curVersion,
                                            bool &applicable,
                                            VersionInfo &imgVersion)
    {
        String          strPath;
        String          strDefPath;
        unsigned int    subType;
        String          fName;
        bool            was_completed;

        if (getFwImageInfo(owner, fwType, subType, fName) < 0)
            return -1;

        strDefPath = getDefaultFwPath(owner, fwType, fName);
        strPath = fixFwImagePath(path,
                                 strDefPath.c_str(),
                                 was_completed);

        applicable = checkImageApplicability(strPath.c_str() +
                                             strlen(FILE_PROTO),
                                             fwType, subType,
                                             curVersion,
                                             &imgVersion, true);
        return 0;
    }

    ///
    /// Install firmware on a NIC from given image.
    ///
    /// @param owner          NIC class pointer
    /// @param fileName       From where to get firmware image
    /// @param pathCompleted  Whether path completion with default
    ///                       image name was used or not
    /// @param fwType         Firmware type (BootROM or Controller)
    /// @param subType        Firmware subtype
    /// @param curVersion     Version of the currently installed firmware
    /// @param force          Use sfupdate with --force option
    /// @param base64_hash    SHA-1 hash of firmware image,
    ///                       encoded in Base64
    ///
    /// @return Install_OK on success, error code on failure
    ///
    static SWElement::InstallRC vmwareInstallFirmware(
                                          const NIC *owner,
                                          const char *fileName,
                                          bool pathCompleted,
                                          UpdatedFirmwareType fwType,
                                          unsigned int subType,
                                          const VersionInfo &curVersion,
                                          bool force,
                                          const char *base64_hash)
    {
        Auto_Mutex    guard(tmpFilesArrLock);

        int   rc = 0;
        char  cmd[CMD_MAX_LEN];
        int   fd = -1;
        char  tmp_file[] = "/tmp/sf_firmware_XXXXXX";
        int   tmp_file_used = 0;

        SWElement::InstallRC install_rc;
        VersionInfo imgVersion;
        const char *sfupdate_image_fn = NULL;

#ifdef TARGET_CIM_SERVER_esxi_native
        size_t fileSize = 0;
        FILE   *fPtr =  NULL;
        char   *pBuffer = NULL;
#endif
        if (((VMwareNIC *)owner)->ports.size() <= 0)
        {
            PROVIDER_LOG_ERR("No ports found");
            return SWElement::Install_Error;
        }

        if (strcasecmp_start(fileName, FILE_PROTO) == 0)
        {
            int fd_aux;

            sfupdate_image_fn = fileName + strlen(FILE_PROTO);

            rc = snprintf(cmd, CMD_MAX_LEN, "sfupdate --adapter=%s "
                          "--write --image=%s%s",
                          ((VMwareNIC *)owner)->ports[0].dev_name.c_str(),
                          fileName + strlen(FILE_PROTO),
                          force ? " --force" : "");
            if (rc < 0 || rc >= CMD_MAX_LEN)
            {
                PROVIDER_LOG_ERR("Failed to format sfupdate command");
                return SWElement::Install_Error;
            }

            fd_aux = open(sfupdate_image_fn, O_RDONLY);
            if (fd_aux < 0)
            {
                if (errno == ENOENT)
                {
                    PROVIDER_LOG_ERR("File '%s' not found",
                                     sfupdate_image_fn);
                    if (pathCompleted)
                        return SWElement::Install_Not_Found;
                    else
                        return SWElement::Install_Error;
                }
                else
                {
                    PROVIDER_LOG_ERR("File '%s' cannot be opened "
                                     "for reading, errno = %d ('%s')",
                                     sfupdate_image_fn, errno,
                                     strerror(errno));
                    return SWElement::Install_Error;
                }
            }
            else
                close(fd_aux);
        }
        else if (strcasecmp_start(fileName, TFTP_PROTO) == 0 ||
                 strcasecmp_start(fileName, FTP_PROTO) == 0 ||
                 strcasecmp_start(fileName, SFTP_PROTO) == 0 ||
                 strcasecmp_start(fileName, HTTP_PROTO) == 0 ||
                 strcasecmp_start(fileName, HTTPS_PROTO) == 0)
        {
            FILE *f;

            fd = mkstemp(tmp_file);
            if (fd < 0)
            {
                PROVIDER_LOG_ERR("mkstemp('%s') failed, errno %d ('%s')",
                                 tmp_file, errno, strerror(errno));
                return SWElement::Install_Error;
            }

            f = fdopen(fd, "w");
            if (f == NULL)
            {
                PROVIDER_LOG_ERR("fdopen() failed, errno %d ('%s')",
                                 errno, strerror(errno));
                close(fd);
                return SWElement::Install_Error;
            }

            install_rc = uri_get_file(fileName, NULL, f);
            if (install_rc != SWElement::Install_OK)
            {
                fclose(f);
                if (pathCompleted &&
                    install_rc == SWElement::Install_Not_Found)
                    return SWElement::Install_Not_Found;
                else
                    return SWElement::Install_Error;
            }
            fclose(f);

            rc = snprintf(cmd, CMD_MAX_LEN, "sfupdate --adapter=%s "
                          "--write --image=%s%s",
                          ((VMwareNIC *)owner)->ports[0].dev_name.c_str(),
                          tmp_file, force ? " --force" : "");
            if (rc < 0 || rc >= CMD_MAX_LEN)
            {
                PROVIDER_LOG_ERR("Failed to format sfupdate command");
                unlink(tmp_file);
                return SWElement::Install_Error;
            }

            tmp_file_used = 1;
            sfupdate_image_fn = tmp_file;
        }
        else // Protocol not supported
        {
            PROVIDER_LOG_ERR("Unknown protocol for path '%s'", fileName);
            return SWElement::Install_Error;
        }

        if (!checkFileHash(sfupdate_image_fn,
                           base64_hash))
        {
            PROVIDER_LOG_ERR("Firmware image hash check failed");
            if (tmp_file_used)
                unlink(tmp_file);
            return SWElement::Install_Error;
        }

        if (!checkImageApplicability(sfupdate_image_fn, fwType, subType,
                                     curVersion, &imgVersion, force))
        {
            PROVIDER_LOG_ERR("Firmware image applicability "
                             "check failed");
            if (tmp_file_used)
                unlink(tmp_file);
            return SWElement::Install_NA;
        }

#ifdef TARGET_CIM_SERVER_esxi_native
        fPtr = fopen(sfupdate_image_fn, "r");
        if (fPtr == NULL)
        {
            PROVIDER_LOG_ERR("%s(): attempt to open '%s' failed",
                             __FUNCTION__, sfupdate_image_fn);
            rc = -1;
            goto cleanup;
        }

        fseek(fPtr, 0, SEEK_END);
        fileSize = ftell(fPtr);
        rewind(fPtr);
        pBuffer = (char*) calloc(fileSize, sizeof(char));

        if (pBuffer == NULL)
        {
            PROVIDER_LOG_ERR("%s(): memory allocation failed",
                             __FUNCTION__);
            rc = -1;
            goto cleanup;
        }
        if (fread(pBuffer, 1, fileSize, fPtr) != fileSize)
        {
            PROVIDER_LOG_ERR("%s(): failed to read firmware image",
                             __FUNCTION__);
            rc = -1;
            goto cleanup;
        }

        sfvmk_imgUpdate_t imgUpdate;

        imgUpdate.pFileBuffer = (vmk_uint64)((vmk_uint32)pBuffer);
        imgUpdate.size = fileSize;

        PROVIDER_LOG_DBG("Image Update called for  fwType:%d with image version:%d.%d.%d.%d",
                          fwType, imgVersion.major(), imgVersion.minor(),
                          imgVersion.revision(), imgVersion.build());

        if (DrvMgmtCall(((VMwareNIC *)owner)->ports[0].dev_name.c_str(),
                        SFVMK_CB_IMG_UPDATE, &imgUpdate) != VMK_OK)
        {
            PROVIDER_LOG_ERR("%s(): Driver Management Call failed", __FUNCTION__);
            rc = -1;
            goto cleanup;
        }
        if (fwType == FIRMWARE_MCFW || fwType == FIRMWARE_SUCFW)
            sleep(CIMPROV_MC_REBOOT_TIME_OUT_SEC);

cleanup:
           if (pBuffer)
               free(pBuffer);
           if (fPtr)
               fclose(fPtr);
           if (tmp_file_used)
               unlink(tmp_file);
           if (rc < 0)
               return SWElement::Install_Error;
           else
               return SWElement::Install_OK;
#else
        PROVIDER_LOG_DBG("Run sfupdate command '%s'", cmd);
        fd = sfupdatePOpen(cmd);
        if (fd < 0)
        {
            if (tmp_file_used)
                unlink(tmp_file);
            return SWElement::Install_Error;
        }

        sfupdateLogOutput(fd);
        close(fd);

        if (tmp_file_used)
            unlink(tmp_file);
        return SWElement::Install_OK;
#endif
    }

    class VMwareDriver : public Driver {
        const Package *owner;
        static const String drvName;
    public:
        VMwareDriver(const Package *pkg, const String& d,
                     const String& sn) :
            Driver(d, sn), owner(pkg) {}
        virtual VersionInfo version() const;
        virtual void initialize() {};
        virtual InstallRC syncInstall(const char *, bool, const char *)
        {
            return Install_Error;
        }
        virtual const String& genericName() const { return description(); }
        virtual const Package *package() const { return owner; }
    };

    ///
    /// Retrieve driver version from system CIM provider
    ///
    /// @return driver version on success, DEFAULT_VERSION_STR otherwise
    ///
    static VersionInfo vmwareGetDriverVersion()
    {
        Ref<CIM_SoftwareIdentity> cimModel =
                                      CIM_SoftwareIdentity::create();
        Ref<Instance>             cimInstance;
        Ref<CIM_SoftwareIdentity> cimSoftId;

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
        else
            return VersionInfo(cimSoftId->VersionString.value.c_str());
    }

#ifndef TARGET_CIM_SERVER_esxi_native
    VersionInfo VMwareDriver::version() const
    {
        struct ethtool_drvinfo   drvinfo;
        struct ifreq             ifr;
        int                      fd;
        char                     device_path[PATH_MAX_LEN];

        Array<String> devDirList;

        unsigned int i;

        if (getDirContents(DEV_PATH, devDirList) < 0)
            return VersionInfo(DEFAULT_VERSION_STR);

        for (i = 0; i < devDirList.size(); i++)
        {
            const char *devName = devDirList[i].c_str();

            if (devName[0] == '.')
                continue;
            if (strlen(devName) > IFNAMSIZ)
                continue;

            if (snprintf(device_path, PATH_MAX_LEN, "%s/%s",
                        DEV_PATH, devName) >= PATH_MAX_LEN)
                continue;
            fd = open(device_path, O_RDWR);
            if (fd < 0)
                continue;

            memset(&ifr, 0, sizeof(ifr));
            memset(&drvinfo, 0, sizeof(drvinfo));
            drvinfo.cmd = ETHTOOL_GDRVINFO;
            ifr.ifr_data = (char *)&drvinfo;
            strcpy(ifr.ifr_name, devName);

            if (ioctl(fd, SIOCETHTOOL, &ifr) == 0)
            {
                close(fd);

                if (strcmp(drvinfo.driver, "sfc") == 0)
                   return VersionInfo(drvinfo.version);
            }
            else
                close(fd);
        }

        // We failed to get it via ethtool (possible reason: no
        // Solarflare interfaces are presented) - we try to get it
        // from VMware root/cimv2 standard objects.

        return vmwareGetDriverVersion();
    }
#else
    VersionInfo VMwareDriver::version() const
    {
        sfvmk_versionInfo_t verInfo = {0};
        sfvmk_ifaceList_t ifaceList;
        unsigned int i;
        int portCount = 0;

        // Trying to retrieve nic list
        if (getNICNameList(&ifaceList) < 0)
        {
            PROVIDER_LOG_ERR("Nic Name list retrieval failed");
        }
        else
        {
            portCount = ifaceList.ifaceCount;
            verInfo.type = SFVMK_GET_DRV_VERSION;

            for (i = 0; i < portCount; i++)
            {
                if (DrvMgmtCall(ifaceList.ifaceArray[i].string, SFVMK_CB_VERINFO_GET,
                                &verInfo) == VMK_OK)
                {
                    return VersionInfo(vmk_NameToString(&verInfo.version));
                }
            }

            PROVIDER_LOG_ERR("%s(): Solarflare Network Device not found", __FUNCTION__);
        }

        // We failed to get it via Driver Management API (possible reason: no
        // Solarflare interfaces are presented) - we try to get it
        // from VMware root/cimv2 standard objects.

        return vmwareGetDriverVersion();
    }
#endif
    class VMwareLibrary : public Library {
        const Package *owner;
        VersionInfo vers;
    public:
        VMwareLibrary(const Package *pkg, const String& d, const String& sn,
                     const VersionInfo& v) :
            Library(d, sn), owner(pkg), vers(v) {}
        virtual VersionInfo version() const { return vers; }
        virtual void initialize() {};
        virtual InstallRC syncInstall(const char *, bool, const char *)
        {
            return Install_Error;
        }
        virtual const String& genericName() const { return description(); }
        virtual const Package *package() const { return owner; }
    };


    /// @brief stub-only implementation of a software package
    /// with kernel drivers
    class VMwareKernelPackage : public Package {
        VMwareDriver kernelDriver;
    protected:
        virtual void setupContents() { kernelDriver.initialize(); };
    public:
        VMwareKernelPackage() :
            Package("NET Driver VIB", "sfc"),
            kernelDriver(this, "NET Driver", "sfc") {}
        virtual PkgType type() const { return VSphereBundle; }
        virtual VersionInfo version() const
        {
            return kernelDriver.version();
        }
        virtual InstallRC syncInstall(const char *, bool, const char *)
        {
            return Install_Error;
        }
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
    class VMwareManagementPackage : public ManagementPackage {
        VMwareLibrary providerLibrary;
    protected:
        virtual void setupContents() { providerLibrary.initialize(); };
    public:
        VMwareManagementPackage() :
            ManagementPackage("CIM Provider VIB", "sfcprovider"),
            providerLibrary(this, "CIM Provider library",
                            PROVIDER_LIBNAME, SF_LIBPROV_VERSION) {}
        virtual PkgType type() const { return VSphereBundle; }
        virtual VersionInfo version() const
        {
            return VersionInfo(SF_LIBPROV_VERSION);
        }
        virtual InstallRC syncInstall(const char *, bool, const char *)
        {
            return Install_Error;
        }
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

    /// Forward declaration
    static int vmwareFillPortAlertsInfo(Array<AlertInfo *> &info,
                                        const Port *port);

    /// @brief stub-only System implementation
    class VMwareSystem : public System {
        VMwareKernelPackage kernelPackage;
        VMwareManagementPackage mgmtPackage;

        bool sfu_dev_initialized;

        VMwareSystem()
        {
            setenv(CIMPLEHOME_ENVVAR, "/tmp/", 1);

            curl_global_init(CURL_GLOBAL_ALL);
            onAlert.setFillPortAlertsInfo(vmwareFillPortAlertsInfo);

            if (sfu_device_init() < 0)
            {
                PROVIDER_LOG_ERR("%s(): sfu_device_init() failed",
                                 __FUNCTION__);
                sfu_dev_initialized = false;
            }
            else
                sfu_dev_initialized = true;

            if (nv_init() < 0)
            {
                PROVIDER_LOG_ERR("%s(): nv_init() failed",
                                  __FUNCTION__);
                sfu_dev_initialized = false;
            }
        };

        ~VMwareSystem()
        {
            unsigned int i;

            curl_global_cleanup();

            for (i = 0; i < NVCtxArr.size(); i++)
            {
                nv_close(NVCtxArr[i].ctx);
                NVCtxArr[i].ctx = NULL;
            }
            NVCtxArr.clear();
        };

        int findSFUDevice(const String &dev_name,
                          sfu_device **devs,
                          sfu_device **dev,
                          int *devs_count);
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
        static VMwareSystem target;
        bool is64bit() const { return true; }
        OSType osType() const { return VMWareESXi; }
        bool forAllNICs(ConstElementEnumerator& en) const;
        bool forAllNICs(ElementEnumerator& en);
        bool forAllPackages(ConstElementEnumerator& en) const;
        bool forAllPackages(ElementEnumerator& en);

        virtual int getRequiredFwImageName(const NIC &nic,
                                           UpdatedFirmwareType type,
                                           unsigned int &img_type,
                                           unsigned int &img_subtype,
                                           String &img_name,
                                           VersionInfo &current_version);
        virtual int getLocalFwImageVersion(const NIC &nic,
                                           UpdatedFirmwareType type,
                                           const char *filename,
                                           bool &applicable,
                                           VersionInfo &imgVersion);
        virtual String createTmpFile();
        virtual int tmpFileBase64Append(const String &fileName,
                                        const String &base64Str);
        virtual int removeTmpFile(String fileName);

        virtual String getSFUDevices();

        virtual bool NVExists(const String &dev_name,
                              unsigned int type, unsigned int subtype,
                              bool try_other_devs,
                              String &correct_dev);
        virtual int NVOpen(const String &dev_name,
                           unsigned int type, unsigned int subtype);
        virtual int NVClose(unsigned int nv_ctx);
        virtual size_t NVPartSize(unsigned int nv_ctx);

        virtual int NVRead(unsigned int nv_ctx,
                           uint64 length,
                           sint64 offset,
                           String &data);
        virtual int NVReadAll(unsigned int nv_ctx,
                              String &data);
        virtual int NVWriteAll(unsigned int nv_ctx,
                               const String &data,
                               bool full_erase);

        virtual int getDriverLoadParameters(String &loadParams);
        virtual int setDriverLoadParameters(const String &loadParams);

#ifndef TARGET_CIM_SERVER_esxi_native
        virtual int MCDIV1Command(const String &dev_name,
                                  unsigned int &cmd,
                                  unsigned int &len,
                                  unsigned int &rc,
                                  String &payload,
                                  int &ioctl_rc,
                                  unsigned int &ioctl_errno);

        virtual int MCDIV2Command(const String &dev_name,
                                  unsigned int &cmd,
                                  unsigned int &inlen,
                                  unsigned int &outlen,
                                  unsigned int &flags,
                                  String &payload,
                                  unsigned int &host_errno,
                                  int &ioctl_rc,
                                  unsigned int &ioctl_errno);

        virtual int MCDIRead(const String &dev_name,
                             uint32 part_type,
                             uint32 len,
                             uint32 offset,
                             String &data);

        virtual int MCDIWrite(const String &dev_name,
                              uint32 part_type,
                              uint32 offset,
                              const String &data);

        virtual int getFuncPrivileges(
                                  const String &dev_name,
                                  const Property<uint32> &pf,
                                  const Property<uint32> &vf,
                                  Property<Array_String> &privilegeNames,
                                  Property<Array_uint32> &privileges) const;


        virtual int modifyFuncPrivileges(
                                  const String &dev_name,
                                  const Property<uint32> &pf,
                                  const Property<uint32> &vf,
                                  const Property<String> &addedMask,
                                  const Property<String> &removedMask) const;
#else
        virtual int getFuncPrivileges(
                                  const Property<String> &PCIAddr,
                                  Property<Array_String> &privilegeNames,
                                  Property<Array_uint32> &privileges) const;


        virtual int modifyFuncPrivileges(
                                  const Property<String> &PCIAddr,
                                  const Property<String> &addedMask,
                                  const Property<String> &removedMask) const;
#endif
    };

    bool VMwareSystem::forAllNICs(ConstElementEnumerator& en) const
    {
        AutoSharedLock auto_shared_lock(nicsLock, false);

        NICDescrs   nics;
        int         i;
        bool        res;
        bool        ret = true;
        int         rc;

        if ((rc = getNICs(nics)) < 0)
            return false;

        for (i = 0; i < (int)nics.size(); i++)
        {
            VMwareNIC  nic(i, nics[i]);

            nic.initialize();
            res = en.process(nic);
            ret = ret && res;
        }

        return ret;
    }

    bool VMwareSystem::forAllNICs(ElementEnumerator& en)
    {
        AutoSharedLock auto_shared_lock(nicsLock, false);

        return forAllNICs((ConstElementEnumerator&) en);
    }

    bool VMwareSystem::forAllPackages(ConstElementEnumerator& en) const
    {
        AutoSharedLock auto_shared_lock(nicsLock, false);

        if (!en.process(kernelPackage))
            return false;
        return en.process(mgmtPackage);
    }
 
    bool VMwareSystem::forAllPackages(ElementEnumerator& en)
    {
        AutoSharedLock auto_shared_lock(nicsLock, false);

        if (!en.process(kernelPackage))
            return false;
        return en.process(mgmtPackage);
    }

    static int getNICFwVersion(const NIC &nic,
                               UpdatedFirmwareType type,
                               VersionInfo &version)
    {
        switch (type)
        {
            case FIRMWARE_BOOTROM:
            {
                VMwareBootROM bootrom(&nic);
                version = bootrom.version();
                break;
            }

            case FIRMWARE_MCFW:
            {
                VMwareNICFirmware firmware(&nic);
                version = firmware.version();
                break;
            }

#ifdef TARGET_CIM_SERVER_esxi_native
            case FIRMWARE_UEFIROM:
            {
                VMwareUEFIROM uefirom(&nic);
                version = uefirom.version();
                break;
            }

            case FIRMWARE_SUCFW:
            {
                VMwareSUCFW sucfw(&nic);
                version = sucfw.version();
                break;
            }
#endif

            default:
                PROVIDER_LOG_ERR("%s(): unknown firmware type %d",
                                 __FUNCTION__, type);
                return -1;
        }

        return 0;
    }

    int VMwareSystem::getRequiredFwImageName(const NIC &nic,
                                             UpdatedFirmwareType type,
                                             unsigned int &img_type,
                                             unsigned int &img_subtype,
                                             String &img_name,
                                             VersionInfo &current_version)
    {
        AutoSharedLock auto_shared_lock(nicsLock, false);

        switch (type)
        {
            case FIRMWARE_BOOTROM:
                img_type = IMAGE_TYPE_BOOTROM;
                break;

            case FIRMWARE_MCFW:
                img_type = IMAGE_TYPE_MCFW;
                break;

#ifdef TARGET_CIM_SERVER_esxi_native
            case FIRMWARE_UEFIROM:
                img_type = IMAGE_TYPE_UEFIROM;
                break;

            case FIRMWARE_SUCFW:
                img_type = IMAGE_TYPE_MUMFW;
                break;
#endif

            default:
                PROVIDER_LOG_ERR("%s(): unknown firmware type %d",
                                 __FUNCTION__, type);
                return -1;
        }

        if (getNICFwVersion(nic, type, current_version) < 0)
            return -1;

        return getFwImageInfo(&nic, type, img_subtype, img_name);
    }

    int VMwareSystem::getLocalFwImageVersion(const NIC &nic,
                                             UpdatedFirmwareType type,
                                             const char *filename,
                                             bool &applicable,
                                             VersionInfo &imgVersion)
    {
        AutoSharedLock auto_shared_lock(nicsLock, false);

        VersionInfo currentVersion;

        if (getNICFwVersion(nic, type, currentVersion) < 0)
            return -1;

        return vmwareGetLocalFwImageVersion(filename, &nic,
                                            type, currentVersion,
                                            applicable, imgVersion);
    }

    String VMwareSystem::createTmpFile()
    {
        Auto_Mutex    guard(tmpFilesArrLock);
        char          tmp_file[] = "/tmp/sf_firmware_XXXXXX";
        int           fd = -1;
        uint64        now;
        unsigned int  i;

        TmpFileDescr fileDescr;

        fd = mkstemp(tmp_file);
        if (fd < 0)
            return String("");
        close(fd);

        now = Time::now();

        fileDescr.fileName = String(tmp_file);
        fileDescr.creationTime = now;
        tmpFilesArr.append(fileDescr);

        return String(tmp_file);
    }

    int VMwareSystem::tmpFileBase64Append(const String &fileName,
                                          const String &base64Str)
    {
        Auto_Mutex    guard(tmpFilesArrLock);
        ssize_t       decSize = base64_dec_size(base64Str.c_str());
        ssize_t       rc;
        char         *data;
        int           fd;

        if (decSize < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to determine decoded data size",
                             __FUNCTION__);
            return -1;
        }

        data = new char[decSize];
        if (data == NULL)
        {
            PROVIDER_LOG_ERR("%s(): failed to allocate memory for "
                             "decoded data",
                             __FUNCTION__);
            return -1;
        }

        if (base64_decode(data, base64Str.c_str()) < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to decode base64 string",
                             __FUNCTION__);
            delete[] data;
            return -1;
        }

        fd = open(fileName.c_str(), O_WRONLY | O_APPEND);
        if (fd < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to open file '%s', "
                             "errno %d ('%s')",
                             __FUNCTION__, fileName.c_str(),
                             errno, strerror(errno));
            delete[] data;
            return -1;
        }

        if ((rc = write(fd, data, decSize)) != decSize)
        {
            PROVIDER_LOG_ERR("%s(): write returned %ld instead of %ld",
                             "errno %d ('%s')",
                             __FUNCTION__, static_cast<long int>(rc),
                             static_cast<long int>(decSize),
                             errno, strerror(errno));
            close(fd);
            delete[] data;
            return -1;
        }

        delete[] data;

        if (close(fd) < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to close a file, "
                             "errno %d ('%s')",
                             __FUNCTION__,
                             errno, strerror(errno));
            return -1;
        }

        return 0;
    }

    int VMwareSystem::removeTmpFile(String fileName)
    {
        Auto_Mutex    guard(tmpFilesArrLock);
        unsigned int  i;

        if (unlink(fileName.c_str()) < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to remove file '%s', "
                             "errno %d ('%s')",
                             __FUNCTION__, fileName.c_str(),
                             errno, strerror(errno));
            return -1;
        }

        for (i = 0; i < tmpFilesArr.size(); i++)
            if (strcmp(fileName.c_str(),
                       tmpFilesArr[i].fileName.c_str()) == 0)
            {
                tmpFilesArr.remove(i);
                i--;
            }

        return 0;
    }

    String VMwareSystem::getSFUDevices()
    {
        AutoSharedLock auto_shared_lock(nicsLock, false);

        sfu_device   *devs;
        int           devs_count;
        unsigned int  i;
        unsigned int  j;

        size_t        enc_size;
        char         *encoded;

        Buffer buf;
        String result;

        if (!sfu_dev_initialized)
        {
            PROVIDER_LOG_ERR("%s(): sfu_device_init() was not called",
                             __FUNCTION__);
            return String("");
        }

        devs_count = sfu_device_enum(&devs);
        if (devs_count < 0)
        {
            PROVIDER_LOG_ERR("%s(): sfu_device_enum() failed",
                             __FUNCTION__);
            return String("");
        }

        buf.format("%d\n", devs_count);
        for (i = 0; i < (unsigned int)devs_count; i++)
        {
            buf.format("%s%s\n", NETIF_NAME_PREF, devs[i].netif_name);
            buf.format("%s%s\n", MAC_ADDR_PREF, devs[i].mac_addr);
            buf.format("%s%s\n", SERIAL_NUM_PREF,
                       devs[i].serial_number);
            buf.format("%s%u\n",
                       PCI_DEVID_PREF,
                       (unsigned int)devs[i].pci_device_id);
            buf.format("%s%u\n",
                       PCI_SUBSYSID_PREF,
                       (unsigned int)devs[i].pci_subsystem_id);
            buf.format("%s%s\n",
                       PCI_ADDR_PREF,
                       devs[i].pci_addr);
            buf.format("%s%u\n", PORT_INDEX_PREF, devs[i].port_index);
#ifndef TARGET_CIM_SERVER_esxi_native
            buf.format("%s%d\n", PHY_TYPE_PREF, devs[i].phy_type);
#endif
            for (j = 0; j < (unsigned int)devs_count; j++)
                if (&(devs[j]) == devs[i].master_port)
                    break;
            buf.format("%s%u\n", MASTER_PORT_PREF, j);
            buf.format("%s\n", NEXT_PREF);
        }

        enc_size = base64_enc_size(strlen(buf.data()));
        encoded = new char[enc_size];
        base64_encode(encoded, buf.data(), strlen(buf.data()));

        result = String(encoded);
        delete[] encoded;
        free(devs);

        return result;
    }

    int VMwareSystem::findSFUDevice(const String &dev_name,
                                    sfu_device **devs,
                                    sfu_device **dev,
                                    int *devs_count)
    {
        AutoSharedLock auto_shared_lock(nicsLock, false);

        unsigned int i;

        if (!sfu_dev_initialized)
        {
            PROVIDER_LOG_ERR("%s(): sfu_device_init() was not called",
                             __FUNCTION__);
            return -1;
        }

        *devs_count = sfu_device_enum(devs);
        if (*devs_count < 0)
        {
            PROVIDER_LOG_ERR("%s(): sfu_device_enum() failed",
                             __FUNCTION__);
            return -1;
        }

        for (i = 0; i < (unsigned int)*devs_count; i++)
        {
            if (strcmp((*devs)[i].netif_name, dev_name.c_str()) == 0)
            {
                *dev = &((*devs)[i]);
                break;
            }
        }

        if (*dev == NULL)
        {
            PROVIDER_LOG_ERR("%s(): failed to find device '%s'",
                             __FUNCTION__, dev_name.c_str());
            free(*devs);
            return -1;
        }

        return 0;
    }

    bool VMwareSystem::NVExists(const String &dev_name,
                                unsigned int type,
                                unsigned int subtype,
                                bool try_other_devs,
                                String &correct_dev)
    {
        AutoSharedLock auto_shared_lock(nicsLock, false);

        sfu_device         *devs;
        sfu_device         *dev = NULL;
        const sfu_device   *dev2 = NULL;

        int           devs_count;
        bool          exists = false;

        if (findSFUDevice(dev_name, &devs, &dev,
                          &devs_count) < 0)
            return -1;

        exists = nv_exists(dev, static_cast<enum nv_part_type>(type),
                           subtype, (try_other_devs ? &dev2 : NULL));
        if (try_other_devs && exists && dev2 != NULL)
            correct_dev = String(dev2->netif_name);
        free(devs);

        return exists;
    }

    int VMwareSystem::NVOpen(const String &dev_name,
                             unsigned int type,
                             unsigned int subtype)
    {
        Auto_Mutex    guard(NVCtxArrLock);

        AutoSharedLock auto_shared_lock(nicsLock, false);

        nv_context   *ctx = NULL;

        sfu_device   *devs;
        sfu_device   *dev = NULL;
        int           devs_count;

        unsigned int  i;
        unsigned int  new_id;

        NVContextDescr ctx_descr;

        for (new_id = 0; new_id < MAX_NV_CTX_ID; new_id++)
        {
            for (i = 0; i < NVCtxArr.size(); i++)
            {
                if (NVCtxArr[i].id == new_id)
                    break;
            }
            if (i == NVCtxArr.size())
                break;
        }
        if (new_id == MAX_NV_CTX_ID)
        {
            PROVIDER_LOG_ERR("%s(): failed to find free context id",
                             __FUNCTION__);
            return -1;
        }

        if (findSFUDevice(dev_name, &devs, &dev,
                          &devs_count) < 0)
            return -1;

        ctx = nv_open(dev, static_cast<enum nv_part_type>(type), subtype);
        if (ctx == NULL)
        {
            PROVIDER_LOG_ERR("nv_open() failed, errno %d('%s')",
                             errno, strerror(errno));
            free(devs);
            return -1;
        }
        free(devs);

        ctx_descr.ctx = ctx;
        ctx_descr.id = new_id;

        NVCtxArr.append(ctx_descr);

        return new_id;
    }

    int VMwareSystem::NVClose(unsigned int nv_ctx)
    {
        Auto_Mutex    guard(NVCtxArrLock);

        AutoSharedLock auto_shared_lock(nicsLock, false);

        int i;

        i = findNVCtxById(nv_ctx);

        if (i < 0)
            return -1;
        else
        {
            nv_close(NVCtxArr[i].ctx);
            NVCtxArr.remove(i);
        }

        return 0;
    }

    size_t VMwareSystem::NVPartSize(unsigned int nv_ctx)
    {
        Auto_Mutex    guard(NVCtxArrLock);

        AutoSharedLock auto_shared_lock(nicsLock, false);

        int i;

        i = findNVCtxById(nv_ctx);

        if (i >= 0)
            return nv_part_size(NVCtxArr[i].ctx);

        return 0;
    }

    static int
    performNVRead(bool read_all,
                  unsigned int nv_ctx,
                  uint64 length,
                  sint64 offset,
                  String &data)
    {
        int       i;
        ssize_t   was_read;
        char     *buf;
        size_t    enc_size;
        char     *encoded_buf;
        size_t    part_size;

        i = findNVCtxById(nv_ctx);
        if (i < 0)
            return -1;

        if (read_all)
        {
            part_size = nv_part_size(NVCtxArr[i].ctx);
            buf = new char[part_size];
        }
        else
            buf = new char[length];

        if (read_all)
        {
            was_read = nv_read_all(NVCtxArr[i].ctx, buf);
            if (was_read < 0)
            {
                PROVIDER_LOG_ERR("nv_read_all() failed, errno %d('%s')",
                                 errno, strerror(errno));
                delete[] buf;
                return -1;
            }
            else
                was_read = part_size;
        }
        else
        {
            was_read = nv_read(NVCtxArr[i].ctx, buf,
                               static_cast<size_t>(length),
                               static_cast<off_t>(offset));
            if (was_read < 0)
            {
                PROVIDER_LOG_ERR("nv_read() failed, errno %d('%s')",
                                 errno, strerror(errno));
                delete[] buf;
                return -1;
            }
        }

        enc_size = base64_enc_size(was_read);
        encoded_buf = new char[enc_size];
        base64_encode(encoded_buf, buf, was_read);

        data = String(encoded_buf);
        delete[] encoded_buf;
        delete[] buf;

        return 0;
    }

    int VMwareSystem::NVRead(unsigned int nv_ctx,
                             uint64 length,
                             sint64 offset,
                             String &data)
    {
        Auto_Mutex    guard(NVCtxArrLock);

        AutoSharedLock auto_shared_lock(nicsLock, false);

        return performNVRead(false, nv_ctx, length, offset,
                             data);
    }

    int VMwareSystem::NVReadAll(unsigned int nv_ctx,
                                String &data)
    {
        Auto_Mutex    guard(NVCtxArrLock);

        AutoSharedLock auto_shared_lock(nicsLock, false);

        return performNVRead(true, nv_ctx, 0, 0, data);
    }

    int VMwareSystem::NVWriteAll(unsigned int nv_ctx,
                                 const String &data,
                                 bool full_erase)
    {
        Auto_Mutex    guard(NVCtxArrLock);

        AutoSharedLock auto_shared_lock(nicsLock, true);

        int       i;
        char     *buf;
        ssize_t   dec_size;
        int       rc;

        i = findNVCtxById(nv_ctx);
        if (i < 0)
            return -1;

        dec_size = base64_dec_size(data.c_str());
        if (dec_size < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to determine decoded "
                             "buffer size", __FUNCTION__);
            return -1;
        }

        buf = new char[dec_size];
        if (base64_decode(buf, data.c_str()) < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to decode data",
                             __FUNCTION__);
            delete[] buf;
            return -1;
        }

        rc = nv_write_all(NVCtxArr[i].ctx, buf, dec_size, full_erase,
                          NULL);
        if (rc < 0)
        {
            PROVIDER_LOG_ERR("nv_write_all() failed, errno %d('%s')",
                             errno, strerror(errno));
            delete[] buf;
            return -1;
        }

        delete[] buf;
        return 0;
    }

#ifndef TARGET_CIM_SERVER_esxi_native
    int VMwareSystem::MCDIRead(const String &dev_name,
                               uint32 part_type,
                               uint32 len,
                               uint32 offset,
                               String &data)
    {
        int       rc = 0;
        int       fd = -1;
        char     *buf = NULL;
        size_t    enc_size;
        char     *encoded_buf = NULL;

        AutoSharedLock auto_shared_lock(nicsLock, false);

        fd = open(DEV_SFC_CONTROL, O_RDWR);
        if (fd < 0)
        {
            PROVIDER_LOG_ERR("Failed to open '%s', errno %d ('%s')",
                             DEV_SFC_CONTROL, errno,
                             strerror(errno));
            rc = -1;
            goto cleanup;
        }

        buf = new char[len];
        rc = siocEFXReadNVRAM(fd, false, (uint8_t *)buf,
                              offset, len, dev_name.c_str(),
                              part_type);
        if (rc < 0)
            goto cleanup;

        enc_size = base64_enc_size(len);
        encoded_buf = new char[enc_size];
        base64_encode(encoded_buf, buf, len);

        data = String(encoded_buf);
cleanup:
        if (fd >= 0)
            close(fd);
        delete[] buf;
        delete[] encoded_buf;
        return rc;
    }

    int VMwareSystem::MCDIWrite(const String &dev_name,
                                uint32 part_type,
                                uint32 offset,
                                const String &data)
    {
        int       rc;
        int       fd;
        char     *decoded = NULL;
        ssize_t   dec_size = 0;

        AutoSharedLock auto_shared_lock(nicsLock, true);

        dec_size = base64_dec_size(data.c_str());
        if (dec_size < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to determine decoded "
                             "payload size", __FUNCTION__);
            return -1;
        }

        decoded = new char[dec_size];
        if (base64_decode(decoded, data.c_str()) < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to decode "
                             "payload", __FUNCTION__);
            delete[] decoded;
            return -1;
        }

        fd = open(DEV_SFC_CONTROL, O_RDWR);
        if (fd < 0)
        {
            PROVIDER_LOG_ERR("Failed to open '%s', errno %d ('%s')",
                             DEV_SFC_CONTROL, errno,
                             strerror(errno));
            delete[] decoded;
            return -1;
        }

        rc = siocEFXWriteNVRAM(fd, false, (uint8_t *)decoded,
                               offset, dec_size, dev_name.c_str(),
                               part_type);

        close(fd);
        delete[] decoded;
        return rc;
    }

    int VMwareSystem::MCDIV1Command(const String &dev_name,
                                    unsigned int &cmd,
                                    unsigned int &len,
                                    unsigned int &rc,
                                    String &payload,
                                    int &ioctl_rc,
                                    unsigned int &ioctl_errno)
    {
        struct efx_mcdi_request *mcdi_req;
        struct efx_ioctl         ioc;

        int      fd;
        ssize_t  dec_size;
        size_t   enc_size;
        char    *encoded;

        AutoSharedLock auto_shared_lock(nicsLock, true);

        memset(&ioc, 0, sizeof(ioc));
        strncpy(ioc.if_name, dev_name.c_str(), sizeof(ioc.if_name));
        ioc.cmd = EFX_MCDI_REQUEST;

        mcdi_req = &ioc.u.mcdi_request;

        mcdi_req->cmd = cmd;
        mcdi_req->len = len;
        mcdi_req->rc = rc;

        dec_size = base64_dec_size(payload.c_str());
        if (dec_size < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to determine decoded "
                             "payload size", __FUNCTION__);
            return -1;
        }
        if ((unsigned int)dec_size > sizeof(mcdi_req->payload))
        {
            PROVIDER_LOG_ERR("%s(): payload is too big, %u bytes",
                             __FUNCTION__, (unsigned int)dec_size);
            return -1;
        }
        if (base64_decode((char *)mcdi_req->payload, payload.c_str()) < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to decode "
                             "payload", __FUNCTION__);
            return -1;

        }

        fd = open(DEV_SFC_CONTROL, O_RDWR);
        if (fd < 0)
        {
            PROVIDER_LOG_ERR("Failed to open '%s', errno %d ('%s')",
                             DEV_SFC_CONTROL, errno,
                             strerror(errno));
            return -1;
        }

        ioctl_rc = ioctl(fd, SIOCEFX, &ioc);
        ioctl_errno = errno;
        close(fd);

        cmd = mcdi_req->cmd;
        len = mcdi_req->len;
        rc = mcdi_req->rc;

        if (rc == 0)
        {
            enc_size = base64_enc_size(sizeof(mcdi_req->payload));
            encoded = new char[enc_size];
            base64_encode(encoded, (char *)mcdi_req->payload,
                          sizeof(mcdi_req->payload));
            payload = String(encoded);
            delete[] encoded;
        }

        return 0;
    }

    int VMwareSystem::MCDIV2Command(const String &dev_name,
                                    unsigned int &cmd,
                                    unsigned int &inlen,
                                    unsigned int &outlen,
                                    unsigned int &flags,
                                    String &payload,
                                    unsigned int &host_errno,
                                    int &ioctl_rc,
                                    unsigned int &ioctl_errno)
    {
        struct efx_mcdi_request2 *mcdi_req2 = NULL;
        struct efx_ioctl          ioc;

        int      fd = -1;
        int      rc = 0;
        ssize_t  dec_size;
        size_t   enc_size;
        char    *encoded = NULL;

        AutoSharedLock auto_shared_lock(nicsLock, true);

        dec_size = base64_dec_size(payload.c_str());
        if (dec_size < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to determine decoded "
                             "payload size", __FUNCTION__);
            rc = -1;
            goto cleanup;
        }

        memset(&ioc, 0, sizeof(ioc));
        strncpy(ioc.if_name, dev_name.c_str(), sizeof(ioc.if_name));
        ioc.cmd = EFX_MCDI_REQUEST2;

        mcdi_req2 = &ioc.u.mcdi_request2;
        mcdi_req2->cmd = cmd;
        mcdi_req2->inlen = inlen;
        mcdi_req2->outlen = outlen;
        mcdi_req2->flags = flags;
        mcdi_req2->host_errno = host_errno;

        if ((unsigned int)dec_size > sizeof(mcdi_req2->payload))
        {
            PROVIDER_LOG_ERR("%s(): payload is too big, %u bytes",
                             __FUNCTION__, (unsigned int)dec_size);
            rc = -1;
            goto cleanup;
        }
        if (base64_decode((char *)mcdi_req2->payload, payload.c_str()) < 0)
        {
            PROVIDER_LOG_ERR("%s(): failed to decode "
                             "payload", __FUNCTION__);
            rc = -1;
            goto cleanup;
        }

        fd = open(DEV_SFC_CONTROL, O_RDWR);
        if (fd < 0)
        {
            PROVIDER_LOG_ERR("Failed to open '%s', errno %d ('%s')",
                             DEV_SFC_CONTROL, errno,
                             strerror(errno));
            rc = -1;
            goto cleanup;
        }

        ioctl_rc = ioctl(fd, SIOCEFX, &ioc);
        ioctl_errno = errno;
        rc = ioctl_rc;

        if (mcdi_req2->outlen <= outlen)
        {
            enc_size = base64_enc_size(sizeof(mcdi_req2->outlen));
            encoded = new char[enc_size];
            base64_encode(encoded, (char *)mcdi_req2->payload,
                          mcdi_req2->outlen);
            payload = String(encoded);
        }

        cmd = mcdi_req2->cmd;
        inlen = mcdi_req2->inlen;
        outlen = mcdi_req2->outlen;
        flags = mcdi_req2->flags;
        host_errno = mcdi_req2->host_errno;

cleanup:
        if (fd >= 0)
            close(fd);
        if (encoded != NULL)
            delete[] encoded;
        return rc;
    }
#endif
#ifndef TARGET_CIM_SERVER_esxi_native
    int VMwareSystem::getFuncPrivileges(
                                  const String &dev_name,
                                  const Property<uint32> &pf,
                                  const Property<uint32> &vf,
                                  Property<Array_String> &privilegeNames,
                                  Property<Array_uint32> &privileges) const
    {
        return doGetPrivileges(dev_name.c_str(), pf, vf,
                               privilegeNames, privileges);
    }

    int VMwareSystem::modifyFuncPrivileges(
                              const String &dev_name,
                              const Property<uint32> &pf,
                              const Property<uint32> &vf,
                              const Property<String> &addedMask,
                              const Property<String> &removedMask) const
    {
        return doModifyPrivileges(dev_name.c_str(),
                                  pf, vf, addedMask, removedMask);
    }
#else
    int VMwareSystem::getFuncPrivileges(
                                  const Property<String> &PCIAddr,
                                  Property<Array_String> &privilegeNames,
                                  Property<Array_uint32> &privileges) const
    {
        return doGetPrivileges(PCIAddr, privilegeNames, privileges);
    }

    int VMwareSystem::modifyFuncPrivileges(
                              const Property<String> &PCIAddr,
                              const Property<String> &addedMask,
                              const Property<String> &removedMask) const
    {
        return doModifyPrivileges(PCIAddr, addedMask, removedMask);
    }
#endif
    static Ref<VMware_KernelModuleService> getKernelModuleService()
    {
        Ref<VMware_KernelModuleService>
                    cimModel = VMware_KernelModuleService::create();

        Ref<Instance>                   cimInstance;
        Ref<VMware_KernelModuleService> km_svc;

        cimple::Instance_Enumerator ie;

        if (cimple::cimom::enum_instances(CIMHelper::baseNS,
                                          cimModel.ptr(), ie) != 0)
        {
            PROVIDER_LOG_ERR("Failed to enumerate "
                             "VMware_KernelModuleService");
            return km_svc;
        }

        cimInstance = ie();
        if (!cimInstance)
        {
            PROVIDER_LOG_ERR("Failed to get instance "
                             "of VMware_KernelModuleService");
            return km_svc;
        }

        km_svc.reset(cast<VMware_KernelModuleService *>(cimInstance.ptr()));

        return km_svc;
    }

    int VMwareSystem::getDriverLoadParameters(String &loadParams)
    {
        Ref<VMware_KernelModuleService> km_svc = getKernelModuleService();

        // cimple::Ref does not compile with methods
        VMware_KernelModuleService_GetModuleLoadParameter_method *
            method = NULL;

        if (!km_svc)
            return -1;

        method = VMware_KernelModuleService_GetModuleLoadParameter_method::create();
        if (!method)
            return -1;

        method->ModuleName.set(VMK_DRIVER_NAME);
        if (cimple::cimom::invoke_method(CIMHelper::baseNS,
                                         km_svc.ptr(), method) != 0)
        {
            PROVIDER_LOG_ERR("Failed to invoke "
                             "GetModuleLoadParameter() method");
            VMware_KernelModuleService_GetModuleLoadParameter_method::destroy(method);
            return -1;
        }
        else if (method->return_value.value != 0)
        {
            PROVIDER_LOG_ERR("GetModuleLoadParameter() method returned %u",
                             (unsigned int)method->return_value.value);
            VMware_KernelModuleService_GetModuleLoadParameter_method::destroy(method);
            return -1;
        }

        loadParams = method->LoadParameter.value;

        VMware_KernelModuleService_GetModuleLoadParameter_method::destroy(method);

        return 0;
    }

    int VMwareSystem::setDriverLoadParameters(const String &loadParams)
    {
        Ref<VMware_KernelModuleService> km_svc = getKernelModuleService();

        // cimple::Ref does not compile with methods
        VMware_KernelModuleService_SetModuleLoadParameter_method *
            method = NULL;

        if (!km_svc)
            return -1;

        method = VMware_KernelModuleService_SetModuleLoadParameter_method::create();
        if (!method)
            return -1;

        method->ModuleName.set(VMK_DRIVER_NAME);
        method->LoadParameter.set(loadParams);
        if (cimple::cimom::invoke_method(CIMHelper::baseNS,
                                         km_svc.ptr(), method) != 0)
        {
            PROVIDER_LOG_ERR("Failed to invoke "
                             "SetModuleLoadParameter() method");
            VMware_KernelModuleService_SetModuleLoadParameter_method::destroy(method);
            return -1;
        }
        else if (method->return_value.value != 0)
        {
            PROVIDER_LOG_ERR("SetModuleLoadParameter() method returned %u",
                             (unsigned int)method->return_value.value);
            VMware_KernelModuleService_SetModuleLoadParameter_method::destroy(method);
            return -1;
        }

        VMware_KernelModuleService_SetModuleLoadParameter_method::destroy(method);

        return 0;
    }

    VMwareSystem VMwareSystem::target;

    System& System::target = VMwareSystem::target;

#ifndef TARGET_CIM_SERVER_esxi_native
    VersionInfo VMwareNICFirmware::version() const
    {
        struct ethtool_drvinfo edata;

        if (((VMwareNIC *)owner)->ports.size() <= 0)
            return VersionInfo(DEFAULT_VERSION_STR);

        if (vmwareEthtoolCmd(((VMwareNIC *)owner)->
                                        ports[0].dev_file.c_str(),
                             ((VMwareNIC *)owner)->
                                        ports[0].dev_name.c_str(),
                             ETHTOOL_GDRVINFO, &edata) < 0)
            return VersionInfo(DEFAULT_VERSION_STR);

        return VersionInfo(edata.fw_version);
    }
#else
    VersionInfo VMwareNICFirmware::version() const
    {
        sfvmk_versionInfo_t verInfo = {0};
        const char *devName;

        if ((((VMwareNIC *)owner)->ports.size() <= 0))
            return VersionInfo(DEFAULT_VERSION_STR);

        devName = ((VMwareNIC *)owner)->ports[0].dev_name.c_str();

        verInfo.type = SFVMK_GET_FW_VERSION;

        if (DrvMgmtCall(devName, SFVMK_CB_VERINFO_GET, &verInfo) == VMK_OK)
            return VersionInfo(vmk_NameToString(&verInfo.version));

        return VersionInfo(DEFAULT_VERSION_STR);
    }
#endif

#ifndef TARGET_CIM_SERVER_esxi_native
    VersionInfo VMwareBootROM::version() const
    {
        VersionInfo ver;
        int         fd;
        uint32_t    pci_device_id;
        const char *ifname;

        const VMwareNIC  *nic = reinterpret_cast<const VMwareNIC *>(owner);

        if (nic == NULL || nic->ports.size() < 1)
            return VersionInfo(DEFAULT_VERSION_STR);

        ifname = nic->ports[0].dev_name.c_str();

        fd = open(DEV_SFC_CONTROL, O_RDWR);
        if (fd < 0)
        {
            PROVIDER_LOG_ERR("Failed to open '%s', errno %d('%s')",
                             DEV_SFC_CONTROL, errno, strerror(errno));
            return VersionInfo(DEFAULT_VERSION_STR);
        }

        pci_device_id = nic->getPCIDeviceID();

        if (SFU_DEVICE_TYPE(pci_device_id) == SFU_DEVICE_TYPE_SIENA)
        {
            if (siocEFXGetBootROMVersionSiena(ifname, 0, fd,
                                              false, ver) == 0)
            {
                close(fd);
                return ver;
            }
        }
        else if (SFU_DEVICE_TYPE(pci_device_id) ==
                                          SFU_DEVICE_TYPE_HUNTINGTON)
        {
            if (siocEFXGetBootROMVersionEF10(ifname, 0, fd,
                                             false, ver) == 0)
            {
                close(fd);
                return ver;
            }
        }

        close(fd);
        return VersionInfo(DEFAULT_VERSION_STR);
    }
#else
    VersionInfo VMwareBootROM::version() const
    {
        sfvmk_versionInfo_t verInfo = {0};
        const char *devName;

        const VMwareNIC  *nic = reinterpret_cast<const VMwareNIC *>(owner);

        if (nic == NULL || nic->ports.size() < 1)
            return VersionInfo(DEFAULT_VERSION_STR);

        devName = nic->ports[0].dev_name.c_str();

        verInfo.type = SFVMK_GET_ROM_VERSION;

        if (DrvMgmtCall(devName, SFVMK_CB_VERINFO_GET, &verInfo) == VMK_OK)
            return VersionInfo(vmk_NameToString(&verInfo.version));

        return VersionInfo(DEFAULT_VERSION_STR);
    }

    VersionInfo VMwareUEFIROM::version() const
    {
        sfvmk_versionInfo_t verInfo = {0};
        const char *devName;

        const VMwareNIC  *nic = reinterpret_cast<const VMwareNIC *>(owner);

        if (nic == NULL || nic->ports.size() < 1)
            return VersionInfo(DEFAULT_VERSION_STR);

        devName = nic->ports[0].dev_name.c_str();

        verInfo.type = SFVMK_GET_UEFI_VERSION;

        if (DrvMgmtCall(devName, SFVMK_CB_VERINFO_GET, &verInfo) == VMK_OK)
            return VersionInfo(vmk_NameToString(&verInfo.version));

        return VersionInfo(DEFAULT_VERSION_STR);
    }

    VersionInfo VMwareSUCFW::version() const
    {
        sfvmk_versionInfo_t verInfo = {0};
        const char *devName;

        const VMwareNIC  *nic = reinterpret_cast<const VMwareNIC *>(owner);

        if (nic == NULL || nic->ports.size() < 1)
            return VersionInfo(DEFAULT_VERSION_STR);

        devName = nic->ports[0].dev_name.c_str();

        verInfo.type = SFVMK_GET_SUC_VERSION;

        if (DrvMgmtCall(devName, SFVMK_CB_VERINFO_GET, &verInfo) == VMK_OK)
            return VersionInfo(vmk_NameToString(&verInfo.version));

        return VersionInfo(DEFAULT_VERSION_STR);
    }
#endif

    ///
    /// Class defining link state alert indication to be
    /// generated on VMware
    ///
    class VMwareLinkStateAlertInfo : public LinkStateAlertInfo {
        String devFile;     ///< Port device file
        String devName;     ///< Port device name

    protected:

        virtual int updateLinkState()
        {
            curLinkState = getLinkStatus(devFile, devName);
            return 0;
        }

    public:

        friend int vmwareFillPortAlertsInfo(Array<AlertInfo *> &info,
                                            const Port *port);
    };

    ///
    /// Class defining sensors alert indications to be
    /// generated on VMware
    ///
    class VMwareSensorsAlertInfo : public SensorsAlertInfo {
        String devName;    ///< Port device name
        int    fd;         ///< /dev/sfc_control fd

    protected:

        virtual int updateSensors()
        {
            if (fd < 0)
                return -1;
            if (mcdiGetSensors(sensorsCur, fd, false, devName) != 0)
                return -1;
            return 0;
        }

    public:
        VMwareSensorsAlertInfo()
        {
            fd = open(DEV_SFC_CONTROL, O_RDWR);
            if (fd < 0)
                PROVIDER_LOG_ERR("Failed to open %s device for checking "
                                 "NIC sensors, errno %d ('%s')",
                                 DEV_SFC_CONTROL, errno, strerror(errno));
        }
        virtual ~VMwareSensorsAlertInfo()
        {
            if (fd >= 0)
                close(fd);
        }

        friend int vmwareFillPortAlertsInfo(Array<AlertInfo *> &info,
                                            const Port *port);
    };

    ///
    /// Fill array of alert indication descriptions.
    ///
    /// @param info   [out] Array to be filled
    /// @param port         Reference to port class instance
    ///
    /// @return -1 on failure, 0 on success.
    ///
    static int vmwareFillPortAlertsInfo(Array<AlertInfo *> &info,
                                        const Port *port)
    {
        VMwareLinkStateAlertInfo *linkStateInstInfo = NULL;
        VMwareSensorsAlertInfo   *sensorsInstInfo = NULL;

        const VMwarePort *vmwarePort =
                      dynamic_cast<const VMwarePort *>(port);

        linkStateInstInfo = new VMwareLinkStateAlertInfo();
        linkStateInstInfo->devFile = vmwarePort->dev_file;
        linkStateInstInfo->devName = vmwarePort->dev_name;
        info.append(linkStateInstInfo);

        sensorsInstInfo = new VMwareSensorsAlertInfo();
        sensorsInstInfo->devName = vmwarePort->dev_name;
        sensorsInstInfo->portFn = vmwarePort->pciAddress().fn();
        info.append(sensorsInstInfo);

        return 0;
    }
}
