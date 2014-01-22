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

    TMPFILE:=$(shell mktemp)
    RESULT:=$(shell echo "\#include <stdio.h>;" >$(TMPFILE); \
                    echo "\#include <stdlib.h>;" >>$(TMPFILE); \
                    echo "int main() { " >>$(TMPFILE); \
                    echo "void *p = (void *)__ms_vsnprintf; return 0; }" \
                    >>$(TMPFILE); \
                    $(CC) $(TMPFILE) -o /dev/null 2>/dev/null; \
                    echo $$?; \
                    rm $(TMPFILE))

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

ifeq (0,$(shell echo "\#include <ncurses/curses.h>" | \
                $(CPP) $(MY_CPPFLAGS) 1>/dev/null 2>/dev/null; echo $$?))
    MY_CPPFLAGS:=$(MY_CPPFLAGS) -DNCURSES_CURSES_H
endif

sfupdate_esxi$(FILE_SUFFIX): main.c fw_images.c $(VSNPRINTF_FIX)
	$(CC) -g main.c fw_images.c -o $@ $(MY_CPPFLAGS) $(MY_CFLAGS) \
	      $(XML2_CFLAGS) $(MY_LIBS) $(VSNPRINTF_FIX)

.PHONY : fw_images.c
fw_images.c :
	./include_fw_imgs.sh $(FIRMWARE_REPO)


DISTDIR = esxi_sfupdate-1.0

DISTFILES = main.c vsnprintf_fix.c Makefile README \
	    		esxi_sfupdate_client.spec \
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
