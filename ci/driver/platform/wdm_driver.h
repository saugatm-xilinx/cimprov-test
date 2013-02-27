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

#ifndef __CI_DRIVER_PLATFORM_WDM_DRIVER_H__
#define __CI_DRIVER_PLATFORM_WDM_DRIVER_H__

/*--------------------------------------------------------------------
 *
 *--------------------------------------------------------------------*/

/* flush DPC function - address looked up dynamically  */
extern VOID (*ci_KeFlushQueuedDpcs)(VOID);

/*--------------------------------------------------------------------
 *
 * ci_waitable
 *
 *--------------------------------------------------------------------*/

/* we use this extern to complete an Irp */
typedef PIRP                    ci_blocking_ctx_t;
extern NTSTATUS
platform_specific_request_complete(ci_blocking_ctx_t request,
                             NTSTATUS status,
	                     ULONG_PTR information);


#if 1	/* ?? Off just until we start wanting to use this stuff. */
#define CI_BLOCKING_CTX_ARG(x)  , x
#else
#define CI_BLOCKING_CTX_ARG(x)
#endif

typedef struct {
  LIST_ENTRY	waiters;
  ci_irqlock_t	lock;
} ci_waitable_t;

typedef struct {
  ci_int64       hnsecs;    /*!< hundreds of nano-seconds */
  int            timedout;
} ci_waitable_timeout_t;

typedef struct {
  PIRP	              irp;
  ci_irqlock_state_t  lock_flags;
} ci_waiter_t;

#define CI_WAITER_BCTX(waiter) ((waiter)->irp)

typedef int (*ci_waiter_on_wakeup_fn)(ci_waiter_t*, void*, void*, int rc);


#define IRP_NIC_GET(irp)           ((irp)->Tail.Overlay.DriverContext[0])
#define IRP_NIC_SET(irp, nic)      (irp)->Tail.Overlay.DriverContext[0] = (nic)

/* Internal to this header */
#define _IRP_ONWAKEUP(irp)         ((irp)->Tail.Overlay.DriverContext[1])
#define _IRP_OPAQUE1(irp)          ((irp)->Tail.Overlay.DriverContext[2])
#define _IRP_OPAQUE2(irp)          ((irp)->Tail.Overlay.DriverContext[3])

/* We thoroughly abuse the list link to store a state when we're not
** on a list.
*/
#define IRP_LIST(irp)              (&(irp)->Tail.Overlay.ListEntry)
#define IRP_FROM_LIST(entry)       (PIRP)(CI_CONTAINER(IRP, Tail.Overlay.ListEntry, 	     \
						     entry))

/* Since these are user mode addresses, and irps live in the kernel,
** lets hope these are safe
*/
#define _IRP_UNINITIALISED  0x00000000
#define _IRP_WAKEUP         0x00000001
#define _IRP_CANCEL         0x00000002

#define IRP_LIST_L(irp)            ((irp)->Tail.Overlay.ListEntry.Flink)
#define IRP_STATE_SET(irp, v)      (IRP_LIST_L(irp) = (void*) (v))
#define IRP_STATE_CMP(irp, v)      (IRP_LIST_L(irp) == (void*) (v))
#define IRP_ONLIST(irp)            (!(IRP_STATE_CMP((irp), _IRP_WAKEUP) | \
				      IRP_STATE_CMP((irp), _IRP_CANCEL)))

#define IRP_STATE_SET_UNINITIALISED(irp) \
        IRP_STATE_SET(irp, _IRP_UNINITIALISED)


#define CI_WAITER_CONTINUE_TO_WAIT -EWOULDBLOCK
#define CI_WAITER_CONTINUE_TO_WAIT_REENTRANT  2
#define CI_WAITER_CONVERT_REENTRANT(x)    ((x) == CI_WAITER_CONTINUE_TO_WAIT\
                                           ? CI_WAITER_CONTINUE_TO_WAIT_REENTRANT\
                                           : (x))


ci_inline int ci_waitable_active(ci_waitable_t* w)
{ return ! IsListEmpty(&w->waiters); }

ci_inline void ci_waitable_ctor(ci_waitable_t* w) {
  InitializeListHead(&w->waiters);
  ci_irqlock_ctor(&w->lock);
}
ci_inline void ci_waitable_dtor(ci_waitable_t* w)
{ ci_irqlock_dtor(&w->lock); }


ci_inline void __ci_waitable_wakeup_irp(ci_waitable_t* w, PIRP irp) {
  ci_waiter_t waiter = { irp };
  int rc;
  PIO_STACK_LOCATION pstack;

  ci_waiter_on_wakeup_fn on_wakeup =
    (ci_waiter_on_wakeup_fn) _IRP_ONWAKEUP(irp);
  rc = on_wakeup(&waiter, _IRP_OPAQUE1(irp), _IRP_OPAQUE2(irp), 0);
  if( rc == CI_WAITER_CONTINUE_TO_WAIT )
    ci_irqlock_unlock(&w->lock, &waiter.lock_flags);
  else if( rc != CI_WAITER_CONTINUE_TO_WAIT_REENTRANT ) {
    IoAcquireCancelSpinLock(&irp->CancelIrql);
    IoSetCancelRoutine(irp, NULL);
    IoReleaseCancelSpinLock(irp->CancelIrql);
    if( rc == 0 )  rc = STATUS_SUCCESS;
    /* Assume here that appropriate output size is the same as the input size, which
       should be the size of the union used for passing in IOCTL parameter data */
    pstack = IoGetCurrentIrpStackLocation(irp);
    platform_specific_request_complete(irp, rc, pstack->Parameters.DeviceIoControl.InputBufferLength);
  }
}

ci_inline void ci_waitable_wakeup_one(ci_waitable_t* w) {
  ci_irqlock_state_t lock_flags;
  PIRP irp = NULL;

  ci_irqlock_lock(&w->lock, &lock_flags);
  if( ! IsListEmpty(&w->waiters) ) {
    PLIST_ENTRY listhead;
    listhead = RemoveHeadList(&w->waiters);
    irp = IRP_FROM_LIST(listhead);
    IRP_STATE_SET(irp, _IRP_WAKEUP);
  }
  ci_irqlock_unlock(&w->lock, &lock_flags);
  if( irp )  __ci_waitable_wakeup_irp(w, irp);
}

ci_inline void ci_waitable_wakeup_all(ci_waitable_t* w) {
  ci_irqlock_state_t lock_flags;
  PLIST_ENTRY irps = NULL;
  ci_irqlock_lock(&w->lock, &lock_flags);
  while( ! IsListEmpty(&w->waiters) ) {
    PLIST_ENTRY l = RemoveHeadList(&w->waiters);
    IRP_STATE_SET(IRP_FROM_LIST(l), _IRP_WAKEUP);
    l->Blink = irps;
    irps = l;
  }
  ci_irqlock_unlock(&w->lock, &lock_flags);
  while( irps ) {
    PLIST_ENTRY next = irps->Blink;
    __ci_waitable_wakeup_irp(w, IRP_FROM_LIST(irps));
    irps = next;
  }
}


ci_inline int ci_waiter_pre(ci_waiter_t* waiter, ci_waitable_t* w
			    CI_BLOCKING_CTX_ARG(ci_blocking_ctx_t irp)) {
  ci_assert(! IRP_ONLIST(irp) || IRP_STATE_CMP((irp), _IRP_UNINITIALISED));
  waiter->irp = irp;
  ci_irqlock_lock(&w->lock, &waiter->lock_flags);
  if( ! IRP_STATE_CMP(irp, _IRP_CANCEL) ) {
    InsertHeadList(&w->waiters, IRP_LIST(irp));
  }
  else {
    ci_irqlock_unlock(&w->lock, &waiter->lock_flags);
    platform_specific_request_complete(irp, STATUS_CANCELLED, 0);
    return -EINTR;
  }
  return 0;
}

#define ci_waiter_exclusive_pre  ci_waiter_pre

ci_inline void ci_waiter_prepare_continue_to_wait(ci_waiter_t* waiter,
                                                  ci_waitable_t* w) {
  ci_assert( !IRP_ONLIST(waiter->irp));
  ci_irqlock_lock(&w->lock, &waiter->lock_flags);
  InsertHeadList(&w->waiters, IRP_LIST(waiter->irp));
}

ci_inline void ci_waiter_dont_continue_to_wait(ci_waiter_t* waiter,
                                                  ci_waitable_t* w)
{ 
  ci_assert(IRP_ONLIST(waiter->irp));
  RemoveEntryList(IRP_LIST(waiter->irp));
  IRP_STATE_SET(waiter->irp, _IRP_WAKEUP);
  ci_irqlock_unlock(&w->lock, &waiter->lock_flags);
}

ci_inline void ci_waiter_post(ci_waiter_t* waiter, ci_waitable_t* w) {
  ci_assert(!IRP_ONLIST(waiter->irp));
}

ci_inline VOID ci_waiter_irp_cancel_routine(IN PDEVICE_OBJECT DeviceObject,
					    IN PIRP irp)
{
  ci_irqlock_state_t lock_flags;
  int cancel = 0;
  //ci_assert(0); // FIXME We need to reinstate the locks here
 // ci_irqlock_lock(&w->lock, &lock_flags);
  if( IRP_STATE_CMP(irp, _IRP_WAKEUP) )
    IRP_STATE_SET(irp, _IRP_CANCEL);
  else if( ! IRP_STATE_CMP(irp, _IRP_CANCEL) ) {
    RemoveEntryList(IRP_LIST(irp));
    cancel = 1;
  }
 // ci_irqlock_unlock(&w->lock, &lock_flags);
  IoSetCancelRoutine((PIRP)(irp), NULL);
  IoReleaseCancelSpinLock(irp->CancelIrql);
  if( cancel )  platform_specific_request_complete(irp, STATUS_CANCELLED, 0);
}

ci_inline void ci_waiter_dont_wait(ci_waiter_t* waiter, ci_waitable_t* w) {
  RemoveEntryList(IRP_LIST(waiter->irp));
  IRP_STATE_SET(waiter->irp, _IRP_WAKEUP);
  ci_irqlock_unlock(&w->lock, &waiter->lock_flags);
}

ci_inline int ci_waiter_wait(ci_waiter_t* waiter, ci_waitable_t* wq,
			     ci_waitable_timeout_t * timeout,
			     void* opaque1, void* opaque2,
			     ci_waiter_on_wakeup_fn on_wakeup) {
  PIRP irp = waiter->irp;
  ci_assert(irp);
  _IRP_ONWAKEUP(irp) = on_wakeup;
  _IRP_OPAQUE1(irp) = opaque1;
  _IRP_OPAQUE2(irp) = opaque2;
  IoAcquireCancelSpinLock(&irp->CancelIrql);
  IoSetCancelRoutine(irp, ci_waiter_irp_cancel_routine);
  IoReleaseCancelSpinLock(irp->CancelIrql);
  ci_irqlock_unlock(&wq->lock, &waiter->lock_flags);
  return -EWOULDBLOCK;
}


/* ?? FIXME: Oh dear.  Anywhere you see this function, it means we haven't
** yet arranged for an IRP to be available in a place where we need to be
** able to block in the kernel.
**
** So this just serves to document such places, and ensure we get a runtime
** error if we hit them.  But don't be too quick to fix them, as in some
** cases a more serious restructuring is needed than simply propagating the
** IRP through to that piece of code.
*/
ci_inline PIRP __ci_blocking_ctx_arg_needed(const char* file, int line) {
  ci_log("%s: BAD FIXME: this code needs an IRP at %s:%d",
	 __FUNCTION__, file, line);
  ci_assert (0);
  return 0;
}
#define ci_blocking_ctx_arg_needed()			\
  __ci_blocking_ctx_arg_needed(__FILE__, __LINE__)


/*--------------------------------------------------------------------
 *
 * wait_queue
 *
 *--------------------------------------------------------------------*/

struct timeval {  /*!< this follows definition in winsock2.h */
  long  tv_sec;
  long  tv_usec;
};


/*! Comment? */
typedef struct {
  ci_dllist  waiters;   /*!< list of [ci_waitq_waiter_t]s */
  ci_irqlock_t  lock;
} ci_waitq_t;


/*! Comment? */
typedef struct {
  KEVENT     ev;
  ci_dllink  link;
  int        signalled;
} ci_waitq_waiter_t;


/*! Comment? */
typedef struct {
  ci_int64       hnsecs;    /*!< hundreds of nano-seconds */
  int            timedout;
} ci_waitq_timeout_t;


/*! Comment? */
ci_inline void
ci_waitq_ctor(ci_waitq_t* wq)
{
  ci_dllist_init(&wq->waiters);
  ci_irqlock_ctor(&wq->lock);
}

/*! Comment? */
ci_inline void
ci_waitq_dtor(ci_waitq_t* wq)
{
  ci_irqlock_dtor(&wq->lock);
}

/*! Comment? */
ci_inline void
ci_waitq_wakeup(ci_waitq_t* wq)
{
  ci_waitq_waiter_t* w;
  ci_irqlock_state_t lock_flags;

  ci_irqlock_lock(&wq->lock, &lock_flags);
  CI_DLLIST_FOR_EACH2(ci_waitq_waiter_t, w, link, &wq->waiters)
    KeSetEvent(&w->ev, IO_NO_INCREMENT, FALSE);
  ci_irqlock_unlock(&wq->lock, &lock_flags);
}

#define ci_waitq_init_timeout(tmo, tv)				\
  do {								\
    (tmo)->timedout = 0;					\
    /* negative means relative time */				\
    (tmo)->hnsecs = -((ci_int64) (tv)->tv_sec * 10000000);	\
    (tmo)->hnsecs -= (tv)->tv_usec * 10;			\
  } while(0)

ci_inline void
ci_waitq_waiter_pre(ci_waitq_waiter_t * waiter, ci_waitq_t* wq)
{
  ci_irqlock_state_t lock_flags;

  KeInitializeEvent(&waiter->ev, SynchronizationEvent, FALSE);
  ci_irqlock_lock(&wq->lock, &lock_flags);
  ci_dllist_push(&wq->waiters, &waiter->link);
  ci_irqlock_unlock(&wq->lock, &lock_flags);
  waiter->signalled = 0;
}

#define ci_waitq_waiter_exclusive_pre  ci_waitq_waiter_pre

#define ci_waitq_active(wq)	  ci_dllist_not_empty(&(wq)->waiters)


#define ci_waitq_waiter_wait(waiter, wq, cond)			\
  do {								\
    if( !(cond) ) {						\
      NTSTATUS status;						\
      status = KeWaitForSingleObject(&(waiter)->ev, Executive,	\
				     KernelMode, TRUE, 0);	\
      (waiter)->signalled = status != STATUS_SUCCESS;		\
    }								\
  } while(0)

#define ci_waitq_waiter_timedwait(waiter, wq, cond, tmo)		   \
  do {									   \
    if( !(cond) ) {							   \
      NTSTATUS status;							   \
      status = KeWaitForSingleObject(&(waiter)->ev, Executive, KernelMode, \
	     TRUE, (PLARGE_INTEGER) ((tmo)->hnsecs ? &(tmo)->hnsecs:0));   \
      if( status == STATUS_TIMEOUT )  (tmo)->timedout = 1;		   \
      else (waiter)->signalled = status != STATUS_SUCCESS;		   \
    }									   \
  } while(0)

#define ci_waitq_waiter_again(waiter, wq)

ci_inline void
ci_waitq_waiter_post(ci_waitq_waiter_t * waiter, ci_waitq_t* wq)
{
  ci_irqlock_state_t lock_flags;
  ci_irqlock_lock(&wq->lock, &lock_flags);
  ci_dllist_remove(&waiter->link);
  ci_irqlock_unlock(&wq->lock, &lock_flags);
}

#define ci_waitq_waiter_signalled(waiter, wq)  ((waiter)->signalled)

#define ci_waitq_waiter_timedout(t)            ((t)->timedout)

/*--------------------------------------------------------------------
 *
 *
 *--------------------------------------------------------------------*/

/* macro to convert a mS timeout to a value that can be passed to DDK wait functions */
#define TIMEOUT_MS(ms)           (((ci_int64)(ms)) * -10L*1000L)

ci_inline void
ci_sleep_ms(ulong ms)
{
  LARGE_INTEGER timeout;
  timeout.QuadPart = TIMEOUT_MS(ms);
  KeDelayExecutionThread(KernelMode, FALSE, &timeout);
}


/*--------------------------------------------------------------------
 *
 * udelay - stalls execution for up to 50us
 *
 *--------------------------------------------------------------------*/

/*! Stalls execution for up to 50us */
ci_inline void ci_udelay(unsigned long usec)
{
  KeStallExecutionProcessor(usec);
}

/*--------------------------------------------------------------------
 *
 * Memory Copy To / From user mode (now properly implemented!)
 *
 *--------------------------------------------------------------------*/

NTSTATUS copy_from_user(void *kdest, void*usrc, unsigned len);
int copy_from_user_ret(void *kdest, void*usrc, unsigned len, int retcode);
NTSTATUS copy_to_user(void *udest, void*ksrc, unsigned len);
int copy_to_user_ret(void *udest, void*ksrc, unsigned len, int retcode);


/*--------------------------------------------------------------------
 * Address space: See comments in linux_kernel.h
 *--------------------------------------------------------------------*/

typedef PKTHREAD ci_addr_spc_t;

#define CI_ADDR_SPC_INVALID     ((ci_addr_spc_t)(ci_uintptr_t) 1)
#define CI_ADDR_SPC_KERNEL      ((ci_addr_spc_t)(ci_uintptr_t) 2)
#define CI_ADDR_SPC_CURRENT     ((ci_addr_spc_t)(ci_uintptr_t) 3)

/* Windows does not support copying to arbitrary address spaces. */
#define ci_addr_spc_is_user(as) 0


#endif  /* __CI_DRIVER_PLATFORM_WDM_DRIVER_H__ */
/*! \cidoxg_end */
