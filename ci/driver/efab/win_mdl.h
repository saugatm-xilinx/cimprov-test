/**************************************************************************\
*//*! \file win_intf.h
** <L5_PRIVATE L5_HEADER >
** \author  cgg
**  \brief  WIN32 MDL manipulation macros
**   \date  
**    \cop  (c) Solarflare Communications
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_driver_efab_windows  */

#ifndef _CI_DRIVER_WIN_MDL_H_
#define _CI_DRIVER_WIN_MDL_H_

#ifndef _WIN32
#error _WIN32 is not defined.
#endif



/*--------------------------------------------------------------------
 *!
 * Windows MDL manipulation
 *
 *--------------------------------------------------------------------*/

/* These functions provide an API for accessing the fields in an MDL that
   are supposed to be opaque:  ideally we would not need these
*/

/* Stop "prefast" complaining - we know it's wrong */
#ifdef _PREFAST_
#pragma warning(disable:28145)
#endif

ci_inline void *
ci_MmGetMdlMappedSystemVa(PMDL mdl)
{   return mdl->MappedSystemVa;
    /*< @TODO: consider using MmGetSystemAddressForMdlSafe(mdl, priority) */
}

ci_inline int /*bool*/
ci_MmGetMdlIsIO(PMDL mdl)
{   return 0 != (mdl->MdlFlags & MDL_IO_SPACE);
}

ci_inline void
ci_MmSetMdlIO(PMDL mdl, int /*bool*/ yesno)
{   if (yesno)
	mdl->MdlFlags |= MDL_IO_SPACE;
    else
	mdl->MdlFlags &= ~MDL_IO_SPACE;
}

ci_inline int /*bool*/
ci_MmGetMdlIsNonpaged(PMDL mdl)
{   return 0 != (mdl->MdlFlags & MDL_SOURCE_IS_NONPAGED_POOL);
}

ci_inline void
ci_MmSetMdlNonpaged(PMDL mdl, int /*bool*/ yesno)
{   if (yesno)
	mdl->MdlFlags |= MDL_SOURCE_IS_NONPAGED_POOL;
    else
	mdl->MdlFlags &= ~MDL_SOURCE_IS_NONPAGED_POOL;
}


ci_inline unsigned ci_MmMdlIsDereferenceable(const MDL* mdl)
{
  return
    mdl->MdlFlags & (MDL_SOURCE_IS_NONPAGED_POOL | MDL_MAPPED_TO_SYSTEM_VA);
}

#ifdef _PREFAST_
#pragma warning(default:28145)
#endif

#endif  /* _CI_DRIVER_WIN_MDL_H_ */
/*! \cidoxg_end */
