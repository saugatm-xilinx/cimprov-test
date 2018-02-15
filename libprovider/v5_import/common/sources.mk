libcommon_PURPOSE = target
libcommon_SOURCES = ef10_image.c efx_crc32.c

libcommon_DIR = libprovider/v5_import/common

libcommon_TARGET = libcommon.a
libcommon_INCLUDES = libprovider/v5_import \
		  libprovider/v5_import/common \

$(eval $(call component,libcommon,STATIC_LIBRARIES))
