#include "sf_platform.h"

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

    String Firmware::name() const 
    {
        String n = nic()->name();
        
        n.append(" ");
        n.append(genericName());
        return n;
    }
    
} // namespace

