/***************************************************************************//*! \file liprovider/SF_SoftwareInstallationService_Provider.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Generated by genprov 2.0.24
#include "SF_SoftwareInstallationService_Provider.h"
#include "SF_SoftwareIdentity_Provider.h"
#include "SF_ComputerSystem_Provider.h"
#include "SF_NICCard_Provider.h"
#include "SF_SoftwareInstallationServiceCapabilities.h"
#include "sf_provider.h"
#include "sf_logging.h"

CIMPLE_NAMESPACE_BEGIN

SF_SoftwareInstallationService_Provider::SF_SoftwareInstallationService_Provider()
{
}

SF_SoftwareInstallationService_Provider::~SF_SoftwareInstallationService_Provider()
{
}

Load_Status SF_SoftwareInstallationService_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_SoftwareInstallationService_Provider::unload()
{
    return UNLOAD_OK;
}

Get_Instance_Status SF_SoftwareInstallationService_Provider::get_instance(
    const SF_SoftwareInstallationService* model,
    SF_SoftwareInstallationService*& instance)
{
    return GET_INSTANCE_UNSUPPORTED;
}

Enum_Instances_Status SF_SoftwareInstallationService_Provider::enum_instances(
    const SF_SoftwareInstallationService* model,
    Enum_Instances_Handler<SF_SoftwareInstallationService>* handler)
{    
    solarflare::EnumInstances<SF_SoftwareInstallationService>::
                                              allSoftwareTypes(handler);
    return ENUM_INSTANCES_OK;
}

Create_Instance_Status SF_SoftwareInstallationService_Provider::create_instance(
    SF_SoftwareInstallationService* instance)
{
    return CREATE_INSTANCE_UNSUPPORTED;
}

Delete_Instance_Status SF_SoftwareInstallationService_Provider::delete_instance(
    const SF_SoftwareInstallationService* instance)
{
    return DELETE_INSTANCE_UNSUPPORTED;
}

Modify_Instance_Status SF_SoftwareInstallationService_Provider::modify_instance(
    const SF_SoftwareInstallationService* model,
    const SF_SoftwareInstallationService* instance)
{
    return MODIFY_INSTANCE_UNSUPPORTED;
}

Invoke_Method_Status SF_SoftwareInstallationService_Provider::RequestStateChange(
    const SF_SoftwareInstallationService* self,
    const Property<uint16>& RequestedState,
    CIM_ConcreteJob*& Job,
    const Property<Datetime>& TimeoutPeriod,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_SoftwareInstallationService_Provider::StartService(
    const SF_SoftwareInstallationService* self,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_SoftwareInstallationService_Provider::StopService(
    const SF_SoftwareInstallationService* self,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_SoftwareInstallationService_Provider::CheckSoftwareIdentity(
    const SF_SoftwareInstallationService* self,
    const CIM_SoftwareIdentity* Source,
    const CIM_ManagedElement* Target,
    const CIM_Collection* Collection,
    Property<Array_uint16>& InstallCharacteristics,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_SoftwareInstallationService_Provider::InstallFromSoftwareIdentity(
    const SF_SoftwareInstallationService* self,
    CIM_ConcreteJob*& Job,
    const Property<Array_uint16>& InstallOptions,
    const Property<Array_String>& InstallOptionsValues,
    const CIM_SoftwareIdentity* Source,
    const CIM_ManagedElement* Target,
    const CIM_Collection* Collection,
    Property<uint32>& return_value)
{
    return INVOKE_METHOD_UNSUPPORTED;
}

bool SF_SoftwareInstallationService_Provider::Installer::process(
                                              solarflare::SystemElement& se)
{
    using namespace solarflare;

    const SWElement& sw = static_cast<const SWElement&>(se);

    SWType          *swType;
    const CIMHelper *helper = NULL;
    unsigned         n;

    swType = sw.getSWType();
    if (swType == NULL)
        return true;

    helper = swType->cimDispatch(*sample->meta_class);
    if (helper == NULL)
    {
        delete swType;
        return true;
    }
    delete swType;

    n = helper->nObjects(se);

    for (unsigned i = 0; i < n; i++)
        if (helper->match(se, *sample, i))
            handler(se, i);

    return true;
}

void SF_SoftwareInstallationService_Provider::Installer::handler(solarflare::SystemElement& se,
                                                                 unsigned)
{
    using namespace solarflare;

    SWElement::InstallRC installRC;

    SWElement& sw = static_cast<SWElement&>(se);

    if (firstRun)
        ok = true;

    if (ok || firstRun)
    {
        installRC = sw.install(uri, true, force, base64_hash);
        if (!(installRC == SWElement::Install_OK ||
              ((installRC == SWElement::Install_NA ||
                installRC == SWElement::Install_Not_Found)
               && skipNoMatchingImage)))
            ok = false;
        if (installRC != SWElement::Install_NA &&
            installRC != SWElement::Install_Not_Found)
            firstRun = false;
    }
}

void SF_SoftwareInstallationService_Provider::NICInstaller::handler(solarflare::SystemElement& se,
                                                                    unsigned)
{
    solarflare::NIC& nic = static_cast<solarflare::NIC&>(se);
    Installer installer(uri, service, force, base64_hash);
    installer.forSoftware(nic);
    ok = installer.isOk();
    runInstallTried = installer.installWasRun();
}

void SF_SoftwareInstallationService_Provider::FwImgInfoGetter::
                            handler(solarflare::SystemElement& se, unsigned)
{
    solarflare::NIC& nic = static_cast<solarflare::NIC&>(se);

    ok = (solarflare::System::target.getRequiredFwImageName(
                                                      nic, fw_type,
                                                      imgType,
                                                      imgSubType,
                                                      imgName,
                                                      curVersion) == 0);
    firstRun = false;
}

void SF_SoftwareInstallationService_Provider::LocalFwImgChecker::
                            handler(solarflare::SystemElement& se, unsigned)
{
    solarflare::NIC& nic = static_cast<solarflare::NIC&>(se);

    ok = (solarflare::System::target.getLocalFwImageVersion(
                                                      nic, fw_type,
                                                      filename.c_str(),
                                                      applicable,
                                                      imgVersion) == 0);
    firstRun = false;
}

Invoke_Method_Status SF_SoftwareInstallationService_Provider::InstallFromURI(
    const SF_SoftwareInstallationService* self,
    CIM_ConcreteJob*& Job,
    const Property<String>& URI,
    const CIM_ManagedElement* Target,
    const Property<Array_uint16>& InstallOptions,
    const Property<Array_String>& InstallOptionsValues,
    Property<uint32>& return_value)
{
    const CIM_ComputerSystem *sys = NULL;

    String base64_hash;
    bool   force = false;

    /// CIMPLE is unable to generate enums for method parameters
    enum ReturnValue 
    {
        OK = 0,
        Error = 2,
        InvalidParameter = 5,
        JobCreated = 4096
    };

    if (URI.null)
    {
        PROVIDER_LOG_ERR("%s(): URI is not specified", __FUNCTION__);
        return_value.set(InvalidParameter);
        return INVOKE_METHOD_OK;
    }

    if (!InstallOptions.null)
    {
        for (size_t i = 0; i < InstallOptions.value.size(); i++)
        {
            switch (InstallOptions.value[i])
            {
                case SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_Force_installation:
                    force = true;
                    break;
                case SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_Update:
                case SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_Repair:
                case SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_Defer_target_system_reset:
                case SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_Log:
                case SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_SilentMode:
                case SF_SoftwareInstallationServiceCapabilities::_SupportedInstallOptions::enum_AdministrativeMode:
                    /* option is supported, do nothing */
                    break;

                case SF_SoftwareInstallationServiceCapabilities::
                        _SupportedInstallOptions::enum_Vendor_Specific:

                        if (InstallOptionsValues.null ||
                            InstallOptionsValues.value[i].size() <= i)
                        {
                            PROVIDER_LOG_ERR("%s(): vendor-specific "
                                             "option supplied without "
                                             "value", __FUNCTION__);
                            return_value.set(InvalidParameter);
                            return INVOKE_METHOD_OK;
                        }
                        base64_hash = InstallOptionsValues.value[i];
                    break;

                    /* option is supported, do nothing */
                default:
                    PROVIDER_LOG_ERR("%s(): unsupported option value %u "
                                     "specified", __FUNCTION__,
                                     InstallOptions.value[i]);
                    return_value.set(InvalidParameter);
                    return INVOKE_METHOD_OK;
            }
        }
    }

    if (Target == NULL ||
        (sys = cast<const CIM_ComputerSystem *>(Target)) != NULL)
    {
        if (Target != NULL &&
            !solarflare::CIMHelper::isOurSystem(sys))
        {
            PROVIDER_LOG_ERR("%s(): incorrect system was "
                             "specified as a target",
                             __FUNCTION__);
            return_value.set(InvalidParameter);
            return INVOKE_METHOD_OK;
        }
        Installer installer(URI.value.c_str(), self,
                            force, base64_hash.c_str(), true);
        installer.forSoftware();
        if (installer.isOk())
            return_value.set(OK);
        else
        {
            if (!installer.installWasRun())
                PROVIDER_LOG_ERR("%s(): no matching software instance "
                                 "was found", __FUNCTION__);
            return_value.set(Error);
        }
    }
    else if (const CIM_Card *card = cast<const CIM_Card *>(Target))
    {
        NICInstaller installer(URI.value.c_str(), self, card,
                               force, base64_hash.c_str());
        installer.forNIC();
        if (installer.isOk())
            return_value.set(OK);
        else
        {
            if (!installer.installWasRun())
                PROVIDER_LOG_ERR("%s(): no matching software instance "
                                 "was found", __FUNCTION__);
            return_value.set(Error);
        }
    }
    else
    {
 
        PROVIDER_LOG_ERR("%s(): incorrect target was specified",
                         __FUNCTION__);
        return_value.set(InvalidParameter);
        return INVOKE_METHOD_OK;
    }

    return INVOKE_METHOD_OK;
}

#ifdef TARGET_CIM_SERVER_esxi
Invoke_Method_Status
    SF_SoftwareInstallationService_Provider::GetRequiredFwImageName(
        const SF_SoftwareInstallationService* self,
        const CIM_Card* Target,
        Property<uint32>& Type,
        Property<uint32>& Subtype,
        Property<String>& Name,
        Property<String>& CurrentVersion,
        Property<uint32>& return_value)
{
    using namespace solarflare;

    UpdatedFirmwareType fwType;

    enum ReturnValue 
    {
        OK = 0,
        Error = 2,
        InvalidParameter = 5,
    };

    if (Target == NULL)
    {
        PROVIDER_LOG_ERR("%s(): Target parameter should be specified",
                         __FUNCTION__);
        return_value.set(InvalidParameter);
        return INVOKE_METHOD_OK;
    }

    if (self == NULL || self->Name.null)
    {
        PROVIDER_LOG_ERR("%s(): Name property of "
                         "SF_SoftwareInstallationService is not known",
                         __FUNCTION__);
        return_value.set(Error);
        return INVOKE_METHOD_OK;
    }

    if (strcmp(self->Name.value.c_str(), "BootROM") == 0)
        fwType = FIRMWARE_BOOTROM;
    else if (strcmp(self->Name.value.c_str(), "Firmware") == 0)
        fwType = FIRMWARE_MCFW;
    else
    {
        PROVIDER_LOG_ERR("%s(): operation is not supported for "
                         "SF_SoftwareInstallationService.Name=%s",
                         __FUNCTION__, self->Name.value.c_str());
        return_value.set(Error);
        return INVOKE_METHOD_OK;
    }

    FwImgInfoGetter getter(fwType, Target);

    getter.forNIC();

    if (!getter.nicFound())
    {
        PROVIDER_LOG_ERR("%s(): failed to find NIC", __FUNCTION__);
        return_value.set(Error);
        return INVOKE_METHOD_OK;
    }
    else if (!getter.isOk())
    {
        return_value.set(Error);
        return INVOKE_METHOD_OK;
    }

    Type.null = false;
    Type.value = getter.getImgType();
    Subtype.null = false;
    Subtype.value = getter.getImgSubType();
    Name.null = false;
    Name.value = getter.getImgName();
    CurrentVersion.null = false;
    CurrentVersion.value = getter.getCurVersion().string();

    return_value.set(OK);
    return INVOKE_METHOD_OK;
}

Invoke_Method_Status
    SF_SoftwareInstallationService_Provider::StartFwImageSend(
        const SF_SoftwareInstallationService* self,
        Property<String>& FileName,
        Property<uint32>& return_value)
{
    enum ReturnValue 
    {
        OK = 0,
        Error = 2,
    };

    String fName = solarflare::System::target.createTmpFile();

    if (self->Name.null ||
        (strcmp(self->Name.value.c_str(), "BootROM") != 0 &&
         strcmp(self->Name.value.c_str(), "Firmware") != 0))
    {
        PROVIDER_LOG_ERR("%s() is not supported for this "
                         "SF_SoftwareInstallationService instance",
                         __FUNCTION__);
        return_value.set(Error);
        return INVOKE_METHOD_OK;
    }

    if (fName.empty())
    {
        FileName.null = true;
        return_value.set(Error);
    }
    else
    {
        FileName.null = false;
        FileName.value = fName;
        return_value.set(OK);
    }

    return INVOKE_METHOD_OK;
}

Invoke_Method_Status
    SF_SoftwareInstallationService_Provider::SendFwImageData(
        const SF_SoftwareInstallationService* self,
        const Property<String>& FileName,
        const Property<String>& Base64Str,
        Property<uint32>& return_value)
{
    enum ReturnValue 
    {
        OK = 0,
        Error = 2,
        InvalidParameter = 5,
    };

    if (self->Name.null ||
        (strcmp(self->Name.value.c_str(), "BootROM") != 0 &&
         strcmp(self->Name.value.c_str(), "Firmware") != 0))
    {
        PROVIDER_LOG_ERR("%s() is not supported for this "
                         "SF_SoftwareInstallationService instance",
                         __FUNCTION__);
        return_value.set(Error);
        return INVOKE_METHOD_OK;
    }

    if (FileName.null || Base64Str.null)
    {
        return_value.set(InvalidParameter);
        return INVOKE_METHOD_OK;
    }

    if (solarflare::System::
              target.tmpFileBase64Append(FileName.value,
                                         Base64Str.value) < 0)
        return_value.set(Error);
    else
        return_value.set(OK);

    return INVOKE_METHOD_OK;
}

Invoke_Method_Status
    SF_SoftwareInstallationService_Provider::RemoveFwImage(
        const SF_SoftwareInstallationService* self,
        const Property<String>& FileName,
        Property<uint32>& return_value)
{
    enum ReturnValue 
    {
        OK = 0,
        Error = 2,
        InvalidParameter = 5,
    };

    if (FileName.null)
    {
        return_value.set(InvalidParameter);
        return INVOKE_METHOD_OK;
    }

    if (solarflare::System::
              target.removeTmpFile(FileName.value) < 0)
        return_value.set(Error);
    else
        return_value.set(OK);

    return INVOKE_METHOD_OK;
}

Invoke_Method_Status
    SF_SoftwareInstallationService_Provider::GetLocalFwImageVersion(
        const SF_SoftwareInstallationService* self,
        const CIM_Card* Target,
        const Property<String>& FileName,
        Property<boolean>& ApplicableFound,
        Property<String>& Version,
        Property<uint32>& return_value)
{
    using namespace solarflare;

    UpdatedFirmwareType fwType;

    enum ReturnValue 
    {
        OK = 0,
        Error = 2,
        InvalidParameter = 5,
    };

    if (Target == NULL || FileName.null || FileName.value.empty())
    {
        PROVIDER_LOG_ERR("%s(): Target and FileName parameters "
                         "should be specified",
                         __FUNCTION__);
        return_value.set(InvalidParameter);
        return INVOKE_METHOD_OK;
    }

    if (self == NULL || self->Name.null)
    {
        PROVIDER_LOG_ERR("%s(): Name property of "
                         "SF_SoftwareInstallationService is not known",
                         __FUNCTION__);
        return_value.set(Error);
        return INVOKE_METHOD_OK;
    }

    if (strcmp(self->Name.value.c_str(), "BootROM") == 0)
        fwType = FIRMWARE_BOOTROM;
    else if (strcmp(self->Name.value.c_str(), "Firmware") == 0)
        fwType = FIRMWARE_MCFW;
    else
    {
        PROVIDER_LOG_ERR("%s(): operation is not supported for "
                         "SF_SoftwareInstallationService.Name=%s",
                         __FUNCTION__, self->Name.value.c_str());
        return_value.set(Error);
        return INVOKE_METHOD_OK;
    }

    LocalFwImgChecker checker(fwType, FileName.value, Target);

    checker.forNIC();

    if (!checker.nicFound())
    {
        PROVIDER_LOG_ERR("%s(): failed to find NIC", __FUNCTION__);
        return_value.set(Error);
        return INVOKE_METHOD_OK;
    }
    else if (!checker.isOk())
    {
        return_value.set(Error);
        return INVOKE_METHOD_OK;
    }

    ApplicableFound.null = false;
    ApplicableFound.value = checker.getApplicable();
    Version.null = false;
    Version.value = checker.getImgVersion().string();

    return_value.set(OK);
    return INVOKE_METHOD_OK;
}

#endif

Invoke_Method_Status SF_SoftwareInstallationService_Provider::ChangeAffectedElementsAssignedSequence(
    const SF_SoftwareInstallationService* self,
    const Property< Array<CIM_ManagedElement*> >& ManagedElements,
    const Property<Array_uint16>& AssignedSequence,
    CIM_ConcreteJob*& Job,
    Property<uint32>& return_value)

{
    return INVOKE_METHOD_UNSUPPORTED;
}

Invoke_Method_Status SF_SoftwareInstallationService_Provider::InstallFromByteStream(
    const SF_SoftwareInstallationService* self,
    CIM_ConcreteJob*& Job,
    const Property<Array_uint8>& Image,
    const CIM_ManagedElement* Target,
    const Property<Array_uint16>& InstallOptions,
    const Property<Array_String>& InstallOptionsValues,
    Property<uint32>& return_value)

{
    return INVOKE_METHOD_UNSUPPORTED;
}

/*@END@*/

CIMPLE_NAMESPACE_END
