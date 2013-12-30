bin_PROGRAMS=sfupdate_esxi
sfupdate_esxi_SOURCES=main.c
if WINDOWS
MY_LIBS=-lcurl -lssh2 -lidn -lgnutls -lnettle -lwldap32 \
        -lz -lgcrypt -lgpg-error -lhogweed -lgmp -lpopt \
        -lintl -liconv -lcrypt32 -lws2_32 -lncurses @XML2_LIBS@ -llzma
else
MY_LIBS=-lcurl -lpopt -lncurses @XML2_LIBS@
endif
sfupdate_esxi_LDADD=$(MY_LIBS)
sfupdate_esxi_CFLAGS=-DCURL_STATICLIB @XML2_CFLAGS@ 
sfupdate_esxi_CPPFLAGS=@NCURSES_CPPFLAGS@ 
