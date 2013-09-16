// Generated by genprov 2.0.24
#ifndef _SF_ProviderLog_Provider_h
#define _SF_ProviderLog_Provider_h

#include <cimple/cimple.h>
#include "SF_ProviderLog.h"
#include "sf_logging.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

class SF_ProviderLog_Provider
{
    class ChangeState : public solarflare::Action
    {
        unsigned reqState;
    protected:
        virtual void handler(solarflare::SystemElement&, unsigned idx);
    public:
        ChangeState(unsigned rqs, const Instance *inst) : solarflare::Action(inst), reqState(rqs) {}
    };

    class LogClearer : public solarflare::Action 
    {
    protected:
        virtual void handler(solarflare::SystemElement&, unsigned idx);
    public:
        LogClearer(const Instance *inst) : solarflare::Action(inst) {}
    };
public:

    typedef SF_ProviderLog Class;

    SF_ProviderLog_Provider();

    ~SF_ProviderLog_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_ProviderLog* model,
        SF_ProviderLog*& instance);

    Enum_Instances_Status enum_instances(
        const SF_ProviderLog* model,
        Enum_Instances_Handler<SF_ProviderLog>* handler);

    Create_Instance_Status create_instance(
        SF_ProviderLog* instance);

    Delete_Instance_Status delete_instance(
        const SF_ProviderLog* instance);

    Modify_Instance_Status modify_instance(
        const SF_ProviderLog* model,
        const SF_ProviderLog* instance);

    Invoke_Method_Status RequestStateChange(
        const SF_ProviderLog* self,
        const Property<uint16>& RequestedState,
        CIM_ConcreteJob*& Job,
        const Property<Datetime>& TimeoutPeriod,
        Property<uint32>& return_value);

    Invoke_Method_Status ClearLog(
        const SF_ProviderLog* self,
        Property<uint32>& return_value);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_ProviderLog_Provider_h */