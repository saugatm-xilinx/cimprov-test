#include "sf_core.h"

#if defined(linux)
#include <unistd.h>
#endif

namespace solarflare 
{
    String BusElement::name() const
    {
        Buffer buf;
        buf.appends(SystemElement::name().c_str());
        buf.append(' ');
        buf.append_uint16(elementId());
        return buf.data();
    }
} // namespace

