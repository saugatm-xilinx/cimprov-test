#include "sf_provider.h"

#if defined(linux)
#include <unistd.h>
#endif

namespace solarflare
{
    const unsigned Diagnostic::maxRecordedEvents = 128;

    String Diagnostic::DiagnosticThread::getThreadID() const
    {
        String tid = CIMHelper::instanceID(owner->name());
        tid.append(":diagThread");
        return tid;
    }

    void Diagnostic::DiagnosticThread::update(Thread *tempThr)
    {
        owner = static_cast<DiagnosticThread *>(tempThr)->owner;
    }

    Thread *Diagnostic::DiagnosticThread::dup() const
    {
        return new DiagnosticThread(owner, id);
    }

    bool Diagnostic::DiagnosticThread::threadProc()
    {
        bool ok;
        onJobCreated.notify(*owner);
        onJobStarted.notify(*owner);
        ok = owner->syncTest() == Passed ? true : false;
        if (ok)
            onJobSuccess.notify(*owner);
        else
            onJobError.notify(*owner);
        return ok;
    }

    unsigned Diagnostic::DiagnosticThread::percentage() const
    {
        switch (currentState())
        {
            case Running:
                return owner->percentage();
            case Succeeded:
            case Failed:
                return 100;
            default:
                return 0;
        }
    }

    void Diagnostic::run(bool sync)
    {
        if (sync)
            syncTest();
        else
            diagThread.start();
    }

    String Diagnostic::name() const
    {
        String n = nic()->name();

        n.append(" ");
        n.append(genericName());
        return n;
    }

    const String Port::portName = "Ethernet Port";
    const char Port::portDescription[] = "NIC Ethernet Port";

    const String Interface::ifGenName = "Ethernet Interface";
    const char Interface::ifGenDescription[] = "NIC Ethernet Interface";

    const String NICFirmware::fwName = "Firmware";
    const char NICFirmware::fwDescription[] = "NIC MC Firmware";
    const char NICFirmware::fwSysname[] = "";

    const String BootROM::romName = "BootROM";
    const char BootROM::romDescription[] = "NIC BootROM";
    const char BootROM::romSysname[] = "";

    const char NIC::nicDescription[] = "Solarflare NIC";
    const String NIC::nicName = "Solarflare: Ethernet Adapter";

    const String System::manfId = "Solarflare Inc.";
    const String System::nsPrefix = "Solarflare";
    const char System::systemDescr[] = "Solarflare-enabled host";
    const String System::systemName = "System";

    class NICPortEnumerator : public ElementEnumerator {
        ElementEnumerator& en;
    public:
        NICPortEnumerator(ElementEnumerator& e) : en(e) {}
        virtual bool process(SystemElement& n) 
        {
            return static_cast<NIC&>(n).forAllPorts(en);
        }
    };

    class ConstNICPortEnumerator : public ConstElementEnumerator {
        ConstElementEnumerator& en;
    public:
        ConstNICPortEnumerator(ConstElementEnumerator& e) : en(e) {}
        virtual bool process(const SystemElement& n) 
        {
            return static_cast<const NIC&>(n).forAllPorts(en);
        }
    };

    class NICIntfEnumerator : public ElementEnumerator {
        ElementEnumerator& en;
    public:
        NICIntfEnumerator(ElementEnumerator& e) : en(e) {}
        virtual bool process(SystemElement& n) 
        {
            return static_cast<NIC&>(n).forAllInterfaces(en);
        }
    };

    class ConstNICIntfEnumerator : public ConstElementEnumerator {
        ConstElementEnumerator& en;
    public:
        ConstNICIntfEnumerator(ConstElementEnumerator& e) : en(e) {}
        virtual bool process(const SystemElement& n) 
        {
            return static_cast<const NIC&>(n).forAllInterfaces(en);
        }
    };


    class NICDiagEnumerator : public ElementEnumerator {
        ElementEnumerator& en;
    public:
        NICDiagEnumerator(ElementEnumerator& e) : en(e) {}
        virtual bool process(SystemElement& n) 
        {
            return static_cast<NIC&>(n).forAllDiagnostics(en);
        }
    };

    class ConstNICDiagEnumerator : public ConstElementEnumerator {
        ConstElementEnumerator& en;
    public:
        ConstNICDiagEnumerator(ConstElementEnumerator& e) : en(e) {}
        virtual bool process(const SystemElement& n) 
        {
            return static_cast<const NIC&>(n).forAllDiagnostics(en);
        }
    };


    class PackageContentsEnumerator : public ElementEnumerator {
        ElementEnumerator& en;
    public:
        PackageContentsEnumerator(ElementEnumerator& e) : en(e) {}
        virtual bool process(SystemElement& se) 
        {
            if (!en.process(se))
                return false;
            return static_cast<Package&>(se).forAllSoftware(en);
        }
    };

    class ConstPackageContentsEnumerator : public ConstElementEnumerator {
        ConstElementEnumerator& en;
    public:
        ConstPackageContentsEnumerator(ConstElementEnumerator& e) : en(e) {}
        virtual bool process(const SystemElement& se) 
        {
            if (!en.process(se))
                return false;
            return static_cast<const Package&>(se).forAllSoftware(en);
        }
    };

    class NICFwEnumerator : public ElementEnumerator {
        ElementEnumerator& en;
    public:
        NICFwEnumerator(ElementEnumerator& e) : en(e) {}
        virtual bool process(SystemElement& n) 
        {
            return static_cast<NIC&>(n).forAllFw(en);
        }
    };

    class ConstNICFwEnumerator : public ConstElementEnumerator {
        ConstElementEnumerator& en;
    public:
        ConstNICFwEnumerator(ConstElementEnumerator& e) : en(e) {}
        virtual bool process(const SystemElement& n) 
        {
            return static_cast<const NIC&>(n).forAllFw(en);
        }
    };

    bool System::forAllPorts(ConstElementEnumerator& en) const
    {
        ConstNICPortEnumerator embed(en);
        return forAllNICs(embed);
    }

    bool System::forAllPorts(ElementEnumerator& en)
    {
        NICPortEnumerator embed(en);
        return forAllNICs(embed);
    }

    bool System::forAllInterfaces(ConstElementEnumerator& en) const
    {
        ConstNICIntfEnumerator embed(en);
        return forAllNICs(embed);
    }

    bool System::forAllInterfaces(ElementEnumerator& en)
    {
        NICIntfEnumerator embed(en);
        return forAllNICs(embed);
    }

    bool System::forAllDiagnostics(ConstElementEnumerator& en) const
    {
        ConstNICDiagEnumerator embed(en);
        return forAllNICs(embed);
    }

    bool System::forAllDiagnostics(ElementEnumerator& en)
    {
        NICDiagEnumerator embed(en);
        return forAllNICs(embed);
    }


    bool System::forAllSoftware(ConstElementEnumerator& en) const
    {
        ConstPackageContentsEnumerator embed(en);
        if (!forAllPackages(embed))
            return false;
        
        ConstNICFwEnumerator embedfw(en);
        if (!forAllNICs(embedfw))
            return false;

        return true;
    }

    bool System::forAllSoftware(ElementEnumerator& en)
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
