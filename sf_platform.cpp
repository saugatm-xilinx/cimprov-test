#include "sf_platform.h"

#if defined(linux)
#include <unistd.h>
#endif

namespace solarflare 
{
    const String Port::portName("Ethernet Port");
    const String Port::portDescription("NIC Ethernet Port");

    const String Interface::ifGenName("Ethernet Interface");
    const String Interface::ifGenDescription("NIC Ethernet Interface");

    const String NICFirmware::fwName("Firmware");
    const String NICFirmware::fwDescription("NIC MC Firmware");
    const String NICFirmware::fwSysname("");

    const String BootROM::romName("BootROM");
    const String BootROM::romDescription("NIC BootROM");
    const String BootROM::romSysname("");

    const String NIC::nicDescription = "Solarflare NIC";
    const String NIC::nicName = "Ethernet Adapter";

    const String System::manfId = "Solarflare Inc.";
    const String System::nsPrefix = "Solarflare";
    const String System::systemDescr = "Solarflare-enabled host";
    const String System::systemName = "System";

    class NICPortEnumerator : public NICEnumerator {
        PortEnumerator& en;
    public:
        NICPortEnumerator(PortEnumerator& e) : en(e) {}
        virtual bool process(NIC& n) 
        {
            return n.forAllPorts(en);
        }
    };

    class ConstNICPortEnumerator : public ConstNICEnumerator {
        ConstPortEnumerator& en;
    public:
        ConstNICPortEnumerator(ConstPortEnumerator& e) : en(e) {}
        virtual bool process(const NIC& n) 
        {
            return n.forAllPorts(en);
        }
    };

    class NICInterfaceEnumerator : public NICEnumerator {
        InterfaceEnumerator& en;
    public:
        NICInterfaceEnumerator(InterfaceEnumerator& e) : en(e) {}
        virtual bool process(NIC& n) 
        {
            return n.forAllInterfaces(en);
        }
    };

    class ConstNICInterfaceEnumerator : public ConstNICEnumerator {
        ConstInterfaceEnumerator& en;
    public:
        ConstNICInterfaceEnumerator(ConstInterfaceEnumerator& e) : en(e) {}
        virtual bool process(const NIC& n) 
        {
            return n.forAllInterfaces(en);
        }
    };


    class PackageContentsEnumerator : public SoftwareEnumerator {
        SoftwareEnumerator& en;
    public:
        PackageContentsEnumerator(SoftwareEnumerator& e) : en(e) {}
        virtual bool process(SWElement& se) 
        {
            if (!en.process(se))
                return false;
            return static_cast<Package&>(se).forAllSoftware(en);
        }
    };

    class ConstPackageContentsEnumerator : public ConstSoftwareEnumerator {
        ConstSoftwareEnumerator& en;
    public:
        ConstPackageContentsEnumerator(ConstSoftwareEnumerator& e) : en(e) {}
        virtual bool process(const SWElement& se) 
        {
            if (!en.process(se))
                return false;
            return static_cast<const Package&>(se).forAllSoftware(en);
        }
    };

    class NICFwEnumerator : public NICEnumerator {
        SoftwareEnumerator& en;
    public:
        NICFwEnumerator(SoftwareEnumerator& e) : en(e) {}
        virtual bool process(NIC& n) 
        {
            return n.forAllFw(en);
        }
    };

    class ConstNICFwEnumerator : public ConstNICEnumerator {
        ConstSoftwareEnumerator& en;
    public:
        ConstNICFwEnumerator(ConstSoftwareEnumerator& e) : en(e) {}
        virtual bool process(const NIC& n) 
        {
            return n.forAllFw(en);
        }
    };

    bool System::forAllPorts(ConstPortEnumerator& en) const
    {
        ConstNICPortEnumerator embed(en);
        return forAllNICs(embed);
    }

    bool System::forAllPorts(PortEnumerator& en)
    {
        NICPortEnumerator embed(en);
        return forAllNICs(embed);
    }

    bool System::forAllInterfaces(ConstInterfaceEnumerator& en) const
    {
        ConstNICInterfaceEnumerator embed(en);
        return forAllNICs(embed);
    }

    bool System::forAllInterfaces(InterfaceEnumerator& en)
    {
        NICInterfaceEnumerator embed(en);
        return forAllNICs(embed);
    }


    bool System::forAllSoftware(ConstSoftwareEnumerator& en) const
    {
        ConstPackageContentsEnumerator embed(en);
        if (!forAllPackages(embed))
            return false;
        
        ConstNICFwEnumerator embedfw(en);
        if (!forAllNICs(embedfw))
            return false;

        return true;
    }

    bool System::forAllSoftware(SoftwareEnumerator& en)
    {
        PackageContentsEnumerator embed(en);
        if (!forAllPackages(embed))
            return false;
        
        NICFwEnumerator embedfw(en);
        if (!forAllNICs(embedfw))
            return false;
        return true;
    }
} // namespace
