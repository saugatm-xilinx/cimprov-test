##########################################################################
##! \file ./mk/pegasus-win.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  Common configuration for OpenPegasus on Windows
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

override CIM_SCHEMA_VERSION_MINOR=28
override CIM_SCHEMA_VERSION_EXPERIMENTAL=
override CIM_SERVER = pegasus
override CIM_INTERFACE = cmpi
override PROVIDER_PLATFORM = windows
override INTREE_PEGASUS = 1
override PEGASUS_SERVER = $(PEGASUS_BINPATH)/cimserver.exe
override PEGASUS_PREBUILT = 1
override PEGASUS_UPSTREAM_VERSION = winbuild
override PEGASUS_SETUP_SUBDIR =
override USE_REGMOD = 
