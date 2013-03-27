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

#ifndef __CI_TOOLS_WIN32_H__
#define __CI_TOOLS_WIN32_H__

#include <windows.h>
#include <winioctl.h>
#include <winsock.h>
#ifndef __ci_driver__
# include <Mswsock.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <time.h>
#include "win_common.h"

/* #define CI_BOMB()  DebugBreak() */

/* The sleep syscall */
#define sleep(n) (Sleep(n * 1000))

/**********************************************************************
 * User level common includes
 */
#include <ci/tools/platform/ul_common.h>

/**********************************************************************
 * IOVEC
 */
struct iovec {
  void*  iov_base;
  size_t iov_len;
};

/**********************************************************************
 * spinlock implementation: used by <ci/tools/spinlock.h>
 */

#define CI_HAVE_SPINLOCKS

typedef ci_uintptr_t			  ci_lock_holder_t;
#define ci_lock_thisthread  (ci_lock_holder_t)GetCurrentThreadId()
#define ci_lock_no_holder   (ci_lock_holder_t)NULL

typedef CRITICAL_SECTION	ci_lock_i;
typedef CRITICAL_SECTION	ci_irqlock_i;
typedef unsigned long		ci_irqlock_state_t;

#define IRQLOCK_CYCLES  500000

#define ci_lock_ctor_i(l)		InitializeCriticalSection(l)
#define ci_lock_dtor_i(l)		DeleteCriticalSection(l)
#define ci_lock_lock_i(l)		EnterCriticalSection(l)
#define ci_lock_trylock_i(l)		(EnterCriticalSection(l),1)
#define ci_lock_unlock_i(l)		LeaveCriticalSection(l)

#define ci_irqlock_ctor_i(l)		InitializeCriticalSection(l)
#define ci_irqlock_dtor_i(l)		DeleteCriticalSection(l)
ci_inline void ci_irqlock_lock_i(ci_irqlock_i* l, ci_irqlock_state_t* s)
{ EnterCriticalSection(l); }
ci_inline void ci_irqlock_unlock_i(ci_irqlock_i* l, ci_irqlock_state_t* s)
{ LeaveCriticalSection(l); }


/**********************************************************************
 * Basic thread compatability layer.
 */

#if !defined(WINCE)
# include <process.h>
#endif

typedef void* (*cithread_fn)(void* arg);

typedef struct {  /* all members are private */
  HANDLE	thread;
  unsigned int  thread_id;
} cithread_info_t;

typedef cithread_info_t* cithread_t;

typedef struct {  /* all members are private */
  cithread_fn   fn;
  void *        arg;
} cithread_args_t;

unsigned int WINAPI _cithread_main(void *arg);

/* Don't include Win32 API implementation in drivers */
#ifndef __ci_driver__

ci_inline int cithread_create(cithread_t* t, cithread_fn fn, void* arg, const char *name)
{
#if !defined(WINCE) /* ?? think that WINCE uses CreateThread */
  cithread_args_t * ta;

  /* CI_ALLOC_OBJ not yet defined */
  *t = (cithread_info_t*) ci_alloc(sizeof(cithread_info_t));
  if( *t == 0 ) return ENOMEM;
  ta = (cithread_args_t*) ci_alloc(sizeof(cithread_args_t));
  if( !ta ) {
    *t = 0;
    return ENOMEM;
  }

  ta->fn = fn;
  ta->arg = arg;
  (*t)->thread = (HANDLE)_beginthreadex(NULL,
					0x1000,  /* hard coded stack size */
					_cithread_main,
					(void*) ta,
					0, &(*t)->thread_id);
  if( !(*t)->thread ) {
    ci_free(ta);
    ci_free(*t);
    *t = 0;
    return EAGAIN;
  }
  return 0;
#else
  *t = 0;
  return EAGAIN;
#endif
}


ci_inline int cithread_detach(cithread_t t)
{
  CloseHandle(t->thread);
  ci_free(t);
  return 0;
}


ci_inline int cithread_join(cithread_t t)
{
  WaitForSingleObject(t->thread, INFINITE);
  CloseHandle(t->thread);
  ci_free(t);
  return 0;
}

#endif /* !__ci_driver__ */

/**********************************************************************
 * Condition variable implementation.
 */

struct _cithread_cond_lnk_t;
typedef struct _cithread_cond_lnk_t cithread_cond_lnk_t;


typedef struct {
  CRITICAL_SECTION * mutex;	/* external mutex for condition variable */
  CRITICAL_SECTION  crit;	/* internal synchronisation */

  cithread_cond_lnk_t *head;	/* doubly linked list of waiting threads */
  cithread_cond_lnk_t *tail;

} cithread_cond_t;


extern void cithread_cond_ctor(cithread_cond_t *c, CRITICAL_SECTION * i);
extern void cithread_cond_dtor(cithread_cond_t *c);
extern int  cithread_cond_wait(cithread_cond_t *c, int *usec);
extern int  cithread_cond_signal(cithread_cond_t *c);
extern int  cithread_cond_broadcast(cithread_cond_t *c);

#define     cithread_cond_timedwait  cithread_cond_wait


/**********************************************************************
 * Error codes.
 */

/* Standarised on Linux-style Exxx codes and associated values throughout
 * the Efab core code.  See win_common.h in this directory */

/**********************************************************************
 * Thread-safe strtok
 */

#define ci_strtok_local(_ptr)
#define ci_strtok(_s, _delim, _ptrptr) strtok(_s, _delim)

#if !defined(__ci_driver__)
#include "MSWsock.h"
#include "ws2tcpip.h"

typedef WSABUF ci_iovec;

struct msghdr {
  char*         msg_name;
  socklen_t     msg_namelen;
  ci_iovec*     msg_iov;
  size_t        msg_iovlen;
  char*         msg_control;
  socklen_t     msg_controllen;
  int           msg_flags;
};
#else
/* copy of WSABUF for driver */
typedef struct ci_iovec_s {
  u_long      len;     /* the length of the buffer */
  char FAR *  buf;     /* the pointer to the buffer */
} ci_iovec;

#endif

/* Accessors for buffer/length in [msg_iov] */
#define CI_IOVEC_BASE(i) ((i)->buf)
#define CI_IOVEC_LEN(i)  ((i)->len)

/* Flags not defined in WinSock2, def'd to 0 so they cannot be set
 * or return a false positive in tests etc. */
# define MSG_EOR      0
# define MSG_CONFIRM  0

/* (moved from ip.h)
 * I think its very unlikely that win32 doesn't support any of these.
 * They must be putting some prefix on them or something.
 * XXX: Assigned temporary values. Check that they don't interfere with
 * other MSG options/values.  FIXME
 * Wrong value for MSG_DONTWAIT - FIXEDABIT (doesn't clash with MSG_xxx
 * in winsock2.h)
 */
# define MSG_DONTWAIT		0x40
//# define MSG_WAITALL		0x100
# define MSG_ERRQUEUE		0x200


/* Is there a cheap way to tell whether an application is multithread on
** win32?
*/
#define ci_is_multithreaded()	1

#ifndef _OFF_T_DEFINED
typedef long _off_t;                /* file offset value */
#if     !__STDC__
/* Non-ANSI name for compatibility */
typedef long off_t;
#endif
#define _OFF_T_DEFINED
#endif

#endif  /* __CI_TOOLS_WIN32_H__ */

/*! \cidoxg_end */
