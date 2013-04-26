libcimple_PURPOSE = target
libcimple_SOURCES = \
    Atomic.cpp \
    Cond_Queue.cpp \
    Exception.cpp \
    boolean.cpp \
    CimpleConfig.cpp \
    real.cpp \
    Arg.cpp \
    Ops.cpp \
    Instance_Hnd.cpp \
    Instance_Ref.cpp \
    log.cpp \
    File_Lock.cpp \
    integer.cpp \
    Value.cpp \
    Container.cpp \
    Buffer.cpp \
    octets.cpp \
    config.cpp \
    ptr_array.cpp \
    io.cpp \
    flags.cpp \
    Meta_Value.cpp \
    Meta_Repository.cpp \
    Meta_Qualifier.cpp \
    Meta_Reference.cpp \
    Thread_Context.cpp \
    cimom.cpp \
    Cond.cpp \
    Error.cpp \
    Mutex.cpp \
    Time.cpp \
    Thread.cpp \
    Auto_Mutex.cpp \
    Provider_Handle.cpp \
    Array_Impl.cpp \
    cimple.cpp \
    Datetime.cpp \
    Instance.cpp \
    Instance_Map.cpp \
    Meta_Feature.cpp \
    Meta_Class.cpp \
    Meta_Method.cpp \
    Meta_Property.cpp \
    Registration.cpp \
    String.cpp \
    Strings.cpp \
    Type.cpp \
    Property.cpp \
    TSD.cpp \
    FileSystem.cpp

libcimple_DIR = cimple/lib
libcimple_INCLUDES = cimple

libcimple_PROVIDE_CPPFLAGS = -DCIMPLE_DEBUG 
libcimple_PROVIDE_CPPFLAGS += -DCIMPLE_PLATFORM_$(CIMPLE_PLATFORM)

libcimple_CPPFLAGS = $(libcimple_PROVIDE_CPPFLAGS) -DCIMPLE_BUILDING_LIBCIMPLE

libcimple_TARGET = libcimple.a

ifeq ($(CIM_SERVER),wmi)
libcimple_DEPENDS = libcimpleposix
endif

$(eval $(call component,libcimple,STATIC_LIBRARIES))

CIMPLE_COMPONENTS += libcimple

libcimplehost_PURPOSE = host

libcimplehost_SOURCES = $(libcimple_SOURCES)
libcimplehost_GENERATED = $(libcimplehost_SOURCES)

libcimplehost_DIR = cimple/libhost

libcimplehost_INCLUDES = $(libcimple_INCLUDES)
libcimplehost_PROVIDE_CPPFLAGS = -DCIMPLE_DEBUG -DCIMPLE_PLATFORM_$(HOST_CIMPLE_PLATFORM)
libcimplehost_CPPFLAGS = $(libcimplehost_PROVIDE_CPPFLAGS) -DCIMPLE_BUILDING_LIBCIMPLE

libcimplehost_TARGET = libcimplehost.a

$(eval $(call component,libcimplehost,STATIC_LIBRARIES))

$(libcimplehost_DIR)/%.cpp : $(libcimple_DIR)/%.cpp
	mkdir -p $(dir $@)
	cp $< $@

