libcimplewmi_SOURCES = WMI_Containers.cpp \
    WMI_Thread_Context.cpp \
    WMI_Converter.cpp \
    WMI_Adapter.cpp \
    WMI_Adapter_Factory.cpp \
	WMI_Ref.cpp \
	utils.cpp \
	SafeArr.cpp

libcimplewmi_TARGET = libcimplewmi.a
libcimplewmi_DIR = cimple/wmi
libcimplewmi_INCLUDES = cimple/wmi
libcimplewmi_DEPENDS = libcimple

$(eval $(call component,libcimplewmi,STATIC_LIBRARIES))

CIMPLE_COMPONENTS := libcimplewmi $(CIMPLE_COMPONENTS)
