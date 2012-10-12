//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENMOD 2.0.24
//
//==============================================================================

/* NOCHKSRC */
#include <cimple/cimple.h>
#include "SF_SoftwareIdentity_Provider.h"
#include "SF_ConcreteJob_Provider.h"
#include "SF_InstalledSoftwareIdentity_Provider.h"
#include "SF_BundleComponent_Provider.h"
#include "SF_ComputerSystem_Provider.h"
#include "SF_NICCard_Provider.h"
#include "SF_PhysicalConnector_Provider.h"
#include "SF_ConnectorOnNIC_Provider.h"
#include "SF_EthernetPort_Provider.h"
#include "SF_PortController_Provider.h"
#include "SF_SystemDevice_Provider.h"
#include "SF_ConnectorRealizesPort_Provider.h"
#include "SF_LANEndpoint_Provider.h"

using namespace cimple;

static int __cimple_SF_SoftwareIdentity_Provider_proc(
    const Registration* registration,
    int operation,
    void* arg0,
    void* arg1,
    void* arg2,
    void* arg3,
    void* arg4,
    void* arg5,
    void* arg6,
    void* arg7)
{
    typedef SF_SoftwareIdentity Class;
    typedef SF_SoftwareIdentity_Provider Provider;

    if (operation != OPERATION_INVOKE_METHOD)
        return Instance_Provider_Proc_T<Provider>::proc(registration,
            operation, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    Provider* provider = (Provider*)arg0;
    const Class* self = (const Class*)arg1;
    const char* meth_name = ((Instance*)arg2)->meta_class->name;

    return -1;
}

static int __cimple_SF_ConcreteJob_Provider_proc(
    const Registration* registration,
    int operation,
    void* arg0,
    void* arg1,
    void* arg2,
    void* arg3,
    void* arg4,
    void* arg5,
    void* arg6,
    void* arg7)
{
    typedef SF_ConcreteJob Class;
    typedef SF_ConcreteJob_Provider Provider;

    if (operation != OPERATION_INVOKE_METHOD)
        return Instance_Provider_Proc_T<Provider>::proc(registration,
            operation, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    Provider* provider = (Provider*)arg0;
    const Class* self = (const Class*)arg1;
    const char* meth_name = ((Instance*)arg2)->meta_class->name;

    if (strcasecmp(meth_name, "KillJob") == 0)
    {
        typedef SF_ConcreteJob_KillJob_method Method;
        Method* method = (Method*)arg2;
        return provider->KillJob(
            self,
            method->DeleteOnKill,
            method->return_value);
    }

    if (strcasecmp(meth_name, "RequestStateChange") == 0)
    {
        typedef SF_ConcreteJob_RequestStateChange_method Method;
        Method* method = (Method*)arg2;
        return provider->RequestStateChange(
            self,
            method->RequestedState,
            method->TimeoutPeriod,
            method->return_value);
    }

    if (strcasecmp(meth_name, "GetError") == 0)
    {
        typedef SF_ConcreteJob_GetError_method Method;
        Method* method = (Method*)arg2;
        return provider->GetError(
            self,
            method->Error,
            method->return_value);
    }

    return -1;
}

static int __cimple_SF_InstalledSoftwareIdentity_Provider_proc(
    const Registration* registration,
    int operation,
    void* arg0,
    void* arg1,
    void* arg2,
    void* arg3,
    void* arg4,
    void* arg5,
    void* arg6,
    void* arg7)
{
    typedef SF_InstalledSoftwareIdentity Class;
    typedef SF_InstalledSoftwareIdentity_Provider Provider;

    if (operation != OPERATION_INVOKE_METHOD)
        return Association_Provider_Proc_T<Provider>::proc(registration,
            operation, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    Provider* provider = (Provider*)arg0;
    const Class* self = (const Class*)arg1;
    const char* meth_name = ((Instance*)arg2)->meta_class->name;

    return -1;
}

static int __cimple_SF_BundleComponent_Provider_proc(
    const Registration* registration,
    int operation,
    void* arg0,
    void* arg1,
    void* arg2,
    void* arg3,
    void* arg4,
    void* arg5,
    void* arg6,
    void* arg7)
{
    typedef SF_BundleComponent Class;
    typedef SF_BundleComponent_Provider Provider;

    if (operation != OPERATION_INVOKE_METHOD)
        return Association_Provider_Proc_T<Provider>::proc(registration,
            operation, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    Provider* provider = (Provider*)arg0;
    const Class* self = (const Class*)arg1;
    const char* meth_name = ((Instance*)arg2)->meta_class->name;

    return -1;
}

static int __cimple_SF_ComputerSystem_Provider_proc(
    const Registration* registration,
    int operation,
    void* arg0,
    void* arg1,
    void* arg2,
    void* arg3,
    void* arg4,
    void* arg5,
    void* arg6,
    void* arg7)
{
    typedef SF_ComputerSystem Class;
    typedef SF_ComputerSystem_Provider Provider;

    if (operation != OPERATION_INVOKE_METHOD)
        return Instance_Provider_Proc_T<Provider>::proc(registration,
            operation, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    Provider* provider = (Provider*)arg0;
    const Class* self = (const Class*)arg1;
    const char* meth_name = ((Instance*)arg2)->meta_class->name;

    if (strcasecmp(meth_name, "RequestStateChange") == 0)
    {
        typedef SF_ComputerSystem_RequestStateChange_method Method;
        Method* method = (Method*)arg2;
        return provider->RequestStateChange(
            self,
            method->RequestedState,
            method->Job,
            method->TimeoutPeriod,
            method->return_value);
    }

    if (strcasecmp(meth_name, "SetPowerState") == 0)
    {
        typedef SF_ComputerSystem_SetPowerState_method Method;
        Method* method = (Method*)arg2;
        return provider->SetPowerState(
            self,
            method->PowerState,
            method->Time,
            method->return_value);
    }

    return -1;
}

static int __cimple_SF_NICCard_Provider_proc(
    const Registration* registration,
    int operation,
    void* arg0,
    void* arg1,
    void* arg2,
    void* arg3,
    void* arg4,
    void* arg5,
    void* arg6,
    void* arg7)
{
    typedef SF_NICCard Class;
    typedef SF_NICCard_Provider Provider;

    if (operation != OPERATION_INVOKE_METHOD)
        return Instance_Provider_Proc_T<Provider>::proc(registration,
            operation, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    Provider* provider = (Provider*)arg0;
    const Class* self = (const Class*)arg1;
    const char* meth_name = ((Instance*)arg2)->meta_class->name;

    if (strcasecmp(meth_name, "IsCompatible") == 0)
    {
        typedef SF_NICCard_IsCompatible_method Method;
        Method* method = (Method*)arg2;
        return provider->IsCompatible(
            self,
            method->ElementToCheck,
            method->return_value);
    }

    if (strcasecmp(meth_name, "ConnectorPower") == 0)
    {
        typedef SF_NICCard_ConnectorPower_method Method;
        Method* method = (Method*)arg2;
        return provider->ConnectorPower(
            self,
            method->Connector,
            method->PoweredOn,
            method->return_value);
    }

    return -1;
}

static int __cimple_SF_PhysicalConnector_Provider_proc(
    const Registration* registration,
    int operation,
    void* arg0,
    void* arg1,
    void* arg2,
    void* arg3,
    void* arg4,
    void* arg5,
    void* arg6,
    void* arg7)
{
    typedef SF_PhysicalConnector Class;
    typedef SF_PhysicalConnector_Provider Provider;

    if (operation != OPERATION_INVOKE_METHOD)
        return Instance_Provider_Proc_T<Provider>::proc(registration,
            operation, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    Provider* provider = (Provider*)arg0;
    const Class* self = (const Class*)arg1;
    const char* meth_name = ((Instance*)arg2)->meta_class->name;

    return -1;
}

static int __cimple_SF_ConnectorOnNIC_Provider_proc(
    const Registration* registration,
    int operation,
    void* arg0,
    void* arg1,
    void* arg2,
    void* arg3,
    void* arg4,
    void* arg5,
    void* arg6,
    void* arg7)
{
    typedef SF_ConnectorOnNIC Class;
    typedef SF_ConnectorOnNIC_Provider Provider;

    if (operation != OPERATION_INVOKE_METHOD)
        return Association_Provider_Proc_T<Provider>::proc(registration,
            operation, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    Provider* provider = (Provider*)arg0;
    const Class* self = (const Class*)arg1;
    const char* meth_name = ((Instance*)arg2)->meta_class->name;

    return -1;
}

static int __cimple_SF_EthernetPort_Provider_proc(
    const Registration* registration,
    int operation,
    void* arg0,
    void* arg1,
    void* arg2,
    void* arg3,
    void* arg4,
    void* arg5,
    void* arg6,
    void* arg7)
{
    typedef SF_EthernetPort Class;
    typedef SF_EthernetPort_Provider Provider;

    if (operation != OPERATION_INVOKE_METHOD)
        return Instance_Provider_Proc_T<Provider>::proc(registration,
            operation, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    Provider* provider = (Provider*)arg0;
    const Class* self = (const Class*)arg1;
    const char* meth_name = ((Instance*)arg2)->meta_class->name;

    if (strcasecmp(meth_name, "RequestStateChange") == 0)
    {
        typedef SF_EthernetPort_RequestStateChange_method Method;
        Method* method = (Method*)arg2;
        return provider->RequestStateChange(
            self,
            method->RequestedState,
            method->Job,
            method->TimeoutPeriod,
            method->return_value);
    }

    if (strcasecmp(meth_name, "SetPowerState") == 0)
    {
        typedef SF_EthernetPort_SetPowerState_method Method;
        Method* method = (Method*)arg2;
        return provider->SetPowerState(
            self,
            method->PowerState,
            method->Time,
            method->return_value);
    }

    if (strcasecmp(meth_name, "Reset") == 0)
    {
        typedef SF_EthernetPort_Reset_method Method;
        Method* method = (Method*)arg2;
        return provider->Reset(
            self,
            method->return_value);
    }

    if (strcasecmp(meth_name, "EnableDevice") == 0)
    {
        typedef SF_EthernetPort_EnableDevice_method Method;
        Method* method = (Method*)arg2;
        return provider->EnableDevice(
            self,
            method->Enabled,
            method->return_value);
    }

    if (strcasecmp(meth_name, "OnlineDevice") == 0)
    {
        typedef SF_EthernetPort_OnlineDevice_method Method;
        Method* method = (Method*)arg2;
        return provider->OnlineDevice(
            self,
            method->Online,
            method->return_value);
    }

    if (strcasecmp(meth_name, "QuiesceDevice") == 0)
    {
        typedef SF_EthernetPort_QuiesceDevice_method Method;
        Method* method = (Method*)arg2;
        return provider->QuiesceDevice(
            self,
            method->Quiesce,
            method->return_value);
    }

    if (strcasecmp(meth_name, "SaveProperties") == 0)
    {
        typedef SF_EthernetPort_SaveProperties_method Method;
        Method* method = (Method*)arg2;
        return provider->SaveProperties(
            self,
            method->return_value);
    }

    if (strcasecmp(meth_name, "RestoreProperties") == 0)
    {
        typedef SF_EthernetPort_RestoreProperties_method Method;
        Method* method = (Method*)arg2;
        return provider->RestoreProperties(
            self,
            method->return_value);
    }

    return -1;
}

static int __cimple_SF_PortController_Provider_proc(
    const Registration* registration,
    int operation,
    void* arg0,
    void* arg1,
    void* arg2,
    void* arg3,
    void* arg4,
    void* arg5,
    void* arg6,
    void* arg7)
{
    typedef SF_PortController Class;
    typedef SF_PortController_Provider Provider;

    if (operation != OPERATION_INVOKE_METHOD)
        return Instance_Provider_Proc_T<Provider>::proc(registration,
            operation, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    Provider* provider = (Provider*)arg0;
    const Class* self = (const Class*)arg1;
    const char* meth_name = ((Instance*)arg2)->meta_class->name;

    if (strcasecmp(meth_name, "RequestStateChange") == 0)
    {
        typedef SF_PortController_RequestStateChange_method Method;
        Method* method = (Method*)arg2;
        return provider->RequestStateChange(
            self,
            method->RequestedState,
            method->Job,
            method->TimeoutPeriod,
            method->return_value);
    }

    if (strcasecmp(meth_name, "SetPowerState") == 0)
    {
        typedef SF_PortController_SetPowerState_method Method;
        Method* method = (Method*)arg2;
        return provider->SetPowerState(
            self,
            method->PowerState,
            method->Time,
            method->return_value);
    }

    if (strcasecmp(meth_name, "Reset") == 0)
    {
        typedef SF_PortController_Reset_method Method;
        Method* method = (Method*)arg2;
        return provider->Reset(
            self,
            method->return_value);
    }

    if (strcasecmp(meth_name, "EnableDevice") == 0)
    {
        typedef SF_PortController_EnableDevice_method Method;
        Method* method = (Method*)arg2;
        return provider->EnableDevice(
            self,
            method->Enabled,
            method->return_value);
    }

    if (strcasecmp(meth_name, "OnlineDevice") == 0)
    {
        typedef SF_PortController_OnlineDevice_method Method;
        Method* method = (Method*)arg2;
        return provider->OnlineDevice(
            self,
            method->Online,
            method->return_value);
    }

    if (strcasecmp(meth_name, "QuiesceDevice") == 0)
    {
        typedef SF_PortController_QuiesceDevice_method Method;
        Method* method = (Method*)arg2;
        return provider->QuiesceDevice(
            self,
            method->Quiesce,
            method->return_value);
    }

    if (strcasecmp(meth_name, "SaveProperties") == 0)
    {
        typedef SF_PortController_SaveProperties_method Method;
        Method* method = (Method*)arg2;
        return provider->SaveProperties(
            self,
            method->return_value);
    }

    if (strcasecmp(meth_name, "RestoreProperties") == 0)
    {
        typedef SF_PortController_RestoreProperties_method Method;
        Method* method = (Method*)arg2;
        return provider->RestoreProperties(
            self,
            method->return_value);
    }

    return -1;
}

static int __cimple_SF_SystemDevice_Provider_proc(
    const Registration* registration,
    int operation,
    void* arg0,
    void* arg1,
    void* arg2,
    void* arg3,
    void* arg4,
    void* arg5,
    void* arg6,
    void* arg7)
{
    typedef SF_SystemDevice Class;
    typedef SF_SystemDevice_Provider Provider;

    if (operation != OPERATION_INVOKE_METHOD)
        return Association_Provider_Proc_T<Provider>::proc(registration,
            operation, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    Provider* provider = (Provider*)arg0;
    const Class* self = (const Class*)arg1;
    const char* meth_name = ((Instance*)arg2)->meta_class->name;

    return -1;
}

static int __cimple_SF_ConnectorRealizesPort_Provider_proc(
    const Registration* registration,
    int operation,
    void* arg0,
    void* arg1,
    void* arg2,
    void* arg3,
    void* arg4,
    void* arg5,
    void* arg6,
    void* arg7)
{
    typedef SF_ConnectorRealizesPort Class;
    typedef SF_ConnectorRealizesPort_Provider Provider;

    if (operation != OPERATION_INVOKE_METHOD)
        return Association_Provider_Proc_T<Provider>::proc(registration,
            operation, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    Provider* provider = (Provider*)arg0;
    const Class* self = (const Class*)arg1;
    const char* meth_name = ((Instance*)arg2)->meta_class->name;

    return -1;
}

static int __cimple_SF_LANEndpoint_Provider_proc(
    const Registration* registration,
    int operation,
    void* arg0,
    void* arg1,
    void* arg2,
    void* arg3,
    void* arg4,
    void* arg5,
    void* arg6,
    void* arg7)
{
    typedef SF_LANEndpoint Class;
    typedef SF_LANEndpoint_Provider Provider;

    if (operation != OPERATION_INVOKE_METHOD)
        return Instance_Provider_Proc_T<Provider>::proc(registration,
            operation, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

    Provider* provider = (Provider*)arg0;
    const Class* self = (const Class*)arg1;
    const char* meth_name = ((Instance*)arg2)->meta_class->name;

    if (strcasecmp(meth_name, "RequestStateChange") == 0)
    {
        typedef SF_LANEndpoint_RequestStateChange_method Method;
        Method* method = (Method*)arg2;
        return provider->RequestStateChange(
            self,
            method->RequestedState,
            method->Job,
            method->TimeoutPeriod,
            method->return_value);
    }

    return -1;
}

CIMPLE_MODULE(Solarflare_Module);
CIMPLE_INSTANCE_PROVIDER(SF_SoftwareIdentity_Provider);
CIMPLE_INSTANCE_PROVIDER(SF_ConcreteJob_Provider);
CIMPLE_ASSOCIATION_PROVIDER(SF_InstalledSoftwareIdentity_Provider);
CIMPLE_ASSOCIATION_PROVIDER(SF_BundleComponent_Provider);
CIMPLE_INSTANCE_PROVIDER(SF_ComputerSystem_Provider);
CIMPLE_INSTANCE_PROVIDER(SF_NICCard_Provider);
CIMPLE_INSTANCE_PROVIDER(SF_PhysicalConnector_Provider);
CIMPLE_ASSOCIATION_PROVIDER(SF_ConnectorOnNIC_Provider);
CIMPLE_INSTANCE_PROVIDER(SF_EthernetPort_Provider);
CIMPLE_INSTANCE_PROVIDER(SF_PortController_Provider);
CIMPLE_ASSOCIATION_PROVIDER(SF_SystemDevice_Provider);
CIMPLE_ASSOCIATION_PROVIDER(SF_ConnectorRealizesPort_Provider);
CIMPLE_INSTANCE_PROVIDER(SF_LANEndpoint_Provider);

#ifdef CIMPLE_PEGASUS_MODULE
  CIMPLE_PEGASUS_PROVIDER_ENTRY_POINT;
# define __CIMPLE_FOUND_ENTRY_POINT
#endif

#ifdef CIMPLE_CMPI_MODULE
  CIMPLE_CMPI_INSTANCE_PROVIDER(SF_SoftwareIdentity_Provider);
  CIMPLE_CMPI_INSTANCE_PROVIDER2(SF_SoftwareIdentity_Provider, SF_SoftwareIdentity);
  CIMPLE_CMPI_INSTANCE_PROVIDER(SF_ConcreteJob_Provider);
  CIMPLE_CMPI_INSTANCE_PROVIDER2(SF_ConcreteJob_Provider, SF_ConcreteJob);
  CIMPLE_CMPI_ASSOCIATION_PROVIDER(SF_InstalledSoftwareIdentity_Provider);
  CIMPLE_CMPI_ASSOCIATION_PROVIDER2(SF_InstalledSoftwareIdentity_Provider, SF_InstalledSoftwareIdentity);
  CIMPLE_CMPI_ASSOCIATION_PROVIDER(SF_BundleComponent_Provider);
  CIMPLE_CMPI_ASSOCIATION_PROVIDER2(SF_BundleComponent_Provider, SF_BundleComponent);
  CIMPLE_CMPI_INSTANCE_PROVIDER(SF_ComputerSystem_Provider);
  CIMPLE_CMPI_INSTANCE_PROVIDER2(SF_ComputerSystem_Provider, SF_ComputerSystem);
  CIMPLE_CMPI_INSTANCE_PROVIDER(SF_NICCard_Provider);
  CIMPLE_CMPI_INSTANCE_PROVIDER2(SF_NICCard_Provider, SF_NICCard);
  CIMPLE_CMPI_INSTANCE_PROVIDER(SF_PhysicalConnector_Provider);
  CIMPLE_CMPI_INSTANCE_PROVIDER2(SF_PhysicalConnector_Provider, SF_PhysicalConnector);
  CIMPLE_CMPI_ASSOCIATION_PROVIDER(SF_ConnectorOnNIC_Provider);
  CIMPLE_CMPI_ASSOCIATION_PROVIDER2(SF_ConnectorOnNIC_Provider, SF_ConnectorOnNIC);
  CIMPLE_CMPI_INSTANCE_PROVIDER(SF_EthernetPort_Provider);
  CIMPLE_CMPI_INSTANCE_PROVIDER2(SF_EthernetPort_Provider, SF_EthernetPort);
  CIMPLE_CMPI_INSTANCE_PROVIDER(SF_PortController_Provider);
  CIMPLE_CMPI_INSTANCE_PROVIDER2(SF_PortController_Provider, SF_PortController);
  CIMPLE_CMPI_ASSOCIATION_PROVIDER(SF_SystemDevice_Provider);
  CIMPLE_CMPI_ASSOCIATION_PROVIDER2(SF_SystemDevice_Provider, SF_SystemDevice);
  CIMPLE_CMPI_ASSOCIATION_PROVIDER(SF_ConnectorRealizesPort_Provider);
  CIMPLE_CMPI_ASSOCIATION_PROVIDER2(SF_ConnectorRealizesPort_Provider, SF_ConnectorRealizesPort);
  CIMPLE_CMPI_INSTANCE_PROVIDER(SF_LANEndpoint_Provider);
  CIMPLE_CMPI_INSTANCE_PROVIDER2(SF_LANEndpoint_Provider, SF_LANEndpoint);
# define __CIMPLE_FOUND_ENTRY_POINT
#endif

#ifdef CIMPLE_OPENWBEM_MODULE
  CIMPLE_OPENWBEM_PROVIDER(Solarflare_Module);
# define __CIMPLE_FOUND_ENTRY_POINT
#endif

#ifdef CIMPLE_WMI_MODULE
# include "guid.h"
  CIMPLE_WMI_PROVIDER_ENTRY_POINTS(CLSID_Solarflare_Module)
# define __CIMPLE_FOUND_ENTRY_POINT
#endif

#ifndef __CIMPLE_FOUND_ENTRY_POINT
# error "No provider entry point found. Please define one of the following: CIMPLE_PEGASUS_MODULE, CIMPLE_CMPI_MODULE, CIMPLE_OPENWBEM_MODULE, CIMPLE_WMI_MODULE"
#endif
