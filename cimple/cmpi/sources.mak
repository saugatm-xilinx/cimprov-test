cmpi_SOURCES = CMPI_Containers.cpp \
    CMPI_Thread_Context.cpp \
    Converter.cpp \
    CMPI_Adapter.cpp

lib$(PROVIDER_LIBRARY)_SOURCES += $(addprefix cimple/cmpi/,$(cmpi_SOURCES))
