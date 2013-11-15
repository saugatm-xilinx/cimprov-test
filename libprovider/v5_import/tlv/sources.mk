libtlv_PURPOSE = target
libtlv_SOURCES = tlv.c tlv_partition.c

libtlv_DIR = libprovider/v5_import/tlv

libtlv_TARGET = libtlv.a
libtlv_INCLUDES = libprovider/v5_import \
		  libprovider/v5_import/tlv \
		  libprovider/v5_import/endianness \
		  libprovider/v5_import/ci/mgmt

libtlv_CFLAGS = -fPIC

$(eval $(call component,libtlv,STATIC_LIBRARIES))
