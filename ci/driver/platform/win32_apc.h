/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER >
** \author
**  \brief
**   \date
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_driver_platform  */

#ifndef __CI_DRIVER_PLATFORM_WDM_APC_H__
#define __CI_DRIVER_PLATFORM_WDM_APC_H__

/*--------------------------------------------------------------------
 * 
 * Started off as APC only - but seems useful to have a header
 * file for stuff which isn't included in standard WDM header file
 * 
 *--------------------------------------------------------------------*/

#if !defined __KERNEL__
# error Kernel only header
#endif

/*--------------------------------------------------------------------
 *--------------------------------------------------------------------*/

/* From "Longhorn" DDK */
extern POBJECT_TYPE *PsThreadType;

/*--------------------------------------------------------------------
 *--------------------------------------------------------------------*/

/* The following definitions are not included in the ddk.  It is generally
 * assumed they will remain true, and the functions they refer to will
 * remain exposed in the device driver interface.
 */

typedef enum _KAPC_ENVIRONMENT {
    OriginalApcEnvironment,
    AttachedApcEnvironment,
    CurrentApcEnvironment
} KAPC_ENVIRONMENT;

NTKERNELAPI VOID
KeInitializeApc (
    IN PRKAPC Apc,
    IN PKTHREAD Thread,
    IN KAPC_ENVIRONMENT Environment,
    IN PKKERNEL_ROUTINE KernelRoutine,
    IN PKRUNDOWN_ROUTINE RundownRoutine OPTIONAL,
    IN PKNORMAL_ROUTINE NormalRoutine OPTIONAL,
    IN KPROCESSOR_MODE ApcMode,
    IN PVOID NormalContext
    );

NTKERNELAPI BOOLEAN
KeInsertQueueApc (
    IN PRKAPC Apc,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2,
    IN KPRIORITY Increment
    );

NTSYSAPI NTSTATUS NTAPI
NtQueueApcThread (
	IN HANDLE Thread,
	IN PKNORMAL_ROUTINE NormalRoutine,
	IN PVOID NormalContext,
	IN PVOID SystemArgument1,
	IN PVOID SystemArgument2
	); 
	
/*--------------------------------------------------------------------
 * 
 * 
 *--------------------------------------------------------------------*/

/* TODO: replace with calls to NtQueueApcThread */
int
wdm_schedule_user_apc(PKTHREAD thread, 
                      PKNORMAL_ROUTINE callback,
                      PVOID arg1, PVOID arg2, PVOID arg3);


/*--------------------------------------------------------------------
 * 
 * 
 *--------------------------------------------------------------------*/

#endif  /* __CI_DRIVER_PLATFORM_WDM_APC_H__ */
/*! \cidoxg_end */
