libcimobjects_PURPOSE = target
libcimobjects_TARGET = libcimobjects.a
libcimobjects_DIR = libcimobjects
libcimobjects_INCLUDES = $(libcimobjects_DIR)
libcimobjects_DEPENDS = $(CIMPLE_COMPONENT)

libcimobjects_EXTRA_MOF = CIM_OperatingSystem CIM_Chassis SF_ServiceAffectsElement
libcimobjects_EXTRA_MOF += SF_InstCreation SF_InstModification SF_AlertIndication
libcimobjects_EXTRA_MOF += SF_Card SF_ConnectorOnPackage SF_OrderedComponent
libcimobjects_EXTRA_MOF += SF_DeviceSAPImplementation SF_Realizes
libcimobjects_EXTRA_MOF += SF_UseOfLog SF_RecordLogCapabilities SF_RecordLog
libcimobjects_EXTRA_MOF += SF_ElementSoftwareIdentity SF_Container

ifeq ($(CIM_SERVER),pegasus)
libcimobjects_EXTRA_MOF += IBMPSG_ComputerSystem IBMSD_ComputerSystem IBMSD_SPComputerSystem IBMSD_Chassis PG_ComputerSystem PG_RegisteredProfile
endif
ifeq ($(CIM_SERVER),esxi)
libcimobjects_EXTRA_MOF += OMC_UnitaryComputerSystem OMC_Chassis
endif
ifeq ($(CIM_SERVER),wmi)
libcimobjects_EXTRA_MOF += Win32_ComputerSystem Win32_SystemEnclosure
endif

ifneq ($(_DO_NOT_GENERATE),1)
include $(libcimobjects_DIR)/classes.mk
endif

libcimobjects_GENERATED = $(libcimobjects_SOURCES) $(patsubst %.cpp,%.h,$(libcimobjects_SOURCES)) classes.mk repository.mof 

$(eval $(call component,libcimobjects,STATIC_LIBRARIES))

genclass_cmd = cd $(libcimobjects_DIR); CIMPLE_MOF_PATH="$(CIM_SCHEMA_DIR)" $(abspath $(genclass_TARGET))

$(libcimobjects_DIR)/.genclass : $(libcimobjects_DIR)/classes $(libcimobjects_DIR)/repository.mof \
								 $(CIM_SCHEMA_ROOTFILE) $(genclass_TARGET)
	$(genclass_cmd) -S -r -e -F$(abspath $<) $(libcimobjects_EXTRA_MOF)

$(filter %.h,$(_libcimobjects_GENERATED)) : $(libcimobjects_DIR)/classes.mk

$(libcimobjects_DIR)/classes.mk : $(libcimobjects_DIR)/.genclass
	$(AWK) '{ print "libcimobjects_SOURCES += ", $$0 }' $< >$@

libcimobjects_MOF_CPPFLAGS = -DINSTANCE='$(MOF_INSTANCE_DEF)' -DASSOCIATION='$(MOF_ASSOCIATION_DEF)' \
	-DINDICATION='$(MOF_INDICATION_DEF)' -DINSTANCE_NWMI='$(MOF_INSTANCE_NWMI_DEF)' \
	-DASSOCIATION_NWMI='$(MOF_ASSOCIATION_NWMI_DEF)' -DINDICATION_NWMI='$(MOF_INDICATION_NWMI_DEF)'
ifeq ($(CIM_SERVER),wmi)
MOF_INSTANCE_DEF = [dynamic, provider("$(PROVIDER_LIBRARY)")]
MOF_ASSOCIATION_DEF = $(MOF_INSTANCE_DEF)
MOF_INDICATION_DEF = [dynamic, provider("$(PROVIDER_LIBRARY)"), Indication]
MOF_INSTANCE_NWMI_DEF = 
MOF_ASSOCIATION_NWMI_DEF =
MOF_INDICATION_NWMI_DEF =
else
MOF_INSTANCE_DEF = 
MOF_ASSOCIATION_DEF = [Association]
MOF_INDICATION_DEF = [Indication]
MOF_INSTANCE_NWMI_DEF = 
MOF_ASSOCIATION_NWMI_DEF = [Association]
MOF_INDICATION_NWMI_DEF = [Indication]
endif

MOF_PREPROCESS := $(target_CXX) -E -P -DTARGET_CIM_SERVER_$(CIM_SERVER) \
						$(libcimobjects_MOF_CPPFLAGS) \
						$(libcimobjects_CPPFLAGS) $(target_CPPFLAGS)


$(libcimobjects_DIR)/repository.mof : $(libcimobjects_DIR)/repository.mof.cpp
	$(MOF_PREPROCESS) -DINBUILD=1 -DIMPNS=1 $< >$@

$(libcimobjects_DIR)/namespace.mof : $(libcimobjects_DIR)/repository.mof.cpp
	$(MOF_PREPROCESS) -DIMPNS=1 $< >$@

$(libcimobjects_DIR)/interop.mof : $(libcimobjects_DIR)/repository.mof.cpp
	$(MOF_PREPROCESS) -DINTEROPNS=1 $< >$@

$(libcimobjects_DIR)/root.mof : $(libcimobjects_DIR)/repository.mof.cpp
	$(MOF_PREPROCESS) -DROOTNS=1 $< >$@

ifeq ($(CIM_INTERFACE),wmi)
PATCH_SCHEMA_FOR_WMI = | $(SED) 's/\[\([^]]*\)Association,[[:space:]]*/\[\1/; s/\[\([^]]*\)Abstract,[[:space:]]*/\[\1/'
CIM_CLASS_PREFIX = CIM_
else
CIM_CLASS_PREFIX = CIM_\|qualifiers
endif


$(libcimobjects_DIR)/schema.mof : $(CIM_SCHEMA_ROOTFILE) $(MAKEFILE_LIST)
	cat `$(SED) -n 's!#pragma include ("\(\([^"/]\+/\)\?\($(CIM_CLASS_PREFIX)\)[^/"]*\.mof\)")!$(dir $<)\1!p' $<` $(PATCH_SCHEMA_FOR_WMI) >$@

