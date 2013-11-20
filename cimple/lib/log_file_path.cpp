#include "log_file_path.h"
#include <cimple/Mutex.h>
#include <cimple/Auto_Mutex.h>
#include <string.h>

namespace solarflare 
{
    using cimple::Mutex;
    using cimple::Auto_Mutex;

    unsigned getCommonPathLen()
    {
        static unsigned     commonLen = 0;
        static bool         firstTime = true;
        static const char  *path = __FILE__;
        const char         *s;

        static Mutex lock(false);

        if (!firstTime)
            return commonLen;

        Auto_Mutex guard(lock);

        s = strrchr(path, '/');
        if (s != NULL)
            commonLen = (s - path) + 1 - strlen("cimple/lib/");

        firstTime = false;
        return commonLen;
    }
}
