override SLES10_BUILD_HOST=ibm360m4b
override CIM_SERVER=pegasus
override CIM_INTERFACE=pegasus
override HOST_CIMPLE_PLATFORM=LINUX_X86_64_GNU
override CIMPLE_PLATFORM=$(HOST_CIMPLE_PLATFORM)
override PEGASUS_ROOT=/usr
override PEGASUS_HOME=/opt/ibm/icc
override PROVIDER_ROOT=/opt/solarflare
override CIM_SCHEMA_DIR = $(abspath $(CIM_SCHEMA_PATCHDIR))
override PROVIDER_PLATFORM = linux
RUNASROOT = sudo 
CURL = curl -u$(LOGNAME)
