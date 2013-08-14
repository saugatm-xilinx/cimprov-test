#include "sf_platform.h"
#include "sf_provider.h"
#include "sf_logging.h"
#include <cimple/Buffer.h>
#include <cimple/Strings.h>
#include <cimple/Array.h>
#include <cimple/log.h>
#include <cimple.h>

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
#include <errno.h>

#include <linux/if.h>
#include <linux/if_arp.h>

// SLES 10 has broken ethtool.h
#ifdef SLES10_BUILD_HOST
#define u32 __u32
#define u16 __u16
#define u8 __u8
#define u64 __u64
#endif
#include <linux/ethtool.h>
#ifdef SLES10_BUILD_HOST
#undef u8
#undef u16
#undef u32
#undef u64
#endif

#include <linux/sockios.h>

#include "sf_mcdi_sensors.h"

#define SYS_PCI_DEVICE_PATH             "/sys/bus/pci/devices"
#define SYS_SF_MOD_PATH                 "/sys/module/sfc"
#define SYS_SF_DRV_PATH                 SYS_SF_MOD_PATH "/drivers/pci:sfc"
#define CLASS_NET_VALUE                 "0x020000"
#define VENDOR_SF_VALUE                 "0x1924"
#define SYS_PATH_MAX_LEN                1024
#define BUF_MAX_LEN                     256

#define BOOT_ROM_VERSION_MAX_OFFSET     0x600
#define BOOT_ROM_VERSION_PREFIX         "Solarflare Boot Manager (v"

#define VPD_TAG_ID                      0x82
#define VPD_TAG_R                       0x90
#define VPD_TAG_W                       0x91
#define VPD_TAG_END                     0x78

#define LINUX_LOG_ERR(_fmt, _args...)   Logger::errorLog.format(_fmt, ##_args)
#define LINUX_LOG_EVT(_fmt, _args...)   Logger::eventLog.format(_fmt, ##_args)
#define LINUX_LOG_DBG(_fmt, _args...)   Logger::debugLog.format(_fmt, ##_args)

namespace solarflare
{
    ///
    /// Get device attribute from sysfs file.
    ///
    /// @param dev_path         Path to device node in sysfs tree
    /// @param attr_name        Attribute name
    /// @param buf              Location for value
    /// @param maxlen           Buffer size
    ///
    /// @return size of value on success, -1 on error
    ///
    int linuxDeviceGetAttr(const char *dev_path, const char *attr_name,
                           char *buf, int maxlen)
    {
        char path[SYS_PATH_MAX_LEN];
        struct stat statbuf;
        int fd;
        ssize_t size;

        sprintf(path, "%s/%s", dev_path, attr_name);
        if (lstat(path, &statbuf) != 0)
            return -1;

        if ((statbuf.st_mode & S_IRUSR) == 0)
            return -1;

        fd = open(path, O_RDONLY);
        if (fd < 0)
        {
            LINUX_LOG_ERR("Failed to open file %s: %s",
                            path, strerror(errno));
            return -1;
        }
        size = read(fd, buf, maxlen);
        close(fd);
        if (size < 0)
        {
            LINUX_LOG_ERR("Failed to read file %s: %s",
                            path, strerror(errno));
            return -1;
        }
        if (size == 0 || size == maxlen)
            return -1;
        if (buf[size - 1] == '\n')
            buf[size - 1] = '\0';
        else
            buf[size] = '\0';
        return size;
    }

    ///
    /// Check that given device is Solarflare network device.
    ///
    /// @param path     Path to device node in sysfs tree
    ///
    /// @return true on success, false on error
    ///
    bool linuxDeviceCheckPath(const char *path)
    {
        char value[BUF_MAX_LEN];

        if (linuxDeviceGetAttr(path, "class", value, sizeof(value)) < 0)
            return false;

        if (0 != strcmp(value, CLASS_NET_VALUE))
            return false;

        if (linuxDeviceGetAttr(path, "vendor", value, sizeof(value)) < 0)
            return false;

        if (0 != strcmp(value, VENDOR_SF_VALUE))
            return false;

        return true;
    }

    ///
    /// Perform ethool command.
    ///
    /// @param ifname            Interface name
    /// @param cmd               Ethtool command
    /// @param edata             Location for get, data for set method
    ///
    /// @return zero on success, -1 on error
    ///
    int linuxEthtoolCmd(const char *ifname, unsigned cmd, void *edata)
    {
        int sock;
        struct ifreq ifr;

        memset(&ifr, 0, sizeof(ifr));
        sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock < 0)
        {
            LINUX_LOG_ERR("Failed to create PF_INET socket: %s",
                             strerror(errno));
            return -1;
        }

        strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
        ifr.ifr_data = edata;
        ((struct ethtool_value *)edata)->cmd = cmd;
        if (ioctl(sock, SIOCETHTOOL, &ifr) < 0)
        {
            LINUX_LOG_ERR("Failed to perform SIOCETHTOOL ioctl with "
                            "cmd value <%u>: %s", cmd, strerror(errno));
            close(sock);
            return -1;
        }

        close(sock);
        return 0;
    }

    ///
    /// Perform ioctl command on interface.
    ///
    /// @param ifname            Interface name
    /// @param cmd               Ethtool command
    /// @param ifr               Location for IO data
    ///
    /// @return zero on success, -1 on error
    ///
    int linuxIOctlCmd(const char *ifname, unsigned cmd, struct ifreq *ifr)
    {
        int sock;

        sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock < 0)
            return -1;

        strncpy(ifr->ifr_name, ifname, sizeof(ifr->ifr_name));
        if (ioctl(sock, cmd, ifr) < 0)
        {
            close(sock);
            return -1;
        }

        close(sock);
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
    static bool getLinkStatus(const String &ifName)
    {
        struct ethtool_value edata;

        memset(&edata, 0, sizeof(edata));
        if (linuxEthtoolCmd(ifName.c_str(),
                            ETHTOOL_GLINK, &edata) < 0)
            return false;

        return edata.data == 1 ? true : false;
    }

    /// Forward declaration
    static int linuxFillPortAlertsInfo(Array<AlertInfo *> &info,
                                       const Port *port);

    class LinuxPort : public Port {
        const NIC *owner;
        Interface *boundIface;
        friend int linuxFillPortAlertsInfo(Array<AlertInfo *> &info,
                                           const Port *port);
    public:
        LinuxPort(const NIC *up, unsigned i) : Port(i), owner(up) {}

        virtual bool linkStatus() const;
        virtual Speed linkSpeed() const;
        virtual void linkSpeed(Speed sp);

        virtual bool fullDuplex() const;
        virtual void fullDuplex(bool fd);
        virtual bool autoneg() const;

        virtual void autoneg(bool an);
        virtual void renegotiate();

        virtual MACAddress permanentMAC() const;

        virtual const NIC *nic() const { return owner; }
        virtual PCIAddress pciAddress() const
        {
            return owner->pciAddress().fn(elementId());
        }

        virtual void initialize() {};

        void bindToIface(Interface *iface) { boundIface = iface; }
    };

    bool LinuxPort::linkStatus() const
    {
        struct ethtool_value edata;

        if (!boundIface)
            return false;

        return getLinkStatus(boundIface->ifName());
    }

    Port::Speed LinuxPort::linkSpeed() const
    {
        struct ethtool_cmd edata;

        if (!boundIface)
            return Speed(Port::SpeedUnknown);

        memset(&edata, 0, sizeof(edata));
        if (linuxEthtoolCmd(boundIface->ifName().c_str(),
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

    void LinuxPort::linkSpeed(Port::Speed sp)
    {
        struct ethtool_cmd edata;

        if (!boundIface)
            return;

        memset(&edata, 0, sizeof(edata));
        if (linuxEthtoolCmd(boundIface->ifName().c_str(),
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

        linuxEthtoolCmd(boundIface->ifName().c_str(),
                        ETHTOOL_SSET, &edata);
    }

    bool LinuxPort::fullDuplex() const
    {
        struct ethtool_cmd edata;

        if (!boundIface)
            return false;

        memset(&edata, 0, sizeof(edata));
        if (linuxEthtoolCmd(boundIface->ifName().c_str(),
                            ETHTOOL_GSET, &edata) < 0)
            return false;

        if (edata.duplex == DUPLEX_HALF)
            return false;

        return true;
    }

    void LinuxPort::fullDuplex(bool fd)
    {
        struct ethtool_cmd edata;

        if (!boundIface)
            return;

        memset(&edata, 0, sizeof(edata));
        if (linuxEthtoolCmd(boundIface->ifName().c_str(),
                            ETHTOOL_GSET, &edata) < 0)
            return;

        if (fd)
            edata.duplex = DUPLEX_FULL;
        else
            edata.duplex = DUPLEX_HALF;

        linuxEthtoolCmd(boundIface->ifName().c_str(),
                        ETHTOOL_SSET, &edata);
    }

    bool LinuxPort::autoneg() const
    {
        struct ethtool_cmd edata;

        if (!boundIface)
            return false;

        memset(&edata, 0, sizeof(edata));
        if (linuxEthtoolCmd(boundIface->ifName().c_str(),
                            ETHTOOL_GSET, &edata) < 0)
            return false;

        if (edata.autoneg == AUTONEG_DISABLE)
            return false;

        return true;
    }

    void LinuxPort::autoneg(bool an)
    {
        struct ethtool_cmd edata;

        if (!boundIface)
            return;

        memset(&edata, 0, sizeof(edata));
        if (linuxEthtoolCmd(boundIface->ifName().c_str(),
                            ETHTOOL_GSET, &edata) < 0)
            return;

        if (an)
            edata.autoneg = AUTONEG_ENABLE;
        else
            edata.autoneg = AUTONEG_DISABLE;

        linuxEthtoolCmd(boundIface->ifName().c_str(),
                        ETHTOOL_SSET, &edata);
    }

    void LinuxPort::renegotiate()
    {
        struct ethtool_cmd edata;

        if (!boundIface)
            return;

        memset(&edata, 0, sizeof(edata));
        linuxEthtoolCmd(boundIface->ifName().c_str(),
                        ETHTOOL_NWAY_RST, &edata);
    }

    MACAddress LinuxPort::permanentMAC() const
    {
        struct ethtool_perm_addr    *edata;

        if (!boundIface)
            return MACAddress();

        edata = (ethtool_perm_addr *)calloc(1,
                    sizeof(struct ethtool_perm_addr) + ETH_ALEN);
        if (!edata)
            return MACAddress();

        edata->size = ETH_ALEN;
        if (linuxEthtoolCmd(boundIface->ifName().c_str(),
                            ETHTOOL_GPERMADDR, edata) < 0)
        {
            free(edata);
            return MACAddress();
        }

        MACAddress mac = MACAddress(edata->data[0], edata->data[1],
                                    edata->data[2], edata->data[3],
                                    edata->data[4], edata->data[5]);
        free(edata);
        return mac;
    }

    class LinuxInterface : public Interface {
        const NIC *owner;
        Port *boundPort;
        String sysfsPath;
    public:
        LinuxInterface(const NIC *up, unsigned i, const char *path) :
            Interface(i),
            owner(up),
            boundPort(NULL),
            sysfsPath(path) { };

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

        /// Check if link status has changed from @p prev to @p exp
        bool linkAlarm(bool prev, bool exp) const;
    };

    bool LinuxInterface::ifStatus() const
    {
        struct ifreq    ifr;

        memset(&ifr, 0, sizeof(ifr));
        if (linuxIOctlCmd(ifName().c_str(), SIOCGIFFLAGS, &ifr))
            return false;

        if (ifr.ifr_flags & IFF_UP)
            return true;

        return false;
    }

    void LinuxInterface::enable(bool st)
    {
        struct ifreq    ifr;

        memset(&ifr, 0, sizeof(ifr));
        if (linuxIOctlCmd(ifName().c_str(), SIOCGIFFLAGS, &ifr))
            return;

        if (st)
            ifr.ifr_flags |= IFF_UP;
        else
            ifr.ifr_flags &= ~IFF_UP;

        linuxIOctlCmd(ifName().c_str(), SIOCSIFFLAGS, &ifr);
    }

    uint64 LinuxInterface::mtu() const
    {
        struct ifreq    ifr;

        memset(&ifr, 0, sizeof(ifr));
        if (linuxIOctlCmd(ifName().c_str(), SIOCGIFMTU, &ifr))
            return 0;

        return ifr.ifr_mtu;
    }

    void LinuxInterface::mtu(uint64 u)
    {
        struct ifreq    ifr;

        memset(&ifr, 0, sizeof(ifr));
        ifr.ifr_mtu = u;
        linuxIOctlCmd(ifName().c_str(), SIOCSIFMTU, &ifr);
    }

    String LinuxInterface::ifName() const
    {
        String  res;
        char   *basec = strdup(sysfsPath.c_str());
        char   *bname = basename(basec);

        res.append(bname);
        free(basec);

        return res;
    }

    MACAddress LinuxInterface::currentMAC() const
    {
        struct ifreq    ifr;

        memset(&ifr, 0, sizeof(ifr));
        ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
        if (linuxIOctlCmd(ifName().c_str(), SIOCGIFHWADDR, &ifr) < 0)
            return MACAddress();

        return MACAddress(ifr.ifr_hwaddr.sa_data[0],
                          ifr.ifr_hwaddr.sa_data[1],
                          ifr.ifr_hwaddr.sa_data[2],
                          ifr.ifr_hwaddr.sa_data[3],
                          ifr.ifr_hwaddr.sa_data[4],
                          ifr.ifr_hwaddr.sa_data[5]);
    }

    void LinuxInterface::currentMAC(const MACAddress& mac)
    {
        int             i;
        struct ifreq    ifr;

        memset(&ifr, 0, sizeof(ifr));
        ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
        for (i = 0; i < ETH_ALEN; i++)
            ifr.ifr_hwaddr.sa_data[i] = mac.address[i];

        linuxIOctlCmd(ifName().c_str(), SIOCSIFHWADDR, &ifr);
    }

    bool LinuxInterface::linkAlarm(bool prev, bool exp) const
    {
        bool cur = ifStatus();

        return (prev != cur) && (prev == exp);
    }

    class LinuxNICFirmware : public NICFirmware {
        const NIC *owner;
        const Interface *boundIface;
    public:
        LinuxNICFirmware(const NIC *o, const Interface *bi) :
            owner(o), boundIface(bi) {}
        virtual const NIC *nic() const { return owner; }
        virtual VersionInfo version() const;
        virtual bool syncInstall(const char *)
        {
            return true;
        }
        virtual void initialize() {};
    };

    VersionInfo LinuxNICFirmware::version() const
    {
        struct ethtool_drvinfo edata;

        if (!boundIface)
            return VersionInfo("");

        memset(&edata, 0, sizeof(edata));
        if (linuxEthtoolCmd(boundIface->ifName().c_str(),
                            ETHTOOL_GDRVINFO, &edata) < 0)
            return VersionInfo("");

        return VersionInfo(edata.fw_version);
    }

    class LinuxBootROM : public BootROM {
        const NIC *owner;
        const Interface *boundIface;
    public:
        LinuxBootROM(const NIC *o, const Interface *bi) :
            owner(o), boundIface(bi) {}
        virtual const NIC *nic() const { return owner; }
        virtual VersionInfo version() const;
        virtual bool syncInstall(const char *) { return true; }
        virtual void initialize() {};
    };

    VersionInfo LinuxBootROM::version() const
    {
        FILE        *mtd_list;
        char         line[BUF_MAX_LEN];
        char         dev_path[BUF_MAX_LEN];
        char         buf[BOOT_ROM_VERSION_MAX_OFFSET + 1];
        char        *dev_name = NULL;
        int          read_bytes;
        int          offset;
        int          fd;
        int          i;
        const char   prefix[] = BOOT_ROM_VERSION_PREFIX;

        if (!boundIface)
            return VersionInfo("");

        String  ifname = boundIface->ifName();

        mtd_list = fopen("/proc/mtd", "r");
        if (!mtd_list)
        {
            LINUX_LOG_ERR("Failed to open /proc/mtd");
            return VersionInfo("");
        }

        fgets(line, sizeof(line), mtd_list);
        while (fgets(line, sizeof(line), mtd_list))
        {
            char *p;

            p = strchr(line, ':');
            if (!p)
                break;

            *p++ = 0;
            if (!strstr(p, ifname.c_str()) ||
                !strstr(p, "sfc_exp_rom"))
                continue;

            dev_name = line;
            break;
        }
        fclose(mtd_list);

        if (!dev_name)
            return VersionInfo("");

        sprintf(dev_path, "/dev/%sro", dev_name);

        fd = open(dev_path, O_RDONLY);
        if (fd < 0)
        {
            LINUX_LOG_ERR("Failed to open file %s: %s",
                            dev_path, strerror(errno));
            return VersionInfo("");
        }

        read_bytes = read(fd, buf, BOOT_ROM_VERSION_MAX_OFFSET);
        close(fd);

        if (read_bytes < 0)
        {
            LINUX_LOG_ERR("Failed to read file %s: %s",
                            dev_path, strerror(errno));
            return VersionInfo("");
        }

        if (read_bytes != BOOT_ROM_VERSION_MAX_OFFSET)
            return VersionInfo("");

        offset = 0;
        while (offset < read_bytes - (int)sizeof(prefix))
        {
            if (memcmp(&buf[offset], prefix, sizeof(prefix) - 1) == 0)
            {
                offset += sizeof(prefix) - 1;
                break;
            }
            offset++;
        }

        if (offset == BOOT_ROM_VERSION_MAX_OFFSET - sizeof (prefix))
            return VersionInfo("");

        i = 0;
        while ((offset + i < BOOT_ROM_VERSION_MAX_OFFSET) &&
                (buf[offset + i] != ')'))
            i++;

        if (offset + i == BOOT_ROM_VERSION_MAX_OFFSET)
            return VersionInfo("");

        buf[offset + i] = '\0';

        return VersionInfo(buf + offset);
    }

    class LinuxDiagnostic : public Diagnostic {
        bool online;
        const NIC *owner;
        const Interface *boundIface;
        Result testPassed;
        static const char sampleDescr[];
        static const String diagOnlineName;
        static const String diagOfflineName;
    public:
        LinuxDiagnostic(bool on, const NIC *o, const Interface *bi) :
            Diagnostic(sampleDescr), online(on), owner(o), boundIface(bi),
            testPassed(NotKnown) {}
        virtual Result syncTest();
        virtual Result result() const { return testPassed; }
        virtual const NIC *nic() const { return owner; }
        virtual void initialize() {};
        virtual const String& genericName() const;
    };

    const char LinuxDiagnostic::sampleDescr[] = "Linux Diagnostic";
    const String LinuxDiagnostic::diagOnlineName = "Diagnostic Online";
    const String LinuxDiagnostic::diagOfflineName = "Diagnostic Offline";

    Diagnostic::Result LinuxDiagnostic::syncTest()
    {
        struct ethtool_drvinfo  drv_data;
        struct ethtool_test    *test_data;

        if (!boundIface)
            return NotKnown;

        memset(&drv_data, 0, sizeof(drv_data));
        if (linuxEthtoolCmd(boundIface->ifName().c_str(),
                            ETHTOOL_GDRVINFO, &drv_data) < 0)
            return NotKnown;

        test_data = (ethtool_test*)calloc(1, sizeof(*test_data) +
                           drv_data.testinfo_len * sizeof(*test_data->data));
        if (!test_data)
            return NotKnown;

        if (!online)
            test_data->flags = ETH_TEST_FL_OFFLINE;
        test_data->len = drv_data.testinfo_len;
        if (linuxEthtoolCmd(boundIface->ifName().c_str(),
                            ETHTOOL_TEST, test_data) < 0)
        {
            free(test_data);
            return NotKnown;
        }

        testPassed = test_data->flags & ETH_TEST_FL_FAILED ? Failed : Passed;
        log().logStatus(testPassed == Passed ? "passed" : "failed");
        free(test_data);
        return testPassed;
    }

    const String& LinuxDiagnostic::genericName() const
    {
        return online ? diagOnlineName : diagOfflineName;
    }

    class LinuxNIC : public NIC {
        int portNum;
        int intfNum;
        LinuxPort port0;
        LinuxPort port1;
        LinuxInterface intf0;
        LinuxInterface intf1;
        LinuxNICFirmware nicFw;
        LinuxBootROM rom;
        LinuxDiagnostic diagOnline;
        LinuxDiagnostic diagOffline;
    protected:
        virtual void setupPorts()
        {
            port0.initialize();
            if (portNum == 2)
                port1.initialize();
        }
        virtual void setupInterfaces()
        {
            if (intfNum > 0)
            {
                intf0.initialize();
                intf0.bindToPort(&port0);
                port0.bindToIface(&intf0);
            }
            else
                port0.bindToIface(NULL);

            if (portNum == 2)
            {
                intf1.initialize();
                intf1.bindToPort(&port1);
                port1.bindToIface(&intf1);
            }
        }
        virtual void setupFirmware()
        {
            nicFw.initialize();
            rom.initialize();
        }
        virtual void setupDiagnostics()
        {
            diagOnline.initialize();
            diagOffline.initialize();
        }
    public:
        String sysfsPath;

        LinuxNIC(unsigned idx, char *NICPath, int pnum, int inum,
                 const char *ifPath0, const char *ifPath1) :
            NIC(idx), portNum(pnum), intfNum(inum),
            port0(this, 0), port1(this, 1),
            intf0(this, 0, ifPath0), intf1(this, 1, ifPath1),
            nicFw(this, &intf0), rom(this, &intf0),
            diagOnline(true, this, pnum > 0 ? &intf0 : NULL),
            diagOffline(false, this, pnum > 0 ? &intf0 : NULL),
            sysfsPath(NICPath)
        {}

        virtual VitalProductData vitalProductData() const;
        Connector connector() const;
        uint64 supportedMTU() const
        { 
            // This one defined in the driver (EFX_MAX_MTU)
            return 9216;
        }

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
            if (!en.process(port0))
                return false;
            if (portNum == 2)
                return en.process(port1);
            return true;
        }

        virtual bool forAllPorts(ConstElementEnumerator& en) const
        {
            if (!en.process(port0))
                return false;
            if (portNum == 2)
                return en.process(port1);
            return true;
        }

        virtual bool forAllInterfaces(ElementEnumerator& en)
        {
            if (intfNum == 0)
                return true;
            if (!en.process(intf0))
                return false;
            if (intfNum == 2)
                return en.process(intf1);
            return true;
        }

        virtual bool forAllInterfaces(ConstElementEnumerator& en) const
        {
            if (intfNum == 0)
                return true;
            if (!en.process(intf0))
                return false;
            if (intfNum == 2)
                return en.process(intf1);
            return true;
        }
        virtual bool forAllDiagnostics(ElementEnumerator& en)
        {
            if (!en.process(diagOnline))
                return false;
            return en.process(diagOffline);
        }

        virtual bool forAllDiagnostics(ConstElementEnumerator& en) const
        {
            if (!en.process(diagOnline))
                return false;
            return en.process(diagOffline);
        }

        virtual PCIAddress pciAddress() const;

        bool tempAlarm() const;
        bool voltAlarm() const;
    };

    VitalProductData LinuxNIC::vitalProductData() const
    {
        char    path[SYS_PATH_MAX_LEN];
        int     fd;
        ssize_t size;
        String  sno;
        String  pno;

        unsigned        deviceId = pciAddress().device();
        const char     *modelId;

        sprintf(path, "%s/vpd", sysfsPath.c_str());

        fd = open(path, O_RDONLY);
        if (fd < 0)
            return VitalProductData("", "", "", "", "", "");

        while (true)
        {
            char                buf[1024];
            int                 field_len;
            int                 area_len;
            unsigned char       tag;
            bool out            = false;
            bool is_pn          = false;
            bool is_sn          = false;

            // Read header
            size = read(fd, buf, 3);
            if (size <= 0)
            {
                close(fd);
                break;
            }

            tag = buf[0];

            if (tag & 0x80)
                field_len = buf[1] + (buf[2] << 8);
            else
                field_len = buf[2];// tag & 7;

            switch (tag)
            {
                case VPD_TAG_END:
                case 0x0F:
                    out = true;
                    break;
                case VPD_TAG_ID:
                    size = read(fd, buf, field_len);
                    if (size != field_len)
                    {
                        out = true;
                        break;
                    }
                    buf[size] = '\0';
                    continue;
                case VPD_TAG_R:
                case VPD_TAG_W:
                    area_len = field_len;
                    continue;
            }
            if (out)
                break;

            if (buf[1] == 'N')
            {
                if (buf[0] == 'S')
                    is_sn = true;
                else if (buf[0] == 'P')
                    is_pn = true;
            }
            size = read(fd, buf, field_len);
            if (size != field_len)
                break;

            buf[size] = '\0';
            if (is_sn)
                sno = String(buf);
            if (is_pn)
                pno = String(buf);
        }

        close(fd);

        switch (deviceId)
        {
            case 0x0703: modelId = "SFC4000 rev A net"; break;
            case 0x0710: modelId = "SFC4000 rev B"; break;
            case 0x0803: modelId = "SFC9020"; break;
            case 0x0813: modelId = "SFC9021"; break;
            case 0x1803: modelId = "SFC9020 Virtual Function"; break;
            case 0x1813: modelId = "SFC9021 Virtual Function"; break;
            case 0x6703: modelId = "SFC4000 rev A iSCSI/Onload"; break;
            case 0xc101: modelId = "EF1-21022T"; break;
            default: modelId = "";
        }

        return VitalProductData(sno, "", sno, pno,
                                modelId, pno);
    }

    NIC::Connector LinuxNIC::connector() const
    {
        VitalProductData        vpd = vitalProductData();
        const char             *part = vpd.part().c_str();
        char                    last = 'T';

        if (*part != '\0')
            last = part[strlen(part) - 1];

       // Mapping is taken from:
       // http://www.solarflare.com/Content/UserFiles/Media/Solarflare_Onload_Performant_10GbE_Adapters_Chart_detail.png
        switch (last)
        {
            case 'F': return NIC::SFPPlus;
            case 'K': // fallthrough
            case 'H': return NIC::Mezzanine;
            case 'T': return NIC::RJ45;

            default: return NIC::RJ45;
        }
    }

    PCIAddress LinuxNIC::pciAddress() const
    {
        unsigned        domain;
        unsigned        bus;
        unsigned        deviceId;
        char            buf[BUF_MAX_LEN];
        char           *ptr;

        const char *addr = sysfsPath.c_str() +
                                strlen(SYS_PCI_DEVICE_PATH) + 1;

        domain = strtoul(addr, &ptr, 16);
        ptr++;
        bus = strtoul(ptr, NULL, 16);

        if (linuxDeviceGetAttr(sysfsPath.c_str(), "device",
                            buf, sizeof(buf)) < 0)
            deviceId = 0;
        else
            deviceId = strtoul(buf, NULL, 16);

        return PCIAddress(domain, bus, deviceId);
    }

    bool LinuxNIC::tempAlarm() const
    {
        char            buf[BUF_MAX_LEN];
        int             i;
        bool            alarm = false;

        for (i = 1;; i++)
        {
            char temp_name[BUF_MAX_LEN];

            sprintf(temp_name, "temp%d_alarm", i);
            if (linuxDeviceGetAttr(sysfsPath.c_str(), temp_name,
                                   buf, sizeof(buf)) < 0)
                break;

            if (*buf == '1')
            {
                alarm = true;
                break;
            }
        }

        return alarm;
    }

    bool LinuxNIC::voltAlarm() const
    {
        char            buf[BUF_MAX_LEN];
        int             i;
        bool            alarm = false;

        for (i = 1;; i++)
        {
            char temp_name[BUF_MAX_LEN];

            sprintf(temp_name, "in%d_alarm", i);
            if (linuxDeviceGetAttr(sysfsPath.c_str(), temp_name,
                                   buf, sizeof(buf)) < 0)
                break;

            if (*buf == '1')
            {
                alarm = true;
                break;
            }
        }

        return alarm;
    }

    class LinuxDriver : public Driver {
        const Package *owner;
        VersionInfo vers;
        static const String drvName;
    public:
        LinuxDriver(const Package *pkg, const String& d, const String& sn,
                     const VersionInfo& v) :
            Driver(d, sn), owner(pkg), vers(v) {}
        virtual VersionInfo version() const;
        virtual void initialize() {};
        virtual bool syncInstall(const char *) { return false; }
        virtual const String& genericName() const { return description(); }
        virtual const Package *package() const { return owner; }
    };

    VersionInfo LinuxDriver::version() const
    {
        char                 value[BUF_MAX_LEN];
        DIR                 *device_dir;
        struct dirent       *device;
        char                *iface_name = NULL;

        if (linuxDeviceGetAttr(SYS_SF_MOD_PATH, "version",
                               value, sizeof(value)) > 0)
            return VersionInfo(value);


        device_dir = opendir(SYS_SF_DRV_PATH);
        if (device_dir == NULL)
            return VersionInfo("");

        for (device = readdir(device_dir);
             device != NULL;
             device = readdir(device_dir))
        {
            char                buf[SYS_PATH_MAX_LEN];
            struct dirent      *iface;
            DIR                *net_dir;

            if (device->d_name[0] == '.')
                continue;

            sprintf(buf, "%s/%s", SYS_SF_DRV_PATH,
                    device->d_name);

            if (!linuxDeviceCheckPath(buf))
                continue;

            sprintf(buf + strlen(buf), "%s", "/net");

            net_dir = opendir(buf);
            if (net_dir == NULL)
                break;

            for (iface = readdir(net_dir); iface != NULL;
                 iface = readdir(net_dir))
            {
                if (strncmp(iface->d_name, "eth", 3) == 0)
                {
                    iface_name = strdup(iface->d_name);
                    break;
                }
            }
            closedir(net_dir);
            if (iface_name != NULL)
                break;
        }
        closedir(device_dir);

        if (iface_name != NULL)
        {
            struct ethtool_drvinfo edata;

            memset(&edata, 0, sizeof(edata));
            if (linuxEthtoolCmd(iface_name,
                                ETHTOOL_GDRVINFO, &edata) < 0)
            {
                free(iface_name);
                return VersionInfo("");
            }

            free(iface_name);
            return VersionInfo(edata.version);
        }

        return VersionInfo("");
    }

    class LinuxLibrary : public Library {
        const Package *owner;
        VersionInfo vers;
    public:
        LinuxLibrary(const Package *pkg, const String& d, const String& sn,
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
    class LinuxKernelPackage : public Package {
        LinuxDriver kernelDriver;
    protected:
        virtual void setupContents() { kernelDriver.initialize(); };
    public:
        LinuxKernelPackage() :
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
    class LinuxManagementPackage : public Package {
        LinuxLibrary providerLibrary;
    protected:
        virtual void setupContents() { providerLibrary.initialize(); };
    public:
        LinuxManagementPackage() :
            Package("CIM Provider RPM", "sfcprovider"),
            providerLibrary(this, "CIM Provider library", "libSolarflare.so",
                            SF_LIBPROV_VERSION) {}
        virtual PkgType type() const { return RPM; }
        virtual VersionInfo version() const
        {
            return VersionInfo(SF_LIBPROV_VERSION);
            }
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
    /// @note all structures are initialised statically,
    /// so initialize() does nothing
    class LinuxSystem : public System {
        LinuxKernelPackage kernelPackage;
        LinuxManagementPackage mgmtPackage;
        LinuxSystem()
        {
            onAlert.setFillPortAlertsInfo(linuxFillPortAlertsInfo);
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
        static LinuxSystem target;
        bool is64bit() const;
        OSType osType() const;
        bool forAllNICs(ConstElementEnumerator& en) const;
        bool forAllNICs(ElementEnumerator& en);
        bool forAllPackages(ConstElementEnumerator& en) const;
        bool forAllPackages(ElementEnumerator& en);
    };

    bool LinuxSystem::is64bit() const
    {
        long wordBits = sysconf(_SC_LONG_BIT);

        if (wordBits == -1)
        {
            LINUX_LOG_ERR("Failed to determine OS bitness");
            return false;
        }
        if (wordBits == 64)
            return true;

        return false;
    }

    System::OSType LinuxSystem::osType() const
    {
        char    buf[SYS_PATH_MAX_LEN];
        ssize_t len;

        if (access("/etc/debian_version", F_OK) == 0)
            return Debian;

        if ((len = readlink("/etc/system-release",
                            buf, sizeof(buf) - 1)) != -1)
        {
            buf[len] = '\0';
            if (strcmp(buf, "redhat-release") == 0)
                return RHEL;
            if (strcmp(buf, "centos-release") == 0)
                return CentOS;
            if (strcmp(buf, "sles-release") == 0)
                return SLES;
            if (strcmp(buf, "oracle-release") == 0)
                return OracleEL;
        }
        
        return GenericLinux;
    }

    bool LinuxSystem::forAllNICs(ConstElementEnumerator& en) const
    {
        int                  NICNum = 0;
        bool                 ret = true;
        DIR                 *device_dir;
        struct dirent       *device;

        device_dir = opendir(SYS_PCI_DEVICE_PATH);
        if (device_dir == NULL)
            return false;

        for (device = readdir(device_dir);
             device != NULL;
             device = readdir(device_dir))
        {
            char        buf[SYS_PATH_MAX_LEN];
            bool        res;
            int         pnum;
            int         inum;
            char       *port_path[2] = {0, };
            char       *iface_path[2] = {0, };
            int         i;

            struct dirent *iface;

            if (device->d_name[0] == '.')
                continue;

            sprintf(buf, "%s/%s", SYS_PCI_DEVICE_PATH, device->d_name);
            if (!linuxDeviceCheckPath(buf))
                continue;

            // assume that dual-port NIC has .0 and .1 numbers in sysfs

            if (buf[strlen(buf) - 1] != '0')
                continue;

            port_path[0] = strdup(buf);
            port_path[1] = strdup(buf);
            port_path[1][strlen(buf) - 1] = '1';
            if (linuxDeviceCheckPath(port_path[1]))
                pnum = 2;
            else
                pnum = 1;

	    inum = 0;
            for (i = 0; i < pnum; i++)
            {
                DIR                 *net_dir;
                struct dirent       *iface;
                bool                 found = false;

                sprintf(buf, "%s/net", port_path[i]);
                net_dir = opendir(buf);
                if (net_dir == NULL)
		{
		    net_dir = opendir(port_path[i]);
		    if (net_dir == NULL)
			continue;

		    for (iface = readdir(net_dir); iface != NULL;
			 iface = readdir(net_dir))
		    {
			if (strncmp(iface->d_name, "net:eth", 7) == 0)
			{
                            char        iface_link[SYS_PATH_MAX_LEN];
                            int         len;

			    sprintf(iface_link, "%s/%s", port_path[i],
                                    iface->d_name);
                            len = readlink(iface_link, buf, SYS_PATH_MAX_LEN);
                            if (len < 0)
                                continue;
                            buf[len] = '\0';
                            found = true;
                            break;
			}
		    }
		}
                else
                {
                    for (iface = readdir(net_dir); iface != NULL;
                         iface = readdir(net_dir))
                    {
                        if (strncmp(iface->d_name, "eth", 3) == 0)
                        {
                            sprintf(buf, "%s/net/%s", port_path[i],
                                    iface->d_name);
                            found = true;
                            break;
                        }
                    }
                }

                if (found)
                {
                    iface_path[i] = strdup(buf);
		    inum++;
                }

                closedir(net_dir);
            }

	    LinuxNIC nic = LinuxNIC(NICNum, port_path[0], pnum, inum,
			        iface_path[0] ? iface_path[0] : "",
			        iface_path[1] ? iface_path[1] : "");
	    nic.initialize();
	    res = en.process(nic);
	    ret = ret && res;
	    NICNum++;

            for (i = 0; i < 2; i++)
            {
                free(port_path[i]);
                free(iface_path[i]);
            }
        }
        closedir(device_dir);

        return ret;
    }

    bool LinuxSystem::forAllNICs(ElementEnumerator& en)
    {
        return forAllNICs((ConstElementEnumerator&) en);
    }

    bool LinuxSystem::forAllPackages(ConstElementEnumerator& en) const
    {
        if (!en.process(kernelPackage))
            return false;
        return en.process(mgmtPackage);
    }

    bool LinuxSystem::forAllPackages(ElementEnumerator& en)
    {
        if (!en.process(kernelPackage))
            return false;
        return en.process(mgmtPackage);
    }

    LinuxSystem LinuxSystem::target;

    System& System::target = LinuxSystem::target;

    ///
    /// Class defining link state alert indication to be
    /// generated on Linux
    ///
    class LinuxLinkStateAlertInfo : public LinkStateAlertInfo {
        String ifName;     ///< Interface name

    protected:

        virtual int updateLinkState()
        {
            curLinkState = getLinkStatus(ifName);
            return 0;
        }

    public:

        friend int linuxFillPortAlertsInfo(Array<AlertInfo *> &info,
                                           const Port *port);
    };

    ///
    /// Class defining sensors alert indications to be
    /// generated on Linux
    ///
    class LinuxSensorsAlertInfo : public SensorsAlertInfo {
        String ifName;     ///< Interface name
        int    fd;         ///< Socket fd

    protected:

        virtual int updateSensors()
        {
            if (fd < 0)
                return -1;
            if (mcdiGetSensors(sensorsCur, fd,
                               true, ifName) != 0)
                return -1;
            return 0;
        }

    public:
        LinuxSensorsAlertInfo()
        {
            // Arbitrary socket
            fd = socket(PF_INET, SOCK_STREAM, 0);
            if (fd < 0)
                CIMPLE_ERR(("Failed to open a socket for checking NIC "
                            "sensors"));
        }
        virtual ~LinuxSensorsAlertInfo()
        {
            if (fd >= 0)
                close(fd);
        }

        friend int linuxFillPortAlertsInfo(Array<AlertInfo *> &info,
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
    static int linuxFillPortAlertsInfo(Array<AlertInfo *> &info,
                                       const Port *port)
    {
        LinuxLinkStateAlertInfo *linkStateInstInfo = NULL;
        LinuxSensorsAlertInfo   *sensorsInstInfo = NULL;

        const LinuxPort *linuxPort =
                      dynamic_cast<const LinuxPort *>(port);

        linkStateInstInfo = new LinuxLinkStateAlertInfo();
        linkStateInstInfo->ifName = linuxPort->boundIface->ifName();
        info.append(linkStateInstInfo);

        sensorsInstInfo = new LinuxSensorsAlertInfo();
        sensorsInstInfo->ifName = linuxPort->boundIface->ifName();
        sensorsInstInfo->portFn = linuxPort->pciAddress().fn();
        info.append(sensorsInstInfo);

        return 0;
    }
}
