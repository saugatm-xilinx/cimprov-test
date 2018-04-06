/***************************************************************************//*! \file liprovider/SF_API_Provider.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2018/03/22
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Generated by genprov 2.0.24
#include "SF_API_Provider.h"
#include "sf_provider.h"
#include "sf_core.h"
#include "sf_platform.h"

CIMPLE_NAMESPACE_BEGIN

enum ReturnValue
{
    OK = 0,
    Error = 2,
    InvalidParameter = 5,
};

using namespace solarflare;

SF_API_Provider::SF_API_Provider()
{
}

SF_API_Provider::~SF_API_Provider()
{
}

Load_Status SF_API_Provider::load()
{
    solarflare::CIMHelper::initialize();
    return LOAD_OK;
}

Unload_Status SF_API_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_API_Provider::get_instance(
    const SF_API* model,
    SF_API*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_API_Provider::enum_instances(
    const SF_API* model,
    Enum_Instances_Handler<SF_API>* handler)
{
    SF_API *inst = SF_API::create();

    if (inst == NULL)
        return ENUM_INSTANCES_FAILED;

    inst->ElementName.set("API");
    inst->Description.set("Common API");
    inst->Caption.set("Common API");

    handler->handle(inst);
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_API_Provider::create_instance(
    SF_API* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_API_Provider::delete_instance(
    const SF_API* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_API_Provider::modify_instance(
    const SF_API* model,
    const SF_API* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

///
/// Enumerator used to call getPFVFByPCIAddr() on every NIC.
///
class DevFinder : public solarflare::ConstElementEnumerator
{
    PCIAddress searchAddr;    ///< PCI address to find.
    bool       getAdminIf;    ///< If true, obtain name of an interface
                              ///  with ADMIN privilege.

    bool    success;          ///< Will be set to true if
                              ///  PF/VF was succesfully found.
    uint32  pf;               ///< Obtained PF.
    uint32  vf;               ///< Obtained VF.
    bool    vf_null;          ///< Whether VF is not set (PF itself was
                              ///  found).
    String  NICTag;           ///< Tag of the NIC on which a function
                              ///  was found.
    String  AdminIntf;        ///< Interface with ADMIN privilege
                              ///  on the NIC on which a function was
                              ///  found.

protected:
    virtual bool process(const SystemElement& elem);

public:
    DevFinder() : searchAddr(0, 0, 0, 0),
                  getAdminIf(false),
                  success(false) {}

    ///
    /// Specify whether an interface with ADMIN privilege
    /// should be retrieved from NIC.
    ///
    /// @param getAdmin     If true, interface will be retrieved.
    void findAdminIf(bool getAdmin)
    {
        getAdminIf = getAdmin;
    }

    ///
    /// Set PCI address to be searched for.
    ///
    /// @param pciAddr      String with PCI address.
    ///
    /// @return 0 on success, -1 on failure.
    int setAddr(const char *pciAddr)
    {
        return searchAddr.parse(pciAddr);
    }

    ///
    /// Check whether a device function was found.
    ///
    /// @return true if it was found, false otherwise.
    bool isFound() const { return success; }

    ///
    /// Get Physical Function number.
    ///
    /// @return PF.
    uint32 PF() const { return pf; }

    ///
    /// Get Virtual Function number.
    ///
    /// @return VF.
    uint32 VF() const { return vf; }

    ///
    /// Check whether VF is not set.
    ///
    /// @return true if VF is not set, false otherwise.
    bool VF_NULL() const { return vf_null; }

    ///
    /// Get tag of the NIC on which a function was found.
    ///
    /// @return NIC's tag.
    const String &Tag() const { return NICTag; }

    ///
    /// Get name of an interface with ADMIN privilege
    /// (findAdminIf() should be used before iterating for this to work).
    ///
    /// @return Interface name.
    const String &AdminIfName() const { return AdminIntf; }
};

bool DevFinder::process(const SystemElement& elem)
{
    const solarflare::NIC& nic = static_cast<const solarflare::NIC&>(elem);

    int  rc;
    bool found;

    if (success)
        return true;

    rc = nic.getPFVFByPCIAddr(searchAddr, found, pf, vf, vf_null);
    if (rc == 0 && found)
    {
        success = true;
        NICTag = nic.tag();
        if (getAdminIf)
            nic.getAdminInterface(AdminIntf);
    }

    return true;
}

Invoke_Method_Status SF_API_Provider::GetPFVFByPCI(
    const SF_API* self,
    const Property<String>& PCIAddr,
    Property<uint32>& PF,
    Property<uint32>& VF,
    Property<String>& NICTag,
    Property<uint32>& return_value)
{
    DevFinder finder;

    return_value.set(Error);

    if (PCIAddr.null)
    {
        PROVIDER_LOG_ERR("PCIAddr should be specified for getPFVFbyPCI()");
        return INVOKE_METHOD_OK;
    }

    if (finder.setAddr(PCIAddr.value.c_str()) < 0)
        return INVOKE_METHOD_OK;

    System::target.forAllNICs(finder);

    if (finder.isFound())
    {
        PF.set(finder.PF());
        if (!finder.VF_NULL())
            VF.set(finder.VF());

        NICTag.set(finder.Tag());
        return_value.set(OK);
    }

    return INVOKE_METHOD_OK;
}

///
/// Process common arguments of GetFuncPrivileges() and
/// ModifyFuncPrivileges().
///
/// @note If PCIAddr is specified, PF/VF will be computed from it.
///       If CallingDev is not specified, calling_if will be set to
///       an interface with ADMIN privilege on the NIC on which
///       a function with a given PCI address resides. If PCIAddr
///       is not specified, CallingDev must be specified.
///       If PhysicalFunction is not specified, then operation
///       is meant to be performed for the calling_if itself.
///
/// @param PhysicalFunction     PhysicalFunction parameter.
/// @param VirtualFunction      VirtualFunction parameter.
/// @param PCIAddr              PCI address of target function.
/// @param CallingDev           Device name on which to issue requests.
/// @param pf                   Where to save PF number.
/// @param vf                   Where to save VF number.
/// @param calling_if           Where to save interface name on which
///                             request will be issued.
///
/// @return 0 on success, -1 on failure.
static int parseFuncArguments(const Property<uint32> &PhysicalFunction,
                              const Property<uint32> &VirtualFunction,
                              const Property<String> &PCIAddr,
                              const Property<String> &CallingDev,
                              Property<uint32> &pf,
                              Property<uint32> &vf,
                              String &calling_if)
{
    if (!PhysicalFunction.null)
        pf.set(PhysicalFunction.value);

    if (!VirtualFunction.null)
        vf.set(VirtualFunction.value);

    if (!CallingDev.null)
        calling_if = CallingDev.value;

    if (!PCIAddr.null)
    {
        DevFinder finder;

        if (CallingDev.null)
            finder.findAdminIf(true);

        if (finder.setAddr(PCIAddr.value.c_str()) < 0)
            return -1;

        System::target.forAllNICs(finder);

        if (finder.isFound())
        {
            pf.set(finder.PF());
            if (!finder.VF_NULL())
                vf.set(finder.VF());

            if (CallingDev.null)
                calling_if = finder.AdminIfName();
        }
        else
        {
            PROVIDER_LOG_ERR("Failed to find function "
                             "with PCI address '%s'",
                             PCIAddr.value.c_str());
            return -1;
        }
    }

    if (calling_if.empty())
    {
        PROVIDER_LOG_ERR("Cannot find interface on which to get privileges");
        return -1;
    }

    return 0;
}

Invoke_Method_Status SF_API_Provider::GetFuncPrivileges(
    const SF_API* self,
    const Property<uint32> &PhysicalFunction,
    const Property<uint32> &VirtualFunction,
    const Property<String> &PCIAddr,
    const Property<String> &CallingDev,
    Property<Array_String>& PrivilegeNames,
    Property<Array_uint32>& Privileges,
    Property<uint32>& return_value)
{
    Property<uint32>  pf;
    Property<uint32>  vf;
    String            calling_if;

    return_value.set(Error);

    if (parseFuncArguments(PhysicalFunction, VirtualFunction,
                           PCIAddr, CallingDev, pf, vf,
                           calling_if) < 0)
        return INVOKE_METHOD_OK;

    if (System::target.getFuncPrivileges(calling_if, pf, vf,
                                         PrivilegeNames, Privileges) == 0)
        return_value.set(OK);

    return INVOKE_METHOD_OK;
}

Invoke_Method_Status SF_API_Provider::ModifyFuncPrivileges(
    const SF_API* self,
    const Property<uint32> &PhysicalFunction,
    const Property<uint32> &VirtualFunction,
    const Property<String> &PCIAddr,
    const Property<String> &CallingDev,
    const Property<String>& AddedMask,
    const Property<String>& RemovedMask,
    Property<uint32>& return_value)
{
    Property<uint32>  pf;
    Property<uint32>  vf;
    String            calling_if;

    return_value.set(Error);

    if (parseFuncArguments(PhysicalFunction, VirtualFunction,
                           PCIAddr, CallingDev, pf, vf,
                           calling_if) < 0)
        return INVOKE_METHOD_OK;

    if (System::target.modifyFuncPrivileges(calling_if, pf, vf,
                                            AddedMask, RemovedMask) == 0)
        return_value.set(OK);

    return INVOKE_METHOD_OK;
}

/*@END@*/

CIMPLE_NAMESPACE_END
