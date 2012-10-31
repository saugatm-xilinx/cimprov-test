#include "sf_provider.h"
#include "CIM_ComputerSystem.h"

namespace solarflare 
{
    using cimple::Instance;
    using cimple::Meta_Class;
    using cimple::CIM_ComputerSystem;
    using cimple::Ref;
    using cimple::cast;

    cimple::Instance *SystemElement::cimInstance(const cimple::Meta_Class& cls) const
    {
        const CIMHelper *helper = cimDispatch(cls);
        return helper ? helper->instance(*this) : NULL;
    }

    cimple::Instance *SystemElement::cimReference(const cimple::Meta_Class& cls) const
    {
        const CIMHelper *helper = cimDispatch(cls);
        return helper ? helper->reference(*this) : NULL;
    }
    
    bool SystemElement::cimIsMe(const cimple::Instance& obj) const
    {
        const CIMHelper *helper = cimDispatch(*obj.meta_class);
        return helper ? helper->match(*this, obj) : false;
    }

    const char CIMHelper::solarflareNS[] = "root/solarflare";
    const char CIMHelper::ibmseNS[] = "root/ibmse";
    const char CIMHelper::interopNS[] = "root/pg_interop";
    const char CIMHelper::baseNS[] = "root/cimv2";
    
    String CIMHelper::instanceID(const String& name)
    {
        String result = System::target.prefix();
        result.append(":");
        result.append(name);
        return result;
    }

    Ref<CIM_ComputerSystem> CIMHelper::cimSystem;

    const CIM_ComputerSystem *CIMHelper::findSystem()
    {
        static const char * const namespaces[] = 
        {ibmseNS, solarflareNS, baseNS, NULL};
        
        if (cimSystem)
            return cast<CIM_ComputerSystem *>(cimSystem.ptr());
        
        Ref<CIM_ComputerSystem> system = CIM_ComputerSystem::create();
        Ref<Instance> sysInstance;
        
        for (const char * const *ns = namespaces; *ns != NULL; ns++)
        {
            cimple::Instance_Enumerator ie;
            
            if (cimple::cimom::enum_instances(*ns, system.ptr(), ie) != 0)
                continue;
            
            sysInstance = ie();
            if (sysInstance)
            {
                break;
            }
        }
        if (sysInstance)
        {
            cimSystem.reset(cast<CIM_ComputerSystem *>(sysInstance.ptr()));
        }
        return cimSystem.ptr();
    }

    bool CIMHelper::isOurSystem(const String& sysclass, const String& sysname)
    {
        const CIM_ComputerSystem* ourSys = findSystem();
        
        return (ourSys->CreationClassName.value == sysclass &&
                ourSys->Name.value == sysname);
    }
    
} // namespace
