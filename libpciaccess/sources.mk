##########################################################################
##! \file ./libpciaccess/sources.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/11/29
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

libpciaccess_PURPOSE = target

# These variables are platform dependent
# Currently we support only Linux though

PCIACCESS_OS_SUPPORT = linux_sysfs.c linux_devmem.c
PCIACCESS_VGA_ARBITER = common_vgaarb.c

PCIIDS_PATH ?= /usr/share/hwdata

libpciaccess_SOURCES = common_bridge.c \
	common_iterator.c \
	common_init.c \
	common_interface.c \
	common_io.c \
	common_capability.c \
	common_device_name.c \
	common_map.c \
	$(PCIACCESS_VGA_ARBITER) \
	$(PCIACCESS_OS_SUPPORT)


libpciaccess_DIR = libpciaccess

libpciaccess_TARGET = libpciaccess.a

libpciaccess_INCLUDES = $(libpciaccess_DIR)

libpciaccess_CPPFLAGS = $(libpciaccess_PROVIDE_CPPFLAGS) \
			-DHAVE_INTTYPES_H=1 -DHAVE_MTRR=1 \
			-DHAVE_STDINT_H=1 -DHAVE_STRINGS_H=1 \
			-DHAVE_STRING_H=1 -DPCIIDS_PATH=\"$(PCIIDS_PATH)\"

$(eval $(call component,libpciaccess,STATIC_LIBRARIES))
