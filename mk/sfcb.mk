##########################################################################
##! \file ./mk/sfcb.mk
## <L5_PRIVATE L5_SCRIPT>
## \author  OktetLabs
##  \brief  CIM Provider
##   \date  2013/10/02
##    \cop  (c) Solarflare Communications Inc.
## </L5_PRIVATE>
##
##########################################################################

override CIM_INTERFACE = cmpi
SFCBSTAGE = $(SFCB_ROOT)/bin/sfcbstage
SFCBREPOS = $(SFCB_ROOT)/bin/sfcbrepos -f

