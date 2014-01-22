ifneq (,$(findstring mingw,$(PLATFORM)))
    CC=$(PLATFORM)-gcc
else
    CC=gcc
endif

SEARCH_PATH:=$(shell $(CC) --print-search-dirs | grep "programs: =" | \
                     sed "s/programs: =//" | sed "s/:/ /g")
SEARCH_PATH:="$(SEARCH_PATH) $(shell echo $$PATH | sed "s/:/ /g")"
XML2_CONFIG:=$(shell for d in "$(SEARCH_PATH)" ; do \
                       if test -f $$d/xml2-config ; then \
                         echo $$d/xml2-config ; \
                         break ; \
                       fi ; \
                     done)

ifeq (,$(XML2_CONFIG))
    $(error Failed to find xml2-config)
endif

XML2_LIBS:=$(shell $(XML2_CONFIG) --libs)
XML2_CFLAGS:=$(shell $(XML2_CONFIG) --cflags)

ifneq (,$(findstring mingw,$(PLATFORM)))
    MY_LIBS=-lcurl -lz -lpopt -lcrypto -lgdi32 \
            -lintl -lcrypt32 -lws2_32 -lncurses  \
            $(XML2_LIBS) -llzma
    MY_CFLAGS=-DCURL_STATICLIB -L./libs/windows/$(PLATFORM)/
    FILE_SUFFIX=.exe
    CPP=$(PLATFORM)-cpp
    MY_CPPFLAGS=
else
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

ifeq (0,$(shell echo "\#include <ncurses/curses.h>" | \
                $(CPP) 1>/dev/null 2>/dev/null; echo $$?))
    MY_CPPFLAGS:=$(MY_CPPFLAGS) -DNCURSES_CURSES_H
endif

sfupdate_esxi$(FILE_SUFFIX): main.c fw_images.c
	$(CC) -g $^ -o $@ $(MY_CPPFLAGS) $(MY_CFLAGS) \
	      $(XML2_CFLAGS) $(MY_LIBS)

.PHONY : fw_images.c
fw_images.c :
	./include_fw_imgs.sh $(FIRMWARE_REPO)


DISTDIR = esxi_sfupdate-1.0

DISTFILES = main.c Makefile README esxi_sfupdate_client.spec \
			fw_images.h include_fw_imgs.sh sf_base64.h \
			libs libs_includes

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
