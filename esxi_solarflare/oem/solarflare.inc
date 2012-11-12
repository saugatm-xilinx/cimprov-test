# defines values for descriptor.xml and bulletin.xml

DESC_NAME := solarflare
DESC_ESX_VERSION := 5.0.0
DESC_VENDOR_EMAIL := none@vmware.com
DESC_VERSION := 1.23-7.89
DESC_VENDOR := VMware
DESC_SUMMARY := Sample ACME

DESC_DESCRIPTION := Sample solarflare provider for ESX Server
DESC_RELEASE_DATE := 2010-09-25T12:44:41-07:00

# The next five definitions name files, rather than final values.
# The format of each file is a series of lines, each one formatted as
# described in /opt/vmware/vibtools*/sample/descriptor-template.xml
# for the corresponding xml list (DESC_URLS is <urls>, DESC_DEPENDS is
# the <depends> list, and so on).  The pathnames are relative to the
# top-level source directory (typically, "samples").
DESC_URLS := oem/descriptor-urls.xml
DESC_DEPENDS := oem/solarflare-vib-depends.xml
DESC_CONFLICTS :=
DESC_REPLACES :=
DESC_PROVIDES :=

DESC_MAINT := 1
DESC_LIVE_INSTALL_ALLOWED := true
DESC_LIVE_REMOVE_ALLOWED := false
DESC_STATELESS_READY := false

VIB_DESCRIPTOR_XML := descriptor.xml
VIB_BULLETIN_XML := bulletin.xml
VIB_BULLETIN_ID := VMW-ESX-${DESC_ESX_VERSION}-${DESC_NAME}-${DESC_VERSION}

BULL_KBURL := http://kb.vmware.com/kb/435
BULL_VENDOR_CODE := VMW

PROV_VIB_BASENAME := vmware-esx-provider-$(DESC_NAME)

