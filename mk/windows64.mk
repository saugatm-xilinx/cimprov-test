##########################################################################
##! \file ./mk/windows64.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  Common configuration for 64-bit Windows
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################
## <example>
override target_CXX = x86_64-w64-mingw32-g++
override target_AR = x86_64-w64-mingw32-ar 
override target_CPPFLAGS = -I$(TOP) -DSF_IMPLEMENTATION_NS=\"$(IMP_NAMESPACE)\" -DSF_INTEROP_NS=\"$(INTEROP_NAMESPACE)\" 
override target_CXXFLAGS = -Wall -W -Wno-unused -Werror -g -fms-extensions -Wno-unknown-pragmas -std=gnu++98
override target_LDFLAGS = -static-libgcc -static-libstdc++ -Wl,--kill-at
override target_SYSLIBRARIES = version kernel32 user32 advapi32 iphlpapi ole32 oleaut32 uuid
override WINDRES = x86_64-w64-mingw32-windres
override target_STRIP = x86_64-w64-mingw32-strip

override CIMPLE_PLATFORM = WIN64_X86_64_MSVC
SOEXT = dll
## </example>