##########################################################################
##! \file ./mk/sample-sources.mk
## <L5_PRIVATE L5_SOURCE>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/11/11
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

$(error This is a sample component)

##! Purpose of the component
## - target
## - host
sample_PURPOSE = target

##! List of source files of the components (relative to sample_DIR())
## It shall include sample_GENERATED(), i.e. the latter is not automatically
## included here
sample_SOURCES = test.cpp

##! List of build-time generated files of the component
sample_GENERATED = generated.cpp

##! Extra commands to perform component clean
sample_EXTRA_CLEAN = rm something

##! Component's principal directory (relative to TOP())
sample_DIR = mydir

##! Component's principal target (i.e. library name)
sample_TARGET = libsample.a

##! List of header file directories exposed by the component
## This will be used to autoconstruct sample_PROVIDE_CPPFLAGS()
sample_INCLUDES = $(sample_DIR)

##! CPPFLAGS needed for the component to build
## It need to include sample_PROVIDE_CPPFLAGS() 
## if sample_INCLUDES() is used
sample_CPPFLAGS = $(sample_PROVIDE_CPPFLAGS) -DSAMPLE=1

##! List of system-wide libraries used by the component
sample_SYSLIBRARIES += pci

##! List of components the component depends upon in run-time
sample_DEPENDS = libcimobjects

##! List of components the component depends upon in build-time
sample_BUILD_DEPENDS = genmod

$(eval $(call component,sample,SHARED_LIBRARIES))
