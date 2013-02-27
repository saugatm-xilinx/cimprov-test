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

#ifndef __CI_TOOLS_SUNOS_KERNEL_H__
#define __CI_TOOLS_SUNOS_KERNEL_H__

#include  <sys/types.h>
#include  <sys/kmem.h>
#include  <sys/sysmacros.h>
#include  <sys/cmn_err.h>
#include  <sys/ksynch.h>
#include  <sys/ddi.h>
#include  <sys/sunddi.h>
#include  <sys/thread.h>
#include  <sys/promif.h>
#include  <netinet/in.h>


#undef  u

#define HZ  (hz)

#define ci_alloc_fn     __ci_alloc		/* inline below */
#define ci_alloc        __ci_alloc
#define ci_atomic_alloc __ci_alloc
#define ci_free         __ci_free

#define ci_vmalloc      __ci_alloc
#define ci_vmalloc_fn   __ci_alloc
#define ci_vfree        __ci_free

extern int    ci_sscanf(const char *buf, const char *fmt, ...);

#define ci_snprintf     (int)snprintf
#define ci_vsnprintf    (int)vsnprintf

#define CI_BOMB()          panic("CI_BOMB")
#define CI_LOG_FN_DEFAULT  ci_log_syslog


/**********************************************************************
 * spinlock implementation: used by <ci/tools/spinlock.h>
 */

#define CI_HAVE_SPINLOCKS

typedef ci_uintptr_t       ci_lock_holder_t;
#define ci_lock_thisthread (ci_lock_holder_t)curthread
#define ci_lock_no_holder  (ci_lock_holder_t)NULL

typedef kmutex_t ci_lock_i;
typedef kmutex_t ci_irqlock_i;

typedef kthread_t *ci_irqlock_state_t;

#define ci_lock_ctor_i(l)    mutex_init(l, NULL, MUTEX_ADAPTIVE, NULL)
#define ci_lock_dtor_i(l)    mutex_destroy(l)
#define ci_lock_lock_i(l)    mutex_enter(l)
#define ci_lock_trylock_i(l) mutex_tryenter(l)
#define ci_lock_unlock_i(l)  mutex_exit(l)

/*
 * Solaris interrupts do not pre-empt the scheduler so we can use mutexes
 * and the cycle count while a lock is held is therefore somewhat
 * unpredictable so make the warning limit very large.
 */
#define IRQLOCK_CYCLES 0xffffffff

#define ci_irqlock_ctor_i(l)     mutex_init(l, NULL, MUTEX_ADAPTIVE, NULL)
#define ci_irqlock_dtor_i(l)	 mutex_destroy(l)

#define ci_irqlock_lock_i(l, s) \
  do { \
    mutex_enter(l); \
    *s = curthread; \
  } while (0);

#define ci_irqlock_unlock_i(l, s) \
  do { \
    ci_assert_equal(*s, curthread); \
    ci_lock_unlock_i(l); \
    *s = NULL; \
  } while (0);

/**********************************************************************
 * threads
 */

typedef struct {
  const char *name;
  void *(*fn)(void* arg);
  void *arg;
  volatile kt_did_t tid;
} ci_kernel_thread_t;

typedef ci_kernel_thread_t* cithread_t;

extern int cithread_create(cithread_t* tidp, void* (*fn)(void*), void* arg,
			   const char* name);
extern int cithread_detach(cithread_t tid);
extern int cithread_join(cithread_t tid);


/**********************************************************************
 * struct iovec abstraction (for Windows port)
 */

typedef struct iovec ci_iovec;

/* Accessors for buffer/length */
#define CI_IOVEC_BASE(i) ((i)->iov_base)
#define CI_IOVEC_LEN(i)  ((i)->iov_len)


/**********************************************************************
 * meaningless macros
 */

#define ci_in_interrupt() 0
#define ci_in_atomic()    0
#define ci_in_irq()       0

/**********************************************************************
 * Signals
 */

ci_inline void
ci_send_sig(int signum)
{
  void *pref = proc_ref();

  proc_signal(pref, signum);
  proc_unref(pref);
}


/* Can't use kernel sprintf due to return semantics difference vs. stdio */
ci_inline int
ci_sprintf(char *buf, const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);
  (void) vsnprintf(buf, INT_MAX, fmt, args);
  va_end(args);

  return strlen(buf);
}

ci_inline int
ci_vsprintf(char *buf, const char *fmt, va_list args)
{
  (void) vsnprintf(buf, INT_MAX, fmt, args);
  return strlen(buf);
}

ci_inline void *
__ci_alloc(size_t n)
{
  caddr_t buf;
  size_t size;

  ci_assert_gt(n, 0);

  size = n + (2 * sizeof (size_t));
  if ((buf = kmem_alloc(size, KM_NOSLEEP)) == NULL)
   return NULL;

  *((size_t *)(buf)) = size;
  buf += sizeof (size_t);
  *((size_t *)(buf + n)) = size;

  return (void *)buf;
}

ci_inline void
__ci_free(void *p)
{
  caddr_t buf = p;
  size_t size1;
  size_t size2;
  size_t n;

  ci_assert(buf);
  buf -= sizeof (size_t);

  size1 = *((size_t *)(buf));  
  n = size1 - sizeof (size_t);
  size2 = *((size_t *)(buf + n));  

  ci_assert_equal(size1, size2);
  kmem_free((void *)buf, size1);
}

#endif  /* __CI_TOOLS_SUNOS_KERNEL_H__ */
/*! \cidoxg_end */

/*
 * vi: sw=2:ai:aw
 * vim: et:ul=0
 */ 
