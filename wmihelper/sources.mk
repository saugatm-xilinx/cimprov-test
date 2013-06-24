$(error This module is not working)

ifeq ($(SF_V5_PATH),)
$(error No path to Solarflare sources)
endif
ifeq ($(ATL_INCLUDES),)
$(error Unable to find ATL include files)
endif

libwmihelper_SOURCES = autogen.cpp \
        Debug.cpp \
        efxhost.cpp \
        utils.cpp \
        WmiInterface.cpp \
        Win32NetworkAdapter.cpp \
        Win32networkadapterconfiguration.cpp \
        Win32networkprotocol.cpp \
        Win32operatingsystem.cpp

libwmihelper_TARGET = libwmihelper.a
libwmihelper_DIR = $(SF_V5_PATH)/src/lib/win/ww
libwmihelper_INCLUDES = $(libwmihelper_DIR) wmihelper $(ATL_INCLUDES)
libwmihelper_CXXFLAGS = -Wno-write-strings -fpermissive -Wno-error 
libwmihelper_PROVIDE_CPPFLAGS = -DUNICODE -D_UNICODE -D__try= -D__finally= -D_declspec=__declspec -D__except='if(0) if'

$(eval $(call component,libwmihelper,STATIC_LIBRARIES))

