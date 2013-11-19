##########################################################################
##! \file ./mk/sample-sources.mk
## <L5_PRIVATE L5_SCRIPT>
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

##! CPPFLAGS needed for components depending on this component
## This variable is automatically appended with sample_INCLUDES()
sample_PROVIDE_CPPFLAGS = -DSAMPLE_XXx

##! CXXFLAGS needed for compiling the component (C++ sources)
## These flags are used in addition to system-wide flags set with
## host_CXXFLAGS() or target_CXXFLAGS()
sample_CXXFLAGS = -fwarning

##! CFLAGS needed for compiling the component (C++ sources)
## These flags are used in addition to system-wide flags set with
## host_CFLAGS() or target_CFLAGS()
sample_CFLAGS = -fwarning

##! CPPFLAGS needed for the component to build
## It need to include sample_PROVIDE_CPPFLAGS() 
## if sample_INCLUDES() is used
## If it is not defined explicitly, it defaults to sample_PROVIDE_CPPFLAGS()
sample_CPPFLAGS = $(sample_PROVIDE_CPPFLAGS) -DSAMPLE=1

##! LDFLAGS needed for the component to build
## \note This should not include any -lXXX flags, use 
## sample_LIBRARIES() and/or sample_SYSLIBRARIES()
sample_LDFLAGS = -Wl,xxxx

##! LDFLAGS needed for dependent components to build
## \note This should not include any -lXXX flags, use 
## sample_PROVIDE_LIBRARIES() and/or sample_PROVIDE_SYSLIBRARIES()
sample_PROVIDE_LDFLAGS = -Wl,yyyy

##! List of project libraries used by the component
## \note In most cases this variable shall be autofilled based on sample_DEPENDS()
sample_LIBRARIES += libsample0.so

##! List of project libraries provided by the component
## \note Usually this is automatically set for static library components to
## sample_TARGET(), and should be left empty for other components
sample_PROVIDE_LIBRARIES =

##! List of system-wide libraries used by the component
## \note The list consist of names as would be passed to the linker, 
## but without -l prefix
sample_SYSLIBRARIES += pci

##! List of system-wide libraries needed for the dependent components
sample_PROVIDE_SYSLIBRARIES += pci

##! List of components the component depends upon in run-time
## Values for sample_CPPFLAGS(), sample_LDFLAGS(), sample_LIBRARIES(), sample_SYSLIBRARIES()
## are augmented with corresponding variable value of the governing components
sample_DEPENDS = libcimobjects

##! List of components the component depends upon in build-time
sample_BUILD_DEPENDS = genmod

$(eval $(call component,sample,SHARED_LIBRARIES))
