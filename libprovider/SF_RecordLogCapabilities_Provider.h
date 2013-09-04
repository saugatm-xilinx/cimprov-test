// Generated by genprov 2.0.24
#ifndef _SF_RecordLogCapabilities_Provider_h
#define _SF_RecordLogCapabilities_Provider_h

#include <cimple/cimple.h>
#include "SF_RecordLogCapabilities.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

class SF_RecordLogCapabilities_Provider
{

public:

    typedef SF_RecordLogCapabilities Class;

    SF_RecordLogCapabilities_Provider();

    ~SF_RecordLogCapabilities_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_RecordLogCapabilities* model,
        SF_RecordLogCapabilities*& instance);

    Enum_Instances_Status enum_instances(
        const SF_RecordLogCapabilities* model,
        Enum_Instances_Handler<SF_RecordLogCapabilities>* handler);

    Create_Instance_Status create_instance(
        SF_RecordLogCapabilities* instance);

    Delete_Instance_Status delete_instance(
        const SF_RecordLogCapabilities* instance);

    Modify_Instance_Status modify_instance(
        const SF_RecordLogCapabilities* model,
        const SF_RecordLogCapabilities* instance);

#if CIM_SCHEMA_VERSION_MINOR < 28
    Invoke_Method_Status CreateGoalSettings(
        const SF_RecordLogCapabilities* self,
        const Property< Array<CIM_SettingData*> >& TemplateGoalSettings,
        Property< Array<CIM_SettingData*> >& SupportedGoalSettings,
        Property<uint16>& return_value);
#endif

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_RecordLogCapabilities_Provider_h */
