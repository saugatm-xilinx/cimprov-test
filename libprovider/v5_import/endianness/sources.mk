libendianness_PURPOSE = target
libendianness_SOURCES = endian.c

libendianness_DIR = libprovider/v5_import/endianness

libendianness_TARGET = libendianness.a
libendianness_INCLUDES = libprovider/v5_import \
		  libprovider/v5_import/endianness \

$(eval $(call component,libendianness,STATIC_LIBRARIES))
