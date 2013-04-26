override target_CXX = amd64-mingw32msvc-gcc
override target_AR = amd64-mingw32msvc-ar
override target_CPPFLAGS = -I. -DSF_IMPLEMENTATION_NS=\"$(IMP_NAMESPACE)\" -DSF_INTEROP_NS=\"$(INTEROP_NAMESPACE)\"
override target_CXXFLAGS = -m32 -Wall -W -Wno-unused -Werror -g -fms-extensions -Wno-unknown-pragmas -std=gnu++98
override target_LDFLAGS = 
override target_LIBRARIES = ole32 oleaut32

override CIMPLE_PLATFORM = WIN32_IX86_MSVC

CIM_SCHEMA_DIR = $(abspath schemas/windows)
