#include "sf_core.h"

#if defined(linux)
#include <unistd.h>
#endif

namespace solarflare 
{
    bool SWElement::InstallThread::threadProc()
    {
        return owner->syncInstall(filename.c_str());
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
} // namespace

