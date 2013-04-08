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

/*! \cidoxg_include_ci_tools_platform  */

#ifndef __CI_TOOLS_WIN32_KERNEL_H__
#define __CI_TOOLS_WIN32_KERNEL_H__


#include <stdio.h>
#include <limits.h>
#include "win_common.h"

/*
 * Get a proper OS!
 */
#define	IPPROTO_IP	0
#define	IPPROTO_ICMP	1
#define	IPPROTO_IGMP	2
#define	IPPROTO_TCP	6
#define	IPPROTO_UDP	17

#define CI_BOMB()  DbgBreakPoint()


#define CI_LOG_FN_DEFAULT  ci_log_syslog

typedef int ssize_t;


/**********************************************************************
 * IOVEC abstraction
 */
/*! \TODO There are places where this is still used, need to confirm that
 *  they're safe or replace them. */
/*struct iovec {
  void*  iov_base;
  size_t iov_len;
  };*/

/* Copy of WSABUF */

typedef struct ci_iovec_s {
  char * buf;
  unsigned long len;
} ci_iovec;

/* Accessors for buffer/length in [msg_iov] */
#define CI_IOVEC_BASE(i) ((i)->buf)
#define CI_IOVEC_LEN(i)  ((i)->len)


/* Probably want to move this elsewhere in this file at some point */
typedef int socklen_t;
struct msghdr {
  char*         msg_name;
  socklen_t     msg_namelen;
  ci_iovec*     msg_iov;
  size_t        msg_iovlen;
  char*         msg_control;
  socklen_t     msg_controllen;
  int           msg_flags;
};

/*--------------------------------------------------------------------
 *
 * sockaddr - part of Winsock2.h (necessary for ip.h)
 *
 *--------------------------------------------------------------------*/

/*
 * Internet address (old style... should be updated)
 */
struct in_addr {
        union {
                struct { unsigned char s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { unsigned short s_w1,s_w2; } S_un_w;
                unsigned long S_addr;
        } S_un;
#define s_addr  S_un.S_addr       /* can be used for most tcp & ip code */
#define s_host  S_un.S_un_b.s_b2  /* host on imp */
#define s_net   S_un.S_un_b.s_b1  /* network */
#define s_imp   S_un.S_un_w.s_w2  /* imp */
#define s_impno S_un.S_un_b.s_b4  /* imp # */
#define s_lh    S_un.S_un_b.s_b3  /* logical host */
};

/*
 * Definitions of bits in internet address integers.
 * On subnets, the decomposition of addresses to host and net parts
 * is done according to subnet mask, not the masks here.
 */
#define IN_CLASSA(i)            (((long)(i) & 0x80000000) == 0)
#define IN_CLASSA_NET           0xff000000
#define IN_CLASSA_NSHIFT        24
#define IN_CLASSA_HOST          0x00ffffff
#define IN_CLASSA_MAX           128

#define IN_CLASSB(i)            (((long)(i) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET           0xffff0000
#define IN_CLASSB_NSHIFT        16
#define IN_CLASSB_HOST          0x0000ffff
#define IN_CLASSB_MAX           65536

#define IN_CLASSC(i)            (((long)(i) & 0xe0000000) == 0xc0000000)
#define IN_CLASSC_NET           0xffffff00
#define IN_CLASSC_NSHIFT        8
#define IN_CLASSC_HOST          0x000000ff

#define IN_CLASSD(i)            (((long)(i) & 0xf0000000) == 0xe0000000)
#define IN_CLASSD_NET           0xf0000000       /* These ones aren't really */
#define IN_CLASSD_NSHIFT        28               /* net and host fields, but */
#define IN_CLASSD_HOST          0x0fffffff       /* routing needn't know.    */
#define IN_MULTICAST(i)         IN_CLASSD(i)

#define INADDR_ANY              (unsigned long)0x00000000
#define INADDR_LOOPBACK         0x7f000001
#define INADDR_BROADCAST        (unsigned long)0xffffffff
#define INADDR_NONE             0xffffffff

#define ADDR_ANY                INADDR_ANY

 /*
 * Structure used by kernel to store most
 * addresses.
 */
struct sockaddr {
        unsigned short sa_family;     /* address family */
        char           sa_data[14];   /* up to 14 bytes of direct address */
};

/*
 * Socket address, internet style.
 */
struct sockaddr_in {
        short   sin_family;
        unsigned short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8];
};

// Stolen from winsock2.h
#define MSG_OOB         0x1             /* process out-of-band data */
#define MSG_PEEK        0x2             /* peek at incoming message */
#define MSG_DONTROUTE   0x4             /* send without using routing tables */

#define MSG_DONTWAIT    0x40
#define MSG_WAITALL     0x100
#define MSG_PARTIAL     0x8000




#if defined(__ci_wdm__) || defined (__ci_iscsi__)

/*--------------------------------------------------------------------
 *
 * WDM spinlock implementation - not to be used at DIRQL. Any functions which
 * require a lock must be run at IRQL <= DISPATCH_LEVEL
 *
 *--------------------------------------------------------------------*/

#define CI_HAVE_SPINLOCKS

typedef ci_uintptr_t        ci_lock_holder_t;
#define ci_lock_thisthread	(ci_lock_holder_t)KeGetCurrentThread()
#define ci_lock_no_holder	  (ci_lock_holder_t)NULL

typedef struct {
  KSPIN_LOCK  lock;
  KIRQL       irql;
} ci_lock_i;

#define ci_lock_ctor_i(l)         KeInitializeSpinLock(&(l)->lock)
#define ci_lock_dtor_i(l)	  do {} while (0)

#define ci_lock_lock_i(l)	                                              \
  do {                                                                        \
    KIRQL irql;                                                               \
    ci_assert(KeGetCurrentIrql() <= DISPATCH_LEVEL);                          \
    KeAcquireSpinLock(&(l)->lock, &irql);                                     \
    (l)->irql = irql;                                                         \
  } while (0)

#if defined(_WIN32) && defined(_PREFAST_)
/* We're not really compiling this code we're doing a static code analysis
   and we want the "prefast" compiler to treat these functions as IRQL-
   changing functions.  We do this by turning them into a macro calls to the
   function that prefast does know about...
*/

#define ci_lock_trylock_i(l)                                                  \
    (KeAcquireSpinLock(&(l)->lock, &(l)->irql), 1)

#else

ci_inline int ci_lock_trylock_i(ci_lock_i* l) {
  KIRQL irql;
#ifndef NDEBUG
  if( CI_UNLIKELY(KeGetCurrentIrql() > DISPATCH_LEVEL) ) {
    DbgPrint("ci_lock_trylock_i: IRQL > DISPATCH_LEVEL\n");
    CI_BOMB();
  }
#endif
  KeAcquireSpinLock(&l->lock, &irql);
  l->irql = irql;
  return 1;
}

#endif /* _PREFAST_ */

#define ci_lock_unlock_i(l)	                                              \
  do {                                                                        \
    KIRQL irql = (l)->irql;                                                   \
    ci_assert(KeGetCurrentIrql() == DISPATCH_LEVEL);                          \
    KeReleaseSpinLock(&(l)->lock, irql);                                      \
  } while (0)

typedef struct _ci_irqlock_i {
  KSPIN_LOCK lock;
  PKSPIN_LOCK plock;
} ci_irqlock_i;

typedef KIRQL			  ci_irqlock_state_t;

#define IRQLOCK_CYCLES  500000

#define ci_irqlock_ctor_i(l)	  do { KeInitializeSpinLock(&(l)->lock);\
                                       (l)->plock = &(l)->lock; } while(0)
#define ci_irqlock_dtor_i(l)	  do {} while (0)

#define ci_irqlock_lock_i(l,s)	                                              \
  do {                                                                        \
    ci_assert(KeGetCurrentIrql() <= DISPATCH_LEVEL);                          \
    KeAcquireSpinLock((l)->plock,s);                                          \
  } while (0)

#define ci_irqlock_unlock_i(l,s)                                              \
  do {                                                                        \
    ci_assert(KeGetCurrentIrql() == DISPATCH_LEVEL);                          \
    KeReleaseSpinLock((l)->plock,*(s));                                       \
  } while (0)



/*--------------------------------------------------------------------
 * WDM counted semaphore implementation
 * Allowable IRQL varies between functions, as indicated for each below
 *--------------------------------------------------------------------*/

typedef KSEMAPHORE ci_semaphore_t;

/* IRQL = PASSIVE_LEVEL */
ci_inline void
ci_sem_init (ci_semaphore_t *sem, int val) {
  ci_assert_equal(KeGetCurrentIrql(), PASSIVE_LEVEL);
  KeInitializeSemaphore(sem, val, 0x7fffffff);
}

/* IRQL <= APC_LEVEL */
ci_inline void
ci_sem_down (ci_semaphore_t *sem) {
  ci_assert_le(KeGetCurrentIrql(), APC_LEVEL);
  KeWaitForSingleObject(sem, Executive, KernelMode, FALSE, NULL);
}

/* IRQL <= DISPATCH_LEVEL */
ci_inline int
ci_sem_trydown (ci_semaphore_t *sem) {
  LARGE_INTEGER timeout;
  ci_assert_le(KeGetCurrentIrql(), DISPATCH_LEVEL);
  timeout.LowPart=0;
  timeout.HighPart=0;

  /* Need to return 0 for success, non-0 for failure */
  return KeWaitForSingleObject(sem, Executive, KernelMode, FALSE, &timeout)
    !=STATUS_SUCCESS;
}

/* IRQL <= DISPATCH_LEVEL */
ci_inline void
ci_sem_up (ci_semaphore_t *sem) {
  ci_assert_le(KeGetCurrentIrql(), DISPATCH_LEVEL);
  KeReleaseSemaphore(sem, 0, 1, FALSE);
}

/* Any IRQL */
ci_inline int
ci_sem_get_count(ci_semaphore_t *sem) {
  return KeReadStateSemaphore(sem);
}


/*--------------------------------------------------------------------
 *
 *
 *--------------------------------------------------------------------*/

//#define CI_MEMLEAK_DEBUG

/* memory tags used by driver in calls to AllocateWithtag */
#define CIWDM_BLOCK_TAG           'mwfE'

#ifdef CI_MEMLEAK_DEBUG

/* replace ci_alloc with an allocator with some instrumentation */
void * ci_memdbg_alloc(size_t n, char * file, int line);
void * ci_memdbg_allocfn(size_t n);
void * ci_memdbg_vmallocfn(size_t n);

#define ci_alloc(n)          ci_memdbg_alloc((n), __FILE__, __LINE__)
#define ci_alloc_nonpaged(n) ci_memdbg_alloc((n), __FILE__, __LINE__)
/* for where code needs a function with the correct prototype */
#define ci_alloc_fn        ci_memdbg_allocfn
#define ci_vmalloc_fn      ci_memdbg_vmallocfn

#define ci_vmalloc(n)      ci_memdbg_alloc((n), __FILE__, __LINE__)
#define ci_atomic_alloc(n) ci_memdbg_alloc((n), __FILE__, __LINE__)
#else

/* by default everything uses standard allocator */
#define ci_alloc        __ci_alloc
#define ci_alloc_fn     __ci_alloc
#define ci_vmalloc      __ci_alloc
#define ci_vmalloc_fn   __ci_alloc
#define ci_atomic_alloc __ci_alloc

#define ci_alloc_nonpaged __ci_alloc_nonpaged

#endif

ci_inline void *
__ci_alloc(size_t n)
{
  /*
   * TODO - not convinced that the efab code will support PagedPool -
   * therefore changing to NonPagedPool. Need to review source
   * and ensure code correctly calls ci_alloc[_nonpaged]
   * - Currently nothing calls nonpaged. aam
   */
  return ExAllocatePoolWithTag(NonPagedPool, n, CIWDM_BLOCK_TAG);
}

ci_inline void *
__ci_alloc_nonpaged(size_t n)
{
  return ExAllocatePoolWithTag(NonPagedPool, n, CIWDM_BLOCK_TAG);
}

ci_inline void
ci_free(void* p)
{
  ExFreePool(p);
}

#define ci_vfree    ci_free

#define ci_sprintf    sprintf
#define ci_snprintf   _snprintf
#define ci_vsprintf   vsprintf
#define ci_vsnprintf  vsnprintf
#define ci_sscanf     sscanf

ci_inline void ci_initialize_lookaside_list(NPAGED_LOOKASIDE_LIST *lookaside, 
                                            size_t size)
{
  ExInitializeNPagedLookasideList(lookaside,
                                  NULL, NULL,/* use default allocate and free */
                                  0,  /* flags */
                                  size,
                                  CIWDM_BLOCK_TAG,
                                  0);
}
#define ci_delete_lookaside_list ExDeleteNPagedLookasideList
#define ci_free_to_lookaside_list ExFreeToNPagedLookasideList
#define ci_alloc_from_lookaside_list ExAllocateFromNPagedLookasideList


/**********************************************************************
 * thread implementation
 */

typedef struct {
  const char*		name;
  PVOID    	        thread;
} ci_kernel_thread_t;


typedef ci_kernel_thread_t* cithread_t;


extern int cithread_create(cithread_t* tid, void* (*fn)(void*), void* arg,
			   const char* name);
extern int cithread_detach(cithread_t kt);
extern int cithread_join(cithread_t kt);

#endif  /* __ci_wdm__ */

/*--------------------------------------------------------------------
 *
 * in_interrupt and in_atomic macros .. well portable code shouldn't
 * be using it, and definitely not for anything other than picking
 * which memory alloc routine to use
 *
 * DJR: which begs the question "Why are we defining them here?  Is this a
 * deliberate attempt to make it easy to introduce bugs into the code
 * base?"
 *
 *--------------------------------------------------------------------*/

#define ci_in_interrupt() 0
#define ci_in_atomic() 0
#define ci_in_irq() 0

/*
from Linux header param.h
*/
#ifdef __KERNEL__
# define HZ		1000		/* Internal kernel timer frequency */
# define USER_HZ	100		/* .. some user interfaces are in "ticks" */
# define CLOCKS_PER_SEC	(USER_HZ)	/* like times() */
#endif

#ifndef HZ
#define HZ 100
#endif
/*
*/

#ifndef _OFF_T_DEFINED
typedef long _off_t;                /* file offset value */
#if     !__STDC__
/* Non-ANSI name for compatibility */
typedef long off_t;
#endif
#define _OFF_T_DEFINED
#endif

#endif  /* __CI_TOOLS_WIN32_KERNEL_H__ */

/*! \cidoxg_end */
