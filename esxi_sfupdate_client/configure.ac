#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([sfupdate_esxi], [0.1], [izbitsky@oktetlabs.ru])
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC

SEARCH_PATH=$($CC --print-search-dirs | grep "programs: =" | sed "s/programs: =//" | sed "s/:/ /g")
SEARCH_PATH="$SEARCH_PATH $(echo $PATH | sed "s/:/ /g")"
for d in $SEARCH_PATH ; do
    if test -f $d/xml2-config ; then
        XML_CONFIG=$d/xml2-config
        break
    fi
done
if test "x$XML_CONFIG" = "x" ; then
    XML_CONFIG=xml2-config
fi
AC_MSG_CHECKING([for libxml2 library])
if ${XML_CONFIG} --libs print > /dev/null 2>&1
then
    XML2_LIBS="$XML2_LIBS `$XML_CONFIG --libs`"
    XML2_CFLAGS="$XML2_CFLAGS `$XML_CONFIG --cflags`"
    AC_MSG_RESULT([found])

    AC_SUBST([XML2_CFLAGS])
    AC_SUBST([XML2_LIBS])
else
    if test x"$ERR_REPORT_TYPE" = x"ERROR"; then
        AC_MSG_ERROR([Could not find libxml2, check ftp://xmlsoft.org/.])
    else
        AC_MSG_ERROR([not found])
    fi
fi

MY_LDFLAGS=
WINDOWS=
if test x$(echo "$host" | grep ".*-mingw.*") != x ; then
    MY_LDFLAGS="-L./libs/windows/$host"
    WINDOWS=1
elif test x"$(file /bin/bash | grep 'x86-64')" != x ; then
    MY_LDFLAGS="-L./libs/linux/64"
else
    MY_LDFLAGS="-L./libs/linux/32"
fi

AC_SUBST([MY_LDFLAGS])
AC_SUBST([WINDOWS])

AC_CHECK_HEADER([ncurses/curses.h],[NCURSES_CPPFLAGS=-DNCURSES_CURSES_H],[])
AC_SUBST([NCURSES_CPPFLAGS])

AC_ARG_VAR([FIRMWARE_IMG_PATHS],
           [Locations of folders with the firmware images to be incorporated])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
