// Generated by genprov 2.0.24
#ifndef _SF_DiagnosticServiceCapabilities_Provider_h
#define _SF_DiagnosticServiceCapabilities_Provider_h

#include <cimple/cimple.h>
#include "SF_DiagnosticServiceCapabilities.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

class SF_DiagnosticServiceCapabilities_Provider
{

public:

    typedef SF_DiagnosticServiceCapabilities Class;

    SF_DiagnosticServiceCapabilities_Provider();

    ~SF_DiagnosticServiceCapabilities_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_DiagnosticServiceCapabilities* model,
        SF_DiagnosticServiceCapabilities*& instance);

    Enum_Instances_Status enum_instances(
        const SF_DiagnosticServiceCapabilities* model,
        Enum_Instances_Handler<SF_DiagnosticServiceCapabilities>* handler);

    Create_Instance_Status create_instance(
        SF_DiagnosticServiceCapabilities* instance);

    Delete_Instance_Status delete_instance(
        const SF_DiagnosticServiceCapabilities* instance);

    Modify_Instance_Status modify_instance(
        const SF_DiagnosticServiceCapabilities* model,
        const SF_DiagnosticServiceCapabilities* instance);

    Invoke_Method_Status CreateGoalSettings(
        const SF_DiagnosticServiceCapabilities* self,
        const Property< Array<CIM_SettingData*> >& TemplateGoalSettings,
        Property< Array<CIM_SettingData*> >& SupportedGoalSettings,
        Property<uint16>& return_value);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_DiagnosticServiceCapabilities_Provider_h */
