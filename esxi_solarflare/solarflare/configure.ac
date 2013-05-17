# Copyright (c) 2007 - 2008 VMware, Inc. All rights reserved.
#
AC_INIT(Solarflare, 1.0.0)
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([libprovider/sf_utils.cpp])

AM_CONFIG_HEADER([config_ac.h])
AC_PREFIX_DEFAULT(/usr)

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_LANG(C)

AC_ARG_ENABLE(debug,
        [AC_HELP_STRING([--enable-debug],
                [enable debug mode for SFCB.])],
        [enable_debug=$enableval],
        [enable_debug="no"]
        )       

case $enable_debug in
yes)
   DEBUG_FLAGS="-DDEBUG -D_GLIBCXX_DEBUG -g -Wall -W" # Additional debugging flags.
   ;;
*)
   OPT_FLAGS="-DNDEBUG -g -Os"      # Additional optimization flags.
   ;;
esac

CXXFLAGS="$CXXFLAGS -fPIC $DEBUG_FLAGS $OPT_FLAGS"
CPPFLAGS=$CXXFLAGS
AC_CHECK_HEADERS([syslog.h string.h],,[AC_MSG_ERROR(Missing headers)])


#######################################################################
smash_namespace="root/cimv2"
AC_ARG_WITH(smash-namespace,
	[ --with-smash-namespace=NS   Specifiy the namespace for OEM specific data. default = oem ],
	[
      if test "x$withval" != "xno" ; then
	  		smash_namespace="$withval"
      fi
	]
)
AC_SUBST([smash_namespace])
#######################################################################
sfcb_interop_namespace="root/interop"
AC_ARG_WITH(sfcb-interop-namespace,
	[ --with-sfcb-interop-namespace=NS   Specifiy the namespace for INTEROP specific data. default = root/interop ],
	[
      if test "x$withval" != "xno" ; then
	  		sfcb_interop_namespace="$withval"
      fi
	]
)
AC_SUBST([sfcb_interop_namespace])

#######################################################################
config_namespace="root/config"
AC_ARG_WITH(config-namespace,
	[ --with-config-namespace=NS   Specifiy the namespace for configurationg data. default = root/config ],
	[
      if test "x$withval" != "xno" ; then
	  		config_namespace="$withval"
      fi
	]
)
AC_SUBST([config_namespace])

sfcb_schema_dir="/var/lib/sfcb/CIM"
AC_ARG_WITH(sfcb-schema-dir,
	[ --with-sfcb-schema-dir=PATH   Specifiy the directory where the CIM schema results. default = /var/lib/sfcb/CIM ],
	[
      if test "x$withval" != "xno" ; then
	  		sfcb_schema_dir="$withval"
      fi
	]
)
sfcb_staging_dir="/var/lib/sfcb/stage"
AC_ARG_WITH(sfcb-staging-dir,
	[ --with-sfcb-staging-dir=PATH   Specifiy the directory where SFCB stages registration and mof files . default = /var/lib/sfcb/stage ],
	[
      if test "x$withval" != "xno" ; then
	  		sfcb_staging_dir="$withval"
      fi
	]
)
sfcb_registration_dir="/var/lib/sfcb/registration"
AC_ARG_WITH(sfcb-registration-dir,
	[ --with-sfcb-registration-dir=PATH   Specifiy the directory where SFCB registration files go. default = /var/lib/sfcb/registration ],
	[
      if test "x$withval" != "xno" ; then
	  		sfcb_registration_dir="$withval"
      fi
	]
)

# Set up CIMOM specific build flags
AC_ARG_WITH([sfcb],
   [AS_HELP_STRING([--with-sfcb], [Enable support for the sfcb CIMOM])],
   [], [with_sfcb=no])

AS_IF([test "x$with_sfcb" != "xno"], 
   [AC_PATH_PROG([SFCBREPOS], [sfcbrepos], [])]
   AS_IF([test x$SFCBREPOS = x],
      [AC_MSG_ERROR([Unable to locate sfcbrepos with --enable-sfcb set])]
   ))
AM_CONDITIONAL(ENABLE_SFCB, test x$with_sfcb != xno)
AC_PATH_PROG([SFCB_REG_TOOL], [gen_sfcb_reg.pl])
AC_PATH_PROG([TEST_CLASSLIST_TOOL], [gen_sfcb_cls.pl])
AC_PATH_PROG([PEGASUS_REG_TOOL], [gen_pegasus_reg.pl])
AC_SUBST([sfcb_regdir], /var/lib/sfcb/stage/regs)
AC_SUBST([test_classlistdir], /etc/cimtest/classlist)

# Set up the locations to place the MOFs
AC_SUBST([sfcb_config_nsdir],
	 "\$(sfcb_staging_dir)/mofs/\$(config_namespace)")
AC_SUBST([sfcb_interop_nsdir],
	 "\$(sfcb_staging_dir)/mofs/\$(sfcb_interop_namespace)")
AC_SUBST([sfcb_root_nsdir],
	 "\$(sfcb_staging_dir)/mofs/root/cimv2")
AC_SUBST([sfcb_nsdir],
	 "\$(sfcb_staging_dir)/mofs/\$(smash_namespace)")

AC_SUBST([sfcb_schema_dir])
AC_SUBST([sfcb_registration_dir])
AC_SUBST([sfcb_staging_dir])

# End CIMOM specific setup



AC_CONFIG_FILES([Makefile repository.reg interop.reg root.reg])

AC_OUTPUT
