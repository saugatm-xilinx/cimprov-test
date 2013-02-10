pegasus_SOURCES = \
    Pegasus_Adapter.cpp \
    Pegasus_Thread_Context.cpp \
    pegasus_unsupported.cpp \
	Containers.cpp \
	Converter.cpp \
	print.cpp

lib$(PROVIDER_LIBRARY)_SOURCES += $(addprefix cimple/pegasus/,$(pegasus_SOURCES))
