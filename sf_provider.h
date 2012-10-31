#ifndef SOLARFLARE_SF_PROVIDER_H
#define SOLARFLARE_SF_PROVIDER_H 1

#include "sf_platform.h"
#include "CIM_ComputerSystem.h"

/// This file contains some helpers and utilities 
/// for writing CIMPLE providers. None of the code is platform-dependent

namespace solarflare 
{
    using cimple::Instance;


    /// @brief Abstract class for ties between CIM classes and #SystemElement.
    /// This class is subclassed internally for descendants of #SystemElement.
    /// @sa SystemElement::cimDispatch()
    class CIMHelper {
        static cimple::Ref<cimple::CIM_ComputerSystem> cimSystem;
    public:
        /// @return A CIM reference matching @p obj
        virtual cimple::Instance *reference(const SystemElement& obj) const = 0;
        /// @return A CIM instance matching @p obj
        virtual cimple::Instance *instance(const SystemElement&) const = 0;
        /// @return true iff @p obj corresponds to CIM instance @p inst
        virtual bool match(const SystemElement& obj, const cimple::Instance& inst) const = 0;

        static const char solarflareNS[];
        static const char ibmseNS[];
        static const char interopNS[];
        static const char baseNS[];
        static void initialize()
        {
            System::target.initialize();
        }

        static String instanceID(const String& name);
        static const cimple::CIM_ComputerSystem *findSystem();
        static cimple::CIM_System *systemRef()
        {
            return reinterpret_cast<cimple::CIM_System *>(findSystem()->clone());
        }
        static bool isOurSystem(const String& sysclass, const String& sysname);
        static bool isOurSystem(const cimple::CIM_ComputerSystem* sys)
        {
            return cimple::key_eq(findSystem(), sys);
        }
    };

    template <class CIMClass>
    class EnumInstances : public ConstElementEnumerator {
        cimple::Enum_Instances_Handler<CIMClass> *handler;
    public:
        EnumInstances(cimple::Enum_Instances_Handler<CIMClass> *h) :
            handler(h) {}
        virtual bool process(const SystemElement& obj)
        {
            CIMClass *cimobj = static_cast<CIMClass *>(obj.cimInstance(CIMClass::static_meta_class));
            if (cimobj != NULL)
                handler->handle(cimobj);
            return true;
        }
        static void allSoftware(cimple::Enum_Instances_Handler<CIMClass> *handler)
        {
            EnumInstances<CIMClass> iter(handler);
            System::target.forAllSoftware(iter);
        }
        static void allNICs(cimple::Enum_Instances_Handler<CIMClass> *handler)
        {
            EnumInstances<CIMClass> iter(handler);
            System::target.forAllNICs(iter);
        }
        static void allInterfaces(cimple::Enum_Instances_Handler<CIMClass> *handler)
        {
            EnumInstances<CIMClass> iter(handler);
            System::target.forAllInterfaces(iter);
        }
        static void allPorts(cimple::Enum_Instances_Handler<CIMClass> *handler)
        {
            EnumInstances<CIMClass> iter(handler);
            System::target.forAllPorts(iter);
        }
        static void allDiagnostics(cimple::Enum_Instances_Handler<CIMClass> *handler)
        {
            EnumInstances<CIMClass> iter(handler);
            System::target.forAllDiagnostics(iter);
        }
        static void allObjects(cimple::Enum_Instances_Handler<CIMClass> *handler)
        {
            EnumInstances<CIMClass> iter(handler);
            System::target.forAllSoftware(iter);
            System::target.forAllNICs(iter);
            System::target.forAllInterfaces(iter);
            System::target.forAllPorts(iter);
            System::target.forAllDiagnostics(iter);
        }
    };

    class Lookup : public ElementEnumerator
    {
        SystemElement *obj;
        const Instance *sample;
    public:
        Lookup(const Instance *s) :
            obj(NULL), sample(s) {}
        SystemElement *found() const { return obj; }
        virtual bool process(SystemElement& el)
        {
            if (el.cimIsMe(*sample))
            {
                obj = &el;
                return false;
            }
            return true;
        }
        static SWElement *findSoftware(const Instance& inst)
        {
            Lookup finder(&inst);
            System::target.forAllSoftware(finder);
            return static_cast<SWElement *>(finder.found());
        }
        static NIC *findNIC(const Instance& inst)
        {
            Lookup finder(&inst);
            System::target.forAllNICs(finder);
            return static_cast<NIC *>(finder.found());
        }
        static Interface *findInterface(const Instance& inst)
        {
            Lookup finder(&inst);
            System::target.forAllInterfaces(finder);
            return static_cast<Interface *>(finder.found());
        }
        static Port *findPort(const Instance& inst)
        {
            Lookup finder(&inst);
            System::target.forAllPorts(finder);
            return static_cast<Port *>(finder.found());
        }
        static Diagnostic *findDiagnostic(const Instance& inst)
        {
            Lookup finder(&inst);
            System::target.forAllDiagnostics(finder);
            return static_cast<Diagnostic *>(finder.found());
        }
        static SystemElement *findAny(const Instance& inst)
        {
            Lookup finder(&inst);
            if (!System::target.forAllSoftware(finder))
                return finder.found();
            if (!System::target.forAllNICs(finder))
                return finder.found();
            if (!System::target.forAllInterfaces(finder))
                return finder.found();
            if (!System::target.forAllPorts(finder))
                return finder.found();
            if (!System::target.forAllDiagnostics(finder))
                return finder.found();
            return NULL;
        }
    };
    
    class ObjectCount : public ConstElementEnumerator
    {
        unsigned cnt;
    public:
        ObjectCount() : cnt(0) {};
        virtual bool process(const SystemElement&) { cnt++; return true; }
        unsigned count() const { return cnt; }
    };


} // namespace

#endif // SOLARFLARE_SF_PROVIDER_H
