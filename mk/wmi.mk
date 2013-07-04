override target_CXX = i686-w64-mingw32-g++
override target_AR = i686-w64-mingw32-ar 
override target_CPPFLAGS = -I. -DSF_IMPLEMENTATION_NS=\"$(IMP_NAMESPACE)\" -DSF_INTEROP_NS=\"$(INTEROP_NAMESPACE)\" 
override target_CXXFLAGS = -Wall -W -Wno-unused -Werror -g -fms-extensions -Wno-unknown-pragmas -std=gnu++98
override target_LDFLAGS = -static-libgcc -static-libstdc++ -Wl,--kill-at
override target_SYSLIBRARIES = version kernel32 advapi32 iphlpapi ole32 oleaut32 uuid
override WINDRES = i686-w64-mingw32-windres

override CIMPLE_PLATFORM = WIN32_IX86_MSVC
SOEXT = dll

CIM_SCHEMA_DIR = $(abspath schemas/windows)
override CIM_SCHEMA_VERSION_MINOR = 0
