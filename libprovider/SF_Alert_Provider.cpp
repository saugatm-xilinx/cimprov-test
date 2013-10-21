/***************************************************************************//*! \file liprovider/SF_Alert_Provider.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Generated by genprov 2.0.24
#include "SF_Alert_Provider.h"
#include "sf_provider.h"
#include "sf_alerts.h"

CIMPLE_NAMESPACE_BEGIN

SF_Alert_Provider::SF_Alert_Provider() : _indication_handler(0)
{
}

SF_Alert_Provider::~SF_Alert_Provider()
{
}

Load_Status SF_Alert_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_Alert_Provider::unload()
{
    return UNLOAD_OK;
}

Enable_Indications_Status SF_Alert_Provider::enable_indications(
    Indication_Handler<SF_Alert>* indication_handler)
{
    solarflare::onAlert.enable(indication_handler);

    return ENABLE_INDICATIONS_OK;
}

Disable_Indications_Status SF_Alert_Provider::disable_indications()
{
    solarflare::onAlert.disable();

    return DISABLE_INDICATIONS_OK;
}

/*@END@*/

CIMPLE_NAMESPACE_END
