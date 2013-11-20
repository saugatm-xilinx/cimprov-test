#ifndef LOG_FILE_PATH_H
#define LOG_FILE_PATH_H 1

#include <string.h>

namespace solarflare 
{
    ///
    /// @return Length of the initial part of file path that
    ///         should be excluded when logging
    ///
    extern unsigned getCommonPathLen();
} // namespace

/// Current file path relative to the top source folder
#define __FILE_REL__ \
    (strlen(__FILE__) < solarflare::getCommonPathLen() ? \
          __FILE__ : (__FILE__ + solarflare::getCommonPathLen()))

#endif   // LOG_FILE_PATH_H
