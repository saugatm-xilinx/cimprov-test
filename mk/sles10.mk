override CIM_SERVER=pegasus
override CIM_INTERFACE=pegasus
override CIMPLE_PLATFORM=$(HOST_CIMPLE_PLATFORM)
override PEGASUS_ROOT=/tmp/pegasus
override PEGASUS_HOME=/tmp/pegasus
override PROVIDER_ROOT=/tmp/solarflare
override CIM_SCHEMA_DIR = $(abspath $(CIM_SCHEMA_PATCHDIR))
override PROVIDER_PLATFORM = linux
override PROVIDER_PLATFORM_VARIANT = sles10
RUNASROOT = sudo 
CURL = curl -u$(LOGNAME)