// Generated by genprov 2.0.24
#ifndef _SF_PhysicalConnector_Provider_h
#define _SF_PhysicalConnector_Provider_h

#include <cimple/cimple.h>
#include "SF_PhysicalConnector.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_PhysicalConnector_Provider
{
public:

    typedef SF_PhysicalConnector Class;

    SF_PhysicalConnector_Provider();

    ~SF_PhysicalConnector_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_PhysicalConnector* model,
        SF_PhysicalConnector*& instance);

    Enum_Instances_Status enum_instances(
        const SF_PhysicalConnector* model,
        Enum_Instances_Handler<SF_PhysicalConnector>* handler);

    Create_Instance_Status create_instance(
        SF_PhysicalConnector* instance);

    Delete_Instance_Status delete_instance(
        const SF_PhysicalConnector* instance);

    Modify_Instance_Status modify_instance(
        const SF_PhysicalConnector* model,
        const SF_PhysicalConnector* instance);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_PhysicalConnector_Provider_h */