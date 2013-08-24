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
        return owner->syncInstall(filename.c_str());
    }

    void SWElement::InstallThread::update(Thread *tempThr)
    {
        owner = static_cast<InstallThread *>(tempThr)->owner;
    }


    bool SWElement::install(const char *filename, bool sync)
    {
        if (sync)
            return syncInstall(filename);
        if (installer.currentState() == Thread::Running ||
            installer.currentState() == Thread::Aborting)
            return false;
        installer.setFilename(filename);
        installer.start();
        return true;
    }

    String BusElement::name() const
    {
        Buffer buf;
        buf.appends(SystemElement::name().c_str());
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
} // namespace

