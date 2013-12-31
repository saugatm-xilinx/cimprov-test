#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([sfupdate_esxi], [0.1], [izbitsky@oktetlabs.ru])
AM_INIT_AUTOMAKE([1.10 foreign])
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
windows=
if test x$(echo "$host" | grep ".*-mingw.*") != x ; then
    MY_LDFLAGS="-L./libs/windows/$host"
    windows=1
fi

AC_SUBST([MY_LDFLAGS])
AM_CONDITIONAL([WINDOWS],
               [test x$windows != x])

AC_CHECK_HEADER([ncurses/curses.h],[NCURSES_CPPFLAGS=-DNCURSES_CURSES_H],[])
AC_SUBST([NCURSES_CPPFLAGS])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
