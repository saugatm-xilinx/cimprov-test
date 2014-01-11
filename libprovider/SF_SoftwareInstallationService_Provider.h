/***************************************************************************//*! \file liprovider/SF_SoftwareInstallationService_Provider.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Generated by genprov 2.0.24
#ifndef _SF_SoftwareInstallationService_Provider_h
#define _SF_SoftwareInstallationService_Provider_h

#include <cimple/cimple.h>
#include "CIM_Card.h"
#include "SF_SoftwareInstallationService.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

class SF_SoftwareInstallationService_Provider
{
    class Installer : public solarflare::ActionForAll
    {
        const char *uri;
        const char *base64_hash;
        bool ok;
        bool firstRun;
    protected:
        virtual void handler(solarflare::SystemElement& se, unsigned);
    public:
        Installer(const char *u, const Instance *inst, const char *hash) :
            solarflare::ActionForAll(inst), uri(u), ok(false),
            firstRun(true), base64_hash(hash) {}
        bool isOk() const { return ok; };
        bool installWasRun() const { return !firstRun; };
        virtual bool process(solarflare::SystemElement& el);
    };

    class NICInstaller : public solarflare::ActionForAll
    {
        const char *uri;
        const char *base64_hash;
        const SF_SoftwareInstallationService *service;
        bool ok;
        bool runInstallTried;
    protected:
        virtual void handler(solarflare::SystemElement& se, unsigned);
    public:
        NICInstaller(const char *u, 
                     const SF_SoftwareInstallationService *svc,
                     const Instance *inst,
                     const char *hash) :  
            solarflare::ActionForAll(inst), uri(u), service(svc),
            ok(false), runInstallTried(false), base64_hash(hash) {}
        bool isOk() const { return ok; };
        bool installWasRun() const { return runInstallTried; };
    };

    class FwImgInfoGetter : public solarflare::Action
    {
        unsigned int imgType;
        unsigned int imgSubType;
        String imgName;

        solarflare::UpdatedFirmwareType fw_type;
        bool ok;
        bool firstRun;
    protected:
        virtual void handler(solarflare::SystemElement& se, unsigned);
    public:
        FwImgInfoGetter(solarflare::UpdatedFirmwareType type,
                        const Instance *inst) :
            solarflare::Action(inst), fw_type(type), ok(false),
            firstRun(true) {}
        bool isOk() const { return ok; };
        bool nicFound() const { return !firstRun; };

        unsigned int getImgType() { return imgType; };
        unsigned int getImgSubType() { return imgSubType; };
        String getImgName() { return imgName; };
    };

public:

    typedef SF_SoftwareInstallationService Class;

    SF_SoftwareInstallationService_Provider();

    ~SF_SoftwareInstallationService_Provider();

    Load_Status load();

    Unload_Status unload();

    Get_Instance_Status get_instance(
        const SF_SoftwareInstallationService* model,
        SF_SoftwareInstallationService*& instance);

    Enum_Instances_Status enum_instances(
        const SF_SoftwareInstallationService* model,
        Enum_Instances_Handler<SF_SoftwareInstallationService>* handler);

    Create_Instance_Status create_instance(
        SF_SoftwareInstallationService* instance);

    Delete_Instance_Status delete_instance(
        const SF_SoftwareInstallationService* instance);

    Modify_Instance_Status modify_instance(
        const SF_SoftwareInstallationService* model,
        const SF_SoftwareInstallationService* instance);

    Invoke_Method_Status RequestStateChange(
        const SF_SoftwareInstallationService* self,
        const Property<uint16>& RequestedState,
        CIM_ConcreteJob*& Job,
        const Property<Datetime>& TimeoutPeriod,
        Property<uint32>& return_value);

    Invoke_Method_Status StartService(
        const SF_SoftwareInstallationService* self,
        Property<uint32>& return_value);

    Invoke_Method_Status StopService(
        const SF_SoftwareInstallationService* self,
        Property<uint32>& return_value);

    Invoke_Method_Status CheckSoftwareIdentity(
        const SF_SoftwareInstallationService* self,
        const CIM_SoftwareIdentity* Source,
        const CIM_ManagedElement* Target,
        const CIM_Collection* Collection,
        Property<Array_uint16>& InstallCharacteristics,
        Property<uint32>& return_value);

    Invoke_Method_Status InstallFromSoftwareIdentity(
        const SF_SoftwareInstallationService* self,
        CIM_ConcreteJob*& Job,
        const Property<Array_uint16>& InstallOptions,
        const Property<Array_String>& InstallOptionsValues,
        const CIM_SoftwareIdentity* Source,
        const CIM_ManagedElement* Target,
        const CIM_Collection* Collection,
        Property<uint32>& return_value);

#ifdef TARGET_CIM_SERVER_esxi
    Invoke_Method_Status GetRequiredFwImageName(
        const SF_SoftwareInstallationService* self,
        const CIM_Card* Target,
        Property<uint32>& type,
        Property<uint32>& subtype,
        Property<String>& name,
        Property<uint32>& return_value);

    Invoke_Method_Status StartFwImageSend(
        const SF_SoftwareInstallationService* self,
        Property<String>& file_name,
        Property<uint32>& return_value);

    Invoke_Method_Status SendFwImageData(
        const SF_SoftwareInstallationService* self,
        const Property<String>& file_name,
        const Property<String>& base64_str,
        Property<uint32>& return_value);

    Invoke_Method_Status RemoveFwImage(
        const SF_SoftwareInstallationService* self,
        const Property<String>& file_name,
        Property<uint32>& return_value);
#endif

    Invoke_Method_Status InstallFromURI(
        const SF_SoftwareInstallationService* self,
        CIM_ConcreteJob*& Job,
        const Property<String>& URI,
        const CIM_ManagedElement* Target,
        const Property<Array_uint16>& InstallOptions,
        const Property<Array_String>& InstallOptionsValues,
        Property<uint32>& return_value);

    Invoke_Method_Status ChangeAffectedElementsAssignedSequence(
        const SF_SoftwareInstallationService* self,
        const Property< Array<CIM_ManagedElement*> >& ManagedElements,
        const Property<Array_uint16>& AssignedSequence,
        CIM_ConcreteJob*& Job,
        Property<uint32>& return_value);

    Invoke_Method_Status InstallFromByteStream(
        const SF_SoftwareInstallationService* self,
        CIM_ConcreteJob*& Job,
        const Property<Array_uint8>& Image,
        const CIM_ManagedElement* Target,
        const Property<Array_uint16>& InstallOptions,
        const Property<Array_String>& InstallOptionsValues,
        Property<uint32>& return_value);

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_SoftwareInstallationService_Provider_h */
