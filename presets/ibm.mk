override CIM_SERVER=pegasus
override CIM_INTERFACE=pegasus
override CIMPLE_PLATFORM=LINUX_IX86_GNU
override PEGASUS_ROOT=/usr
override PEGASUS_HOME=/opt/ibm/icc
override CIM_SCHEMA_DIR = $(abspath $(CIM_SCHEMA_PATCHDIR))
CURL = curl