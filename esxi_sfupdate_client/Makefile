define check_func
$(strip
$(eval TMPIN=$(shell mktemp --suffix=.c)) \
$(eval TMPOUT=$(shell mktemp)) \
$(shell echo "#include <stdio.h>" >$(TMPIN); \
		echo "#include <stdlib.h>" >>$(TMPIN); \
		echo "int main() { " >>$(TMPIN); \
		echo "void *p = (void *)$(1); return 0; }" \
		>>$(TMPIN); \
		$(CC) $(MY_CPPFLAGS) $(TMPIN) -o $(TMPOUT) 2>/dev/null; \
		echo $$? ; \
		rm $(TMPIN); rm $(TMPOUT)))
endef

VSNPRINTF_FIX=
GETENV_FIX=
MY_CPPFLAGS:=-D_GNU_SOURCE

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
    MY_CPPFLAGS:=$(MY_CPPFLAGS) -I./libs_includes/windows/$(PLATFORM)/

    ifeq (0,$(call check_func,__ms_vsnprintf))
        VSNPRINTF_FIX=vsnprintf_fix.c
    endif
else
    CC=gcc
    XML2_LIBS=-lxml2
    XML2_CFLAGS=
    MY_LIBS= -Wl,-Bstatic -lpopt -lncurses -ltinfo $(XML2_LIBS) -llzma \
             -lcurl -lssl -lcrypto -lz -lm -lrt -Wl,-Bdynamic -ldl
    ifeq (,$(shell file /bin/bash | grep 'x86-64'))
        MY_CFLAGS=-L./libs/linux/32/
        MY_CPPFLAGS:=$(MY_CPPFLAGS) -I./libs_includes/linux/32/
    else
        MY_CFLAGS=-L./libs/linux/64/
        MY_CPPFLAGS:=$(MY_CPPFLAGS) -I./libs_includes/linux/64/
    endif
    FILE_SUFFIX=
    CPP=cpp

    ifneq (0,$(call check_func,__secure_getenv))
        GETENV_FIX=secure_getenv_fix.c
    endif
    ifeq (0,$(call check_func,secure_getenv))
        MY_CPPFLAGS:=$(MY_CPPFLAGS) -DSECURE_GETENV_FOUND=1
    endif
endif
MY_CPPFLAGS:=$(MY_CPPFLAGS) -DLIBXML_STATIC

ifeq (0,$(shell echo "\#include <ncurses/curses.h>" | \
                $(CPP) $(MY_CPPFLAGS) 1>/dev/null 2>/dev/null; echo $$?))
    MY_CPPFLAGS:=$(MY_CPPFLAGS) -DNCURSES_CURSES_H
endif

GCC_OPTS=
ifeq (1,$(shell gcc -no-pie 2>&1 | grep -i "unrecognized"; echo $$?))
    # This is necessary because otherwise gcc may think it builds
    # a shared library and fail because -fPIC was not used when
    # building some of the static libraries to which this program
    # links.
    GCC_OPTS=-no-pie
endif

sfupdate_esxi$(FILE_SUFFIX): main.c fw_images.c cim_client.c \
  			curl_aux.c $(VSNPRINTF_FIX) $(GETENV_FIX)
	$(CC) $(GCC_OPTS) -g main.c fw_images.c cim_client.c curl_aux.c \
	      $(VSNPRINTF_FIX) $(GETENV_FIX) -o $@ $(MY_CPPFLAGS) \
	      $(MY_CFLAGS) $(XML2_CFLAGS) $(MY_LIBS)

.PHONY : fw_images.c
fw_images.c :
	./include_fw_imgs.sh $(FIRMWARE_REPO)


DISTDIR = esxi_sfupdate-1.0

DISTFILES = main.c vsnprintf_fix.c secure_getenv_fix.c curl_aux.c curl_aux.h \
	    		cim_client.c cim_client.h Makefile README \
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
