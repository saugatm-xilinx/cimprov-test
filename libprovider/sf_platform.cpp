/***************************************************************************//*! \file liprovider/sf_platform.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include "sf_provider.h"
#include "SF_SoftwareIdentity.h"
#include "SF_EthernetPort.h"
#include "CIM_RecordLog.h"
#include "cimple/Auto_Mutex.h"

#if defined(linux)
#include <unistd.h>
#endif

namespace solarflare
{
    using cimple::Auto_Mutex;

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

    static void findDriverVersion(const SystemElement& obj,
                                  void *data)
    {
        const SWElement &sw = dynamic_cast<const SWElement&>(obj);

        VersionInfo *vers = reinterpret_cast<VersionInfo *>(data);

        if (vers == NULL)
            return;

        if (sw.classify() == SWElement::SWDriver)
            *vers = sw.version();
    }

    const SWElement *Diagnostic::diagnosticTool() const
    {
        VersionInfo  vers;

        if (!(this->nic() != NULL &&
             (vers = this->nic()->driverVersion()) != VersionInfo()))
        {
            ConstEnumProvClsInsts en(findDriverVersion, &vers);

            System::target.forAllNDiagSoftware(en);
            if (vers == VersionInfo())
                return NULL;
        }

        soft = DiagSWElement(this->description(), this->name(),
                             vers);

        return &soft;
    }

    const String Port::portName = "Port";
    const char Port::portDescription[] = "NIC Ethernet Port";

    const String Interface::ifGenName = "Interface";
    const char Interface::ifGenDescription[] = "NIC Ethernet Interface";

    const String NICFirmware::fwName = "Firmware";
    const char NICFirmware::fwDescription[] = "NIC MC Firmware";
    const char NICFirmware::fwSysname[] = "";

    const String BootROM::romName = "BootROM";
    const char BootROM::romDescription[] = "NIC BootROM";
    const char BootROM::romSysname[] = "";

    const String UEFIROM::uefiromName = "UEFIROM";
    const char UEFIROM::uefiromDescription[] = "NIC UEFIROM";
    const char UEFIROM::uefiromSysname[] = "";

    const char NIC::nicDescription[] = "Solarflare NIC";
    const String NIC::nicName = "Ethernet Adapter";

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

    class DiagSwEnumerator : public ElementEnumerator {
        ElementEnumerator& en;
    public:
        DiagSwEnumerator(ElementEnumerator& e) : en(e) {}
        virtual bool process(SystemElement& n) 
        {
            return static_cast<Diagnostic&>(n).forAllSoftware(en);
        }
    };

    class ConstDiagSwEnumerator : public ConstElementEnumerator {
        ConstElementEnumerator& en;
    public:
        ConstDiagSwEnumerator(ConstElementEnumerator& e) : en(e) {}
        virtual bool process(const SystemElement& n) 
        {
            return static_cast<const Diagnostic&>(n).forAllSoftware(en);
        }
    };

    class NICContentsEnumerator : public ElementEnumerator {
        ElementEnumerator& en;
    public:
        NICContentsEnumerator(ElementEnumerator& e) : en(e) {}
        virtual bool process(SystemElement& se) 
        {
            NIC *nic = dynamic_cast<NIC *>(&se);

            DiagSwEnumerator embedsw(en);

            if (!en.process(*nic))
                return false;

            if (!nic->forAllPorts(en))
                return false;
            if (!nic->forAllInterfaces(en))
                return false;
            if (!nic->forAllDiagnostics(en))
                return false;
            if (!nic->forAllSoftware(en))
                return false;
            if (!nic->forAllDiagnostics(embedsw))
                return false;

            return true;
        }
    };

    class ConstNICContentsEnumerator : public ConstElementEnumerator {
        ConstElementEnumerator& en;
    public:
        ConstNICContentsEnumerator(ConstElementEnumerator& e) : en(e) {}
        virtual bool process(const SystemElement& se) 
        {
            const NIC *nic = dynamic_cast<const NIC *>(&se);

            ConstDiagSwEnumerator embedsw(en);

            if (!en.process(*nic))
                return false;

            if (!nic->forAllPorts(en))
                return false;
            if (!nic->forAllInterfaces(en))
                return false;
            if (!nic->forAllDiagnostics(en))
                return false;
            if (!nic->forAllSoftware(en))
                return false;
            if (!nic->forAllDiagnostics(embedsw))
                return false;

            return true;
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

    bool System::forAllNDiagSoftware(ConstElementEnumerator& en) const
    {
        ConstPackageContentsEnumerator embed(en);
        if (!forAllPackages(embed))
            return false;
        
        ConstNICFwEnumerator embedfw(en);
        if (!forAllNICs(embedfw))
            return false;

        return forAllDrivers(en);
    }

    /// This enumerator enumerates each object passed to it
    /// plus software type object if it can be obtained for
    /// a given object.
    class ConstEnumIncSWTypes : public ConstElementEnumerator {
        ConstElementEnumerator& elmEnum;
        bool swTypesOnly;
        Array<SWType *> types;
    public:
        ConstEnumIncSWTypes(ConstElementEnumerator& en,
                            bool swTOnly = false) : elmEnum(en),
                                                    swTypesOnly(swTOnly) {};

        virtual bool process(const SystemElement& obj)
        {
            bool ret = true;

            unsigned int  i;

            if (!swTypesOnly && !elmEnum.process(obj))
                return false;

            SWType *swType = obj.getSWType();
            if (swType != NULL)
            {
                for (i = 0; i < types.size(); i++)
                    if (*(types[i]) == *swType)
                        break;
                if (i < types.size())
                    delete swType;
                else
                {
                    ret = elmEnum.process(*swType);
                    types.append(swType);
                }
            }
            
            return ret;
        }

        ~ConstEnumIncSWTypes()
        {
            unsigned int i = 0;

            for (i = 0; i < types.size(); i++)
                delete types[i];

            types.clear();
        }
    };

    bool System::forAllSoftwareNTypes(ConstElementEnumerator& en) const
    {
        if (!forAllNDiagSoftware(en))
            return false;

        ConstDiagSwEnumerator embedsw(en);
        if (!forAllDiagnostics(embedsw))
            return false;

        return true;
    }

    bool System::forAllSoftwareTypes(ConstElementEnumerator& en) const
    {
        ConstEnumIncSWTypes    incSwTypesEmbed(en, true);

        return this->forAllSoftwareNTypes(incSwTypesEmbed);
    }

    bool System::forAllSoftware(ConstElementEnumerator& en) const
    {
        ConstEnumIncSWTypes incSwTypesEmbed(en);

        return this->forAllSoftwareNTypes(incSwTypesEmbed);
    }

    bool System::forAllNDiagSoftware(ElementEnumerator& en)
    {
        PackageContentsEnumerator embed(en);
        if (!forAllPackages(embed))
            return false;
        
        NICFwEnumerator embedfw(en);
        if (!forAllNICs(embedfw))
            return false;

        return forAllDrivers(en);
    }

    bool System::forAllSoftware(ElementEnumerator& en)
    {
        if (!forAllNDiagSoftware(en))
            return false;

        DiagSwEnumerator embedsw(en);
        if (!forAllDiagnostics(embedsw))
            return false;

        return true;
    }

    bool System::forAllObjects(ConstElementEnumerator& en) const
    {
        ConstEnumIncSWTypes incSwTypesEmbed(en);

        incSwTypesEmbed.process(System::target);

        // Enumerate software in packages
        ConstPackageContentsEnumerator embedPkg(incSwTypesEmbed);
        if (!forAllPackages(embedPkg))
            return false;

        if (!forAllDrivers(incSwTypesEmbed))
            return false;

        // Enumerate NICs and all objects they contain
        ConstNICContentsEnumerator embedNIC(incSwTypesEmbed);
        if (!forAllNICs(embedNIC))
            return false;

        return true;
    }

    bool System::forAllObjects(ElementEnumerator& en)
    {
        en.process(System::target);

        // Enumerate software in packages
        PackageContentsEnumerator embedPkg(en);
        if (!forAllPackages(embedPkg))
            return false;

        if (!forAllDrivers(en))
            return false;

        // Enumerate NICs and all objects they contain
        NICContentsEnumerator embedNIC(en);
        if (!forAllNICs(embedNIC))
            return false;

        return true;
    }

    SWType *Package::getSWType() const
    {
        SWPkgType *type = new SWPkgType(this->description(),
                                        this->sysName(),
                                        this->genericName(),
                                        this->classify(),
                                        this->type());

        return type;
    }

    VersionInfo NIC::driverVersion() const
    {
        if (driver() != NULL)
            return driver()->version();
        else
            return VersionInfo();
    }

    Array<ReqStateStorageEntry> System::portReqStateStorage;
    Mutex System::portReqStateStorageLock(false);
    Array<ReqStateStorageEntry> System::logReqStateStorage;
    Mutex System::logReqStateStorageLock(false);

    void ReqStateStorageEntry::saveReqState(
                                Array<ReqStateStorageEntry> &storage,
                                const String &name,
                                unsigned reqSt)
    {
        unsigned   i;

        for (i = 0; i < storage.size(); i++)
            if (storage[i].name == name)
                break;

        if (i < storage.size())
            storage[i].requestedState = reqSt;
        else
        {
            ReqStateStorageEntry  entry;

            entry.name = name;
            entry.requestedState = reqSt;
            storage.append(entry);
        }
    }

    int ReqStateStorageEntry::getReqState(
                              const Array<ReqStateStorageEntry> &storage,
                              const String &name,
                              unsigned &value)
    {
        unsigned   i;

        for (i = 0; i < storage.size(); i++)
            if (storage[i].name == name)
                break;
    
        if (i < storage.size())
        {
            value = storage[i].requestedState;
            return 0;
        }

        return -1;
    }

    void System::savePortReqState(const String &portName,
                                  unsigned requestedState)
    {
        Auto_Mutex guard(portReqStateStorageLock);

        ReqStateStorageEntry::saveReqState(portReqStateStorage,
                                           portName,
                                           requestedState);
    }

    unsigned System::getPortReqState(const String &portName)
    {
        Auto_Mutex guard(portReqStateStorageLock);
        unsigned   value = 0;

        if (ReqStateStorageEntry::getReqState(portReqStateStorage,
                                              portName,
                                              value) == 0)
            return value;
        else
#if CIM_SCHEMA_VERSION_MINOR > 0
            return cimple::SF_EthernetPort::_RequestedState::enum_Unknown;
#else
            return 0;
#endif
    }

    void System::saveLogReqState(const String &logName,
                                 unsigned requestedState)
    {
        Auto_Mutex guard(logReqStateStorageLock);

        ReqStateStorageEntry::saveReqState(logReqStateStorage,
                                           logName,
                                           requestedState);
    }

    unsigned System::getLogReqState(const String &logName)
    {
        Auto_Mutex guard(logReqStateStorageLock);
        unsigned   value = 0;

        if (ReqStateStorageEntry::getReqState(logReqStateStorage,
                                              logName,
                                              value) == 0)
            return value;
        else
            return cimple::CIM_RecordLog::_RequestedState::enum_Unknown;
    }
} // namespace
