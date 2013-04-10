// Generated by genprov 2.0.24
#include "SF_JobSuccess_Provider.h"
#include "sf_provider.h"

CIMPLE_NAMESPACE_BEGIN

SF_JobSuccess_Provider::SF_JobSuccess_Provider()
{
}

SF_JobSuccess_Provider::~SF_JobSuccess_Provider()
{
}

Load_Status SF_JobSuccess_Provider::load()
{
    return LOAD_OK;
}

Unload_Status SF_JobSuccess_Provider::unload()
{
    return UNLOAD_OK;
}

Enable_Indications_Status SF_JobSuccess_Provider::enable_indications(
    Indication_Handler<SF_JobSuccess>* indication_handler)
{
    solarflare::onJobSuccess.enable(indication_handler);
    return ENABLE_INDICATIONS_OK;
}

Disable_Indications_Status SF_JobSuccess_Provider::disable_indications()
{
    solarflare::onJobSuccess.disable();
    return DISABLE_INDICATIONS_OK;
}

/*@END@*/

CIMPLE_NAMESPACE_END
