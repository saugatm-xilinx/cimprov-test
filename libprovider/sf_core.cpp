/***************************************************************************//*! \file liprovider/sf_core.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include "sf_core.h"
#include "sf_platform.h"
#include "sf_provider.h"

#if defined(linux)
#include <unistd.h>
#endif

namespace solarflare
{
    Thread *SWElement::InstallThread::dup() const
    {
        return new InstallThread(owner, id);
    }

    String SWElement::InstallThread::getThreadID() const
    {
        String tid = CIMHelper::instanceID(owner->name());
        tid.append(":installThread");
        return tid;
    }

    bool SWElement::InstallThread::threadProc()
    {
        return (owner->syncInstall(filename.c_str(), force,
                                   hash.c_str()) == Install_OK);
    }

    void SWElement::InstallThread::update(Thread *tempThr)
    {
        owner = static_cast<InstallThread *>(tempThr)->owner;
    }


    SWElement::InstallRC SWElement::install(const char *filename, bool sync,
                                            bool force,
                                            const char *base64_hash)
    {
        if (sync)
            return syncInstall(filename, force, base64_hash);
        if (installer.currentState() == Thread::Running ||
            installer.currentState() == Thread::Aborting)
            return Install_Error;
        installer.setFilename(filename);
        installer.setForce(force);
        installer.setHash(base64_hash);
        installer.start();
        return Install_OK;
    }

    String OrderedElement::name(const String& prefix) const
    {
        Buffer buf;
        buf.appends(prefix.c_str());
        buf.append(' ');
        buf.append_uint16(elementId());
        return buf.data();
    }

    String Firmware::name() const 
    {
        String n = nic()->name();

        n.append(" ");
        n.append(genericName());
        return n;
    }

    SWType *SWElement::getSWType() const
    {
        SWType *type = new SWType(this->description(),
                                  this->sysName(),
                                  this->genericName(),
                                  this->classify());

        return type;
    }

    String Firmware::sysName() const
    {
        String part;

        if (nic() == NULL)
            return String("");

        part = nic()->productNumber();

#if defined(TARGET_CIM_SERVER_esxi_native)
        return part;
#else
        if (strcmp(part.c_str(), "SFN5162F") == 0)
            return String("47C9955");
        else if (strcmp(part.c_str(), "SFN6122F") == 0)
            return String("47C9963");
        else
            return String("unknown");
#endif
    }

    const String DiagSWElement::diagSwGenName = "Diagnostic";

    String NICElement::name() const
    {
        Buffer      buf;
        const NIC  *nic = this->nic(); 

        if (nic != NULL)
        {
            buf.appends(nic->name().c_str());
            buf.append(' ');
        }

        buf.appends(genericName().c_str());
        buf.append(' ');
        buf.append_uint16(elementId());

        return buf.data();
    }

} // namespace

