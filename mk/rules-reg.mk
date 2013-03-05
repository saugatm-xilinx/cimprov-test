repository.reg : repository.mof mof2reg.awk
	$(AWK) -f mof2reg.awk -vPRODUCTNAME=$(PROVIDER_LIBRARY) -vNAMESPACE=$(IMP_NAMESPACE) $< >$@

interop.reg : interop.mof mof2reg.awk
	$(AWK) -f mof2reg.awk -vPRODUCTNAME=$(PROVIDER_LIBRARY) -vNAMESPACE=$(INTEROP_NAMESPACE) $< >$@
