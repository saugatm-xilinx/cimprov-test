/***************************************************************************//*! \file liprovider/SF_JobError_Provider.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

// Generated by genprov 2.0.24
#ifndef _SF_JobError_Provider_h
#define _SF_JobError_Provider_h

#include <cimple/cimple.h>
#include "SF_JobError.h"

CIMPLE_NAMESPACE_BEGIN

class SF_JobError_Provider
{
public:

    typedef SF_JobError Class;

    SF_JobError_Provider();

    ~SF_JobError_Provider();

    Load_Status load();

    Unload_Status unload();

    Enable_Indications_Status enable_indications(
        Indication_Handler<SF_JobError>* indication_handler);

    Disable_Indications_Status disable_indications();

    /*@END@*/
};

CIMPLE_NAMESPACE_END

#endif /* _SF_JobError_Provider_h */
