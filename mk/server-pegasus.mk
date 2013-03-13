
define pegasus_tool

$($(1)_DIR)/%.o $($(1)_DIR)/%.d : CPPFLAGS += $$(PEGASUS_CPPFLAGS)

$($(1)_TARGET) : LDFLAGS += $$(PEGASUS_LDFLAGS) 

$($(1)_TARGET) : LIBRARIES += $$(PEGASUS_LIBRARIES)

endef
