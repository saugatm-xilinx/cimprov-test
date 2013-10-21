libcimplewmi_SOURCES = utils.cpp \
	SafeArr.cpp
ifeq ($(CIM_INTERFACE),wmi)
libcimplewmi_SOURCES += WMI_Containers.cpp \
    WMI_Thread_Context.cpp \
    WMI_Converter.cpp \
    WMI_Adapter.cpp \
    WMI_Adapter_Factory.cpp \
	WMI_Ref.cpp
endif

libcimplewmi_TARGET = libcimplewmi.a
libcimplewmi_DIR = cimple/wmi
libcimplewmi_INCLUDES = cimple/wmi
ifeq ($(CIM_INTERFACE),wmi)
libcimplewmi_DEPENDS = libcimple
endif
libcimplewmi_CPPFLAGS = $(libcimplewmi_PROVIDE_CPPFLAGS) 
ifeq ($(CIM_INTERFACE),wmi)
libcimplewmi_CPPFLAGS += -DCIMPLE_WMI_MODULE
else
libcimplewmi_CPPFLAGS += $(libcimple_PROVIDE_CPPFLAGS)
endif

$(eval $(call component,libcimplewmi,STATIC_LIBRARIES))

CIMPLE_COMPONENT += libcimplewmi 
