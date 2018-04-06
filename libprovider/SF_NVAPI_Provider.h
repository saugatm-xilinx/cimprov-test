/***************************************************************************//*! \file liprovider/SF_NVAPI_Provider.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2015/09/03
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Generated by genprov 2.0.24
#ifndef _SF_NVAPI_Provider_h
#define _SF_NVAPI_Provider_h

#include <cimple/cimple.h>
#include "SF_NVAPI.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

class SF_NVAPI_Provider
{
public:

    typedef SF_NVAPI Class;

    SF_NVAPI_Provider();

    ~SF_NVAPI_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_NVAPI* model,
        SF_NVAPI*& instance);

    Enum_Instances_Status enum_instances(
        const SF_NVAPI* model,
        Enum_Instances_Handler<SF_NVAPI>* handler);

    Create_Instance_Status create_instance(
        SF_NVAPI* instance);

    Delete_Instance_Status delete_instance(
        const SF_NVAPI* instance);

    Modify_Instance_Status modify_instance(
        const SF_NVAPI* model,
        const SF_NVAPI* instance);

    Invoke_Method_Status getSFUDevices(
        const SF_NVAPI* self,
        Property<String>& Devices,
        Property<uint32>& return_value);

    Invoke_Method_Status NVExists(
        const SF_NVAPI* self,
        const Property<String>& Device,
        const Property<uint32>& Type,
        const Property<uint32>& Subtype,
        const Property<boolean>& TryOtherDevs,
        Property<boolean>& Exists,
        Property<String>& CorrectDevice,
        Property<uint32>& return_value);

    Invoke_Method_Status NVOpen(
        const SF_NVAPI* self,
        const Property<String>& Device,
        const Property<uint32>& Type,
        const Property<uint32>& Subtype,
        Property<uint32>& NVContext,
        Property<uint32>& return_value);

    Invoke_Method_Status NVClose(
        const SF_NVAPI* self,
        const Property<uint32>& NVContext,
        Property<uint32>& return_value);

    Invoke_Method_Status NVPartSize(
        const SF_NVAPI* self,
        const Property<uint32>& NVContext,
        Property<uint64>& PartSize,
        Property<uint32>& return_value);

    Invoke_Method_Status NVRead(
        const SF_NVAPI* self,
        const Property<uint32>& NVContext,
        const Property<uint64>& Length,
        const Property<sint64>& Offset,
        Property<String>& Data,
        Property<uint32>& return_value);

    Invoke_Method_Status NVReadAll(
        const SF_NVAPI* self,
        const Property<uint32>& NVContext,
        Property<String>& Data,
        Property<uint32>& return_value);

    Invoke_Method_Status NVWriteAll(
        const SF_NVAPI* self,
        const Property<uint32>& NVContext,
        const Property<String>& Data,
        const Property<boolean>& FullErase,
        Property<uint32>& return_value);

    Invoke_Method_Status MCDIV1Command(
        const SF_NVAPI* self,
        const Property<String>& Device,
        Property<uint32>& Command,
        Property<uint32>& Len,
        Property<uint32>& RC,
        Property<String>& Payload,
        Property<sint32>& Ioctl_rc,
        Property<uint32>& Ioctl_errno,
        Property<uint32>& return_value);

    Invoke_Method_Status MCDIV2Command(
        const SF_NVAPI* self,
        const Property<String>& Device,
        Property<uint32>& Command,
        Property<uint32>& InLen,
        Property<uint32>& OutLen,
        Property<uint32>& Flags,
        Property<String>& Payload,
        Property<uint32>& Host_errno,
        Property<sint32>& Ioctl_rc,
        Property<uint32>& Ioctl_errno,
        Property<uint32>& return_value);

    Invoke_Method_Status MCDIRead(
        const SF_NVAPI* self,
        const Property<String>& Device,
        const Property<uint32>& PartitionType,
        const Property<uint32>& Length,
        const Property<uint32>& Offset,
        Property<String>& Data,
        Property<uint32>& return_value);

    Invoke_Method_Status MCDIWrite(
        const SF_NVAPI* self,
        const Property<String>& Device,
        const Property<uint32>& PartitionType,
        const Property<uint32>& Offset,
        const Property<String>& Data,
        Property<uint32>& return_value);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_NVAPI_Provider_h */
