#include "sf_utils.h"

#if defined(linux)
#include <unistd.h>
#endif

namespace solarflare 
{
    const unsigned VersionInfo::unknown = unsigned(-1);

    VersionInfo::VersionInfo(const char *s) :
        vmajor(0), vminor(0), revisionNo(unknown), buildNo(unknown)
    {
        char *s0 = const_cast<char *>(s);
        vmajor = strtoul(s0, &s0, 10);
        if (*s0 == '.')
        {
            vminor = strtoul(s0 + 1, &s0, 10);
            if (*s0 == '.' || *s0 == '-')
            {
                revisionNo = strtoul(s0 + 1, &s0, 10);
                if (*s0 == '.' || *s0 == '-')
                    buildNo = strtoul(s0 + 1, NULL, 10);
            }
        }
        versionStr = String(s);
    }
    

    String VersionInfo::string() const
    {
        if (versionStr.size() > 0)
            return versionStr;
        
        Buffer result;
        
        result.append_uint32(vmajor);
        result.append('.');
        result.append_uint32(vminor);
        if (revisionNo != unknown)
        {
            result.append('.');
            result.append_uint32(revisionNo);
        }
        if (buildNo != unknown)
        {
            result.append('.');
            result.append_uint32(buildNo);
        }
        return result.data();
    }

    const String& VitalProductData::manufacturer() const 
    {
        return (manufac.size() > 0 ? manufac : 
                System::target.manufacturer());
    }


    String MACAddress::string() const
    {
        char str[sizeof(address) * 2 + 1];
        sprintf(str, "%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
                address[0], address[1], address[2],
                address[3], address[4], address[5]);
        return str;
    }

    const unsigned PCIAddress::unknown = unsigned(-1);
}
