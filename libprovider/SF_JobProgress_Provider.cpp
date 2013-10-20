/***************************************************************************//*! \file liprovider/SF_JobProgress_Provider.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Generated by genprov 2.0.24
#include "SF_JobProgress_Provider.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_JobProgress_Provider::SF_JobProgress_Provider()
{
}

SF_JobProgress_Provider::~SF_JobProgress_Provider()
{
}

Load_Status SF_JobProgress_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_JobProgress_Provider::unload()
{
    return UNLOAD_OK;
}

Enable_Indications_Status SF_JobProgress_Provider::enable_indications(
    Indication_Handler<SF_JobProgress>* indication_handler)
{
    solarflare::onJobProgress.enable(indication_handler);
    return ENABLE_INDICATIONS_OK;
}

Disable_Indications_Status SF_JobProgress_Provider::disable_indications()
{
    solarflare::onJobProgress.disable();

    return DISABLE_INDICATIONS_OK;
}

/*@END@*/

CIMPLE_NAMESPACE_END
