VSNPRINTF_FIX=
ifneq (,$(findstring mingw,$(PLATFORM)))
    CC=$(PLATFORM)-gcc
    XML2_LIBS=-lxml2 -lz -liconv -lws2_32
    XML2_CFLAGS=
    MY_LIBS=-lcurl -lz -lpopt -lcrypto -lgdi32 \
            -lintl -lcrypt32 -lws2_32 -lncurses  \
            $(XML2_LIBS) -llzma
    MY_CFLAGS=-DCURL_STATICLIB -L./libs/windows/$(PLATFORM)/
    FILE_SUFFIX=.exe
    CPP=$(PLATFORM)-cpp
    MY_CPPFLAGS=-I./libs_includes/windows/$(PLATFORM)/

    TMPIN:=$(shell mktemp --suffix=.c)
    TMPOUT:=$(shell mktemp)
    RESULT:=$(shell echo "\#include <stdio.h>" >$(TMPIN); \
                    echo "\#include <stdlib.h>" >>$(TMPIN); \
                    echo "int main() { " >>$(TMPIN); \
                    echo "void *p = (void *)__ms_vsnprintf; return 0; }" \
                    >>$(TMPIN); \
                    $(CC) $(TMPIN) -o $(TMPOUT) 2>/dev/null; \
                    echo $$?; \
                    rm $(TMPIN); rm $(TMPOUT))

    ifneq (0,$(RESULT))
        VSNPRINTF_FIX=vsnprintf_fix.o
    endif
else
    CC=gcc
    XML2_LIBS=-lxml2
    XML2_CFLAGS=
    MY_LIBS= -Wl,-Bstatic -lpopt -lncurses -ltinfo $(XML2_LIBS) -llzma \
             -lcurl -lssl -lcrypto -lz -lm -lrt -Wl,-Bdynamic -ldl
    ifeq (,$(shell file /bin/bash | grep 'x86-64'))
        MY_CFLAGS=-L./libs/linux/32/
        MY_CPPFLAGS=-I./libs_includes/linux/32/
    else
        MY_CFLAGS=-L./libs/linux/64/
        MY_CPPFLAGS=-I./libs_includes/linux/64/
    endif
    FILE_SUFFIX=
    CPP=cpp
endif
MY_CPPFLAGS:=$(MY_CPPFLAGS) -DLIBXML_STATIC
TOP ?= .
V5IMPORT_DIR             = common
V5IMPORT_URL             = http://source.uk.solarflarecom.com/hg/incoming/v5/rawfile/default/src/driver/common/
V5IMPORT_IMGFILE	 = ef10_image.c
V5IMPORT_CRCFILE	 = efx_crc32.c
V5IMPORT_EF10_SIMGFILE	 = ef10_signed_image_layout.h

ifeq (0,$(shell echo "\#include <ncurses/curses.h>" | \
                $(CPP) $(MY_CPPFLAGS) 1>/dev/null 2>/dev/null; echo $$?))
    MY_CPPFLAGS:=$(MY_CPPFLAGS) -DNCURSES_CURSES_H
endif


sfupdate_esxi$(FILE_SUFFIX): main.c fw_images.c cim_client.c \
  			curl_aux.c $(VSNPRINTF_FIX)
	rm -f $(TOP)/$(V5IMPORT_DIR)/$(V5IMPORT_IMGFILE)
	rm -f $(TOP)/$(V5IMPORT_DIR)/$(V5IMPORT_CRCFILE)
	rm -f $(TOP)/$(V5IMPORT_DIR)/$(V5IMPORT_EF10_SIMGFILE)
	wget -q -P $(TOP)/$(V5IMPORT_DIR)  $(V5IMPORT_URL)/$(V5IMPORT_IMGFILE)
	wget -q -P $(TOP)/$(V5IMPORT_DIR)  $(V5IMPORT_URL)/$(V5IMPORT_CRCFILE)
	wget -q -P $(TOP)/$(V5IMPORT_DIR)  $(V5IMPORT_URL)/$(V5IMPORT_EF10_SIMGFILE)

	$(CC) -g main.c fw_images.c cim_client.c curl_aux.c common/ef10_image.c common/efx_crc32.c\
	      -o $@ $(MY_CPPFLAGS) $(MY_CFLAGS) \
	      $(XML2_CFLAGS) $(MY_LIBS) $(VSNPRINTF_FIX)
	if test x != x$(VSNPRINTF_FIX) ; then rm $(VSNPRINTF_FIX); fi

.PHONY : fw_images.c
fw_images.c :
	./include_fw_imgs.sh $(FIRMWARE_REPO)


DISTDIR = esxi_sfupdate-1.0

DISTFILES = main.c vsnprintf_fix.c curl_aux.c curl_aux.h common\efx.h common\efsys.h\
			common\efx_impl.h cim_client.c cim_client.h Makefile README \
	    		esxi_sfupdate_client.spec \
			fw_images.h include_fw_imgs.sh sf_base64.h \
			cim_log.h libs libs_includes

.PHONY : clean
clean:
	rm -rf *.o sfupdate_esxi$(FILE_SUFFIX) $(DISTDIR) $(DISTDIR).tar.gz

.PHONY : dist
dist: $(DISTDIR).tar.gz

$(DISTDIR).tar.gz : $(DISTFILES)
	rm -rf $(DISTDIR)
	mkdir -p $(DISTDIR)
	cp -a $^ $(DISTDIR)
	tar czf $@ $(DISTDIR)

MSI_BITNESS = $(if $(findstring x86_64,$(PLATFORM)),64,32)
MSI_VERSION = 1.0.0.0
MSI_VENDOR = Solarflare Communications
MSI_TOOLNAME = remote sfupdate for ESXi
MSI_TOOLDESC = $(MSI_TOOLNAME)
MSI_INSTALLER = sfupdate_esxi-$(MSI_VERSION)-windows$(MSI_BITNESS)$(FILE_SUFFIX)

NSIS = makensis

.PHONY : msi 

msi : $(MSI_INSTALLER)

$(MSI_INSTALLER) : sfupdate.nsi sfupdate_esxi$(FILE_SUFFIX) Makefile
	$(NSIS) -DBITNESS=$(MSI_BITNESS) -DINSTALLERNAME=$@ -DTOOLNAME="$(MSI_TOOLNAME)" \
			-DTOOLDESC="$(MSI_TOOLDESC)" -DVENDORNAME="$(MSI_VENDOR)" -DTOOLFILE=$(word 2,$^) -DTOP=.. $<
