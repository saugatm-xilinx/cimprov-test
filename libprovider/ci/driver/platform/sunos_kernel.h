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

#ifndef __CI_DRIVER_PLATFORM_SUNOS_KERNEL_H__
#define __CI_DRIVER_PLATFORM_SUNOS_KERNEL_H__


/**********************************************************************
 * Kernel headers.
 */

#include <sys/types.h>
#include <sys/sysmacros.h>
#include <sys/cmn_err.h>
#include <sys/kmem.h>
#include <sys/vmem.h>
#include <sys/mkdev.h>
#include <sys/errno.h>
#include <sys/cpuvar.h>
#include <sys/stat.h>
#include <sys/conf.h>
#include <sys/ddi.h>
#include <sys/sunddi.h>
#include <sys/esunddi.h>
#include <sys/modctl.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/pci.h>
#include <sys/stream.h>
#include <sys/strsun.h>
#include <sys/stropts.h>
#include <sys/atomic.h>
#include <sys/cyclic.h>
#include <sys/sunndi.h>
#include <sys/ndi_impldefs.h>
#include <sys/sunldi.h>
#include <sys/termios.h>
#include <sys/file.h>
#include <sys/fs/snode.h>
#include <netinet/in.h>
#include <sys/systm.h>
#include <sys/socketvar.h>
#include <sys/cred.h>
#include <sys/policy.h>
#include <sys/priv.h>
#include <sys/priv_names.h>
#include <sys/poll.h>
#include <sys/poll_impl.h>
#include <sys/mutex_impl.h>
#include <net/if.h>
#include <net/route.h>
#include <inet/mib2.h>
#include <inet/common.h>
#include <inet/ip.h>
#include <inet/arp.h>
#include <net/if_arp.h>
#include <sys/tihdr.h>
#include <net/if_types.h>
#include <sys/ethernet.h>
#include <fcntl.h>
#include <sys/sdt.h>

/*
 * Virtually contiguous buffer i.e. multiple pages
 */

struct {
  unsigned long n;
  caddr_t base;
  ddi_umem_cookie_t cookie;
} efhw_pages;

typedef struct efhw_pages  ci_contig_shmbuf_t;

extern int            efhw_pages_alloc(struct efhw_pages *, unsigned long);
extern void           efhw_pages_free(struct efhw_pages *);
extern int            efhw_mmap_pages(struct efhw_pages *, unsigned int,
                                    unsigned long *, void *, int *,
                                    unsigned long *);

ci_inline caddr_t
efhw_pages_ptr(struct efhw_pages *p)
{
  return p->base;
}

ci_inline unsigned long
efhw_pages_n(struct efhw_pages *p)
{
  return p->n;
}

/*
 * Single page
 */

struct {
  ci_pages_t page;
} efhw_page;

#define efhw_page_ptr(p)        efhw_pages_ptr(&(p).page)

#define efhw_page_is_valid(p)   (efhw_page_ptr(p) != NULL)

/*
 * Physically contiguous shared memory buffer
 */

#define ci_contig_shmbuf_alloc(s, size) \
        efhw_pages_alloc((s), (size + CI_PAGE_SIZE - 1) >> CI_PAGE_SHIFT);

#define efhw_contig_shmbuf_free(s)  efhw_pages_free(&(s)->page)
#define efhw_contig_shmbuf_ptr(s)   efhw_pages_ptr(&(s)->page)
#define efhw_contig_shmbuf_size(s)  (ci_pages_n(&(s)->page) << CI_PAGE_SHIFT)

#define efhw_contig_shmbuf_mmap(s, offset, bytes, opaque, map_num, p_offset) \
        ci_mmap_pages(&(s)->page, (offset), (bytes), (opaque), \
                      (map_num), (p_offset))

/*--------------------------------------------------------------------
 *
 * ci_waitable_t: A Windows friendly blocking abstraction.
 *
 *--------------------------------------------------------------------*/

#define CI_BLOCKING_CTX_ARG(x)

typedef struct {
  kcondvar_t  cv;
  kmutex_t    lock;
  uint_t      nwaiters;
} ci_waitable_t;

typedef kthread_t * ci_waiter_t;

typedef clock_t ci_waitable_timeout_t;

#define ci_waitable_init_timeout(t, timeval) \
do { \
  if (ci_waitq_wait_forever(timeval)) { \
    *(t) = -1; \
  } else { \
    *(t) = 0; \
    if ((timeval)->tv_usec > 0) \
      *(t) += drv_usectohz((timeval)->tv_usec); \
    if ((timeval)->tv_sec > 0) \
      *(t) += drv_usectohz((timeval)->tv_sec * 1000000ul); \
  } \
} while(0)

typedef int (*ci_waiter_on_wakeup_fn)(ci_waiter_t*, void*, void*, int rc);


ci_inline void
ci_waitable_ctor(ci_waitable_t *w)
{
  cv_init(&(w->cv), NULL, CV_DRIVER, NULL); 
  mutex_init(&(w->lock), NULL, MUTEX_DRIVER, NULL);
  w->nwaiters = 0;
}

ci_inline void
ci_waitable_dtor(ci_waitable_t *w)
{
  ci_assert_equal(w->nwaiters, 0);

  mutex_destroy(&(w->lock));
  cv_destroy(&(w->cv));
}

#define ci_waitable_active(w)	((w)->nwaiters > 0)

ci_inline void
ci_waitable_wakeup_one(ci_waitable_t *w)
{
  if (w->nwaiters == 0)
    return;

  mutex_enter(&(w->lock));
  cv_signal(&(w->cv));
  mutex_exit(&(w->lock));
}

ci_inline void
ci_waitable_wakeup_all(ci_waitable_t *w)
{
  if (w->nwaiters == 0)
    return;

  mutex_enter(&(w->lock));
  cv_broadcast(&(w->cv));
  mutex_exit(&(w->lock));
}

ci_inline int
__ci_waiter_pre(ci_waiter_t *context, ci_waitable_t* w) {
  mutex_enter(&(w->lock));
  *context = curthread;
  w->nwaiters++;
  return 0;
}
#define ci_waiter_pre(waiter, waitable, context)            __ci_waiter_pre((waiter), (waitable))
#define ci_waiter_exclusive_pre(waiter, waitable, context)  __ci_waiter_pre((waiter), (waitable))

ci_inline void
__ci_waiter_post(ci_waiter_t *context, ci_waitable_t* w) {
  ci_assert_equal(*context, curthread);
  ci_assert_gt(w->nwaiters, 0);
  --w->nwaiters;
  mutex_exit(&(w->lock));
}
#define ci_waiter_post(waiter, waitable)      __ci_waiter_post((waiter), (waitable))
#define ci_waiter_dont_wait(waiter, waitable) __ci_waiter_post((waiter), (waitable))

#define ci_waiter_prepare_continue_to_wait(waiter, waitable)
#define ci_waiter_dont_continue_to_wait(waiter, waitable)

#define CI_WAITER_CONTINUE_TO_WAIT	1
#define CI_WAITER_CONVERT_REENTRANT(x)    (x)

ci_inline int
ci_waiter_wait(ci_waiter_t* context, ci_waitable_t* w,
               ci_waitable_timeout_t *timeout,
               void* opaque1, void* opaque2,
               ci_waiter_on_wakeup_fn on_wakeup)
{
  int rc;

  ci_assert_equal(*context, curthread);

  for(;;) {

    ci_assert(mutex_owned(&(w->lock)));
    if( timeout && *timeout >= 0 ) {
      clock_t ticks;

      ticks = ddi_get_lbolt();
      ticks += *timeout;

      switch (cv_timedwait_sig(&(w->cv), &(w->lock), ticks)) {
      case 0:
        rc = -EINTR;
        break;
      case -1:
        *timeout = 0;
        rc = -ETIMEDOUT;
        break;
      default:
        rc = 0;
        break;
      }
    } else {
      if (cv_wait_sig(&(w->cv), &(w->lock)) == 0) {
        rc = -EINTR;
      } else {
        rc = 0;
      }
    }
    ci_assert(mutex_owned(&(w->lock)));

    rc = on_wakeup(context, opaque1, opaque2, rc);
    if( rc != CI_WAITER_CONTINUE_TO_WAIT )
      break;
  }

  return rc;
}


/*--------------------------------------------------------------------
 *
 * ci_waitq_t: Very similar to the above ci_waitable_t but not
 *             Windows friendly.
 *
 *--------------------------------------------------------------------*/


typedef struct {
  kcondvar_t  cv;
  kmutex_t    lock;
  uint_t      nwaiters;
  int         rc;
} ci_waitq_t;

typedef kthread_t * ci_waitq_waiter_t;

typedef clock_t ci_waitq_timeout_t;

#define ci_waitq_init_timeout(t, timeval) \
do { \
  if (ci_waitq_wait_forever(timeval)) { \
    *(t) = -1; \
  } else { \
    *(t) = 0; \
    if ((timeval)->tv_usec > 0) \
      *(t) += drv_usectohz((timeval)->tv_usec); \
    if ((timeval)->tv_sec > 0) \
      *(t) += drv_usectohz((timeval)->tv_sec * 1000000ul); \
  } \
} while(0)

ci_inline void
ci_waitq_ctor(ci_waitq_t *w)
{
  cv_init(&(w->cv), NULL, CV_DRIVER, NULL); 
  mutex_init(&(w->lock), NULL, MUTEX_DRIVER, NULL);
  w->nwaiters = 0;
  w->rc = -EFAULT;
}

ci_inline void
ci_waitq_dtor(ci_waitq_t *w)
{
  ci_assert_equal(w->nwaiters, 0);

  mutex_destroy(&(w->lock));
  cv_destroy(&(w->cv));
}

#define ci_waitq_active(w)	((w)->nwaiters > 0)

ci_inline void
ci_waitq_wakeup(ci_waitq_t *w)
{
  if (w->nwaiters == 0)
    return;

  mutex_enter(&(w->lock));
  cv_signal(&w->cv);
  mutex_exit(&(w->lock));
}

ci_inline void
ci_waitq_wakeup_all(ci_waitq_t *w)
{
  if (w->nwaiters == 0)
    return;

  mutex_enter(&(w->lock));
  cv_broadcast(&w->cv);
  mutex_exit(&(w->lock));
}

ci_inline int
__ci_waitq_waiter_pre(ci_waitq_waiter_t *context, ci_waitq_t* w)
{
  mutex_enter(&(w->lock));
  *context = curthread;
  w->nwaiters++;
  w->rc = -EINPROGRESS;
  return 0;
}
#define ci_waitq_waiter_pre(waiter, waitq)            __ci_waitq_waiter_pre((waiter), (waitq))
#define ci_waitq_waiter_exclusive_pre(waiter, waitq)  __ci_waitq_waiter_pre((waiter), (waitq))

ci_inline void
ci_waitq_waiter_post(ci_waitq_waiter_t *context, ci_waitq_t* w)
{
  ci_assert_equal(*context, curthread);
  ci_assert_gt(w->nwaiters, 0);

  --w->nwaiters;
  mutex_exit(&(w->lock));
}

ci_inline void
ci_waitq_waiter_wait(ci_waitq_waiter_t *context, ci_waitq_t *w, int cond)
{
  ci_assert_equal(*context, curthread);
  ci_assert(mutex_owned(&(w->lock)));
  ci_assert(w->rc == -EINPROGRESS);

  if (cond) {
    w->rc = 0;
    return;
  }

  if (cv_wait_sig(&(w->cv), &(w->lock)) == 0)
    w->rc = -ERESTART;
  else
    w->rc = 0;
}

ci_inline void
ci_waitq_waiter_timedwait(ci_waitq_waiter_t *context, ci_waitq_t *w, int cond, ci_waitq_timeout_t *timeout)
{
  clock_t ticks;

  ci_assert_equal(*context, curthread);
  ci_assert(mutex_owned(&(w->lock)));
  ci_assert(w->rc == -EINPROGRESS);

  if (cond) {
    w->rc = 0;
    return;
  }

  ticks = ddi_get_lbolt();
  ticks += *timeout;

  switch (cv_timedwait_sig(&(w->cv), &(w->lock), ticks)) {
  case 0:
    w->rc = -ERESTART;
    break;
  case -1:
    *timeout = 0;
    w->rc = -ETIMEDOUT;
    break;
  default:
    w->rc = 0;
    break;
  }
}

ci_inline int
ci_waitq_waiter_again(ci_waitq_waiter_t *__unused, ci_waitq_t* w)
{
  w->rc = -EINPROGRESS;
  return 0;
}

ci_inline int
ci_waitq_waiter_signalled(ci_waitq_waiter_t *__unused, ci_waitq_t *w)
{
  return (w->rc == -ERESTART);
}

#define ci_waitq_waiter_timedout(timeout)         (*(timeout) == 0)

/*--------------------------------------------------------------------
 * PCI
 *--------------------------------------------------------------------*/

#define PCI_BASE_ADDRESS_SPACE          PCI_BASE_SPACE_M
#define PCI_BASE_ADDRESS_SPACE_MEMORY   PCI_BASE_SPACE_MEM
#define PCI_BASE_ADDRESS_SPACE_IO       PCI_BASE_SPACE_IO

#define PCI_BASE_ADDRESS_MEM_MASK       PCI_BASE_M_ADDR_M
#define PCI_BASE_ADDRESS_IO_MASK        PCI_BASE_IO_ADDR_M

/*--------------------------------------------------------------------
 * Misc.
 *--------------------------------------------------------------------*/

#define ci_udelay(us) drv_usecwait(us)

ci_inline void
ci_sleep_ms(uint_t ms)
{
  uint_t us = drv_usectohz(ms * 1000);

  delay(us);
}

ci_inline int
copy_to_user(void *udst, void *ksrc, size_t len)
{
  return copyout(ksrc, udst, len);
}

ci_inline int
copy_from_user(void *kdst, void *usrc, size_t len)
{
  return copyin(usrc, kdst, len);
}

ci_inline int
copy_to_user_ret(void *udst, void *ksrc, size_t len, int retval)
{
  return (copyout(ksrc, udst, len) == 0 ? 0 : retval);
}

ci_inline int
copy_from_user_ret(void *kdst, void *usrc, size_t len, int retval)
{
  return (copyin(usrc, kdst, len) == 0 ? 0 : retval);
}


/*--------------------------------------------------------------------
 * Address space: See comments in linux_kernel.h
 *--------------------------------------------------------------------*/

typedef struct as* ci_addr_spc_t;

#define CI_ADDR_SPC_INVALID     ((ci_addr_spc_t)(ci_uintptr_t) 1)
#define CI_ADDR_SPC_KERNEL      ((ci_addr_spc_t)(ci_uintptr_t) 2)
#define CI_ADDR_SPC_CURRENT     ((ci_addr_spc_t)(ci_uintptr_t) 3)

#define ci_addr_spc_is_user(as) (((ci_uintptr_t) (as) & 0x3) == 0u)

/* Pinbuf pages */
extern struct page* ci_follow_page(ci_addr_spc_t addr_spc, caddr_t address);
extern void *ci_kmap_in_atomic(struct page *page);
extern void ci_kunmap_in_atomic(struct page *page, void* kaddr);

#define put_page(p)  /*empty*/	/*XXX stub */

/*--------------------------------------------------------------------
 * File descriptor manipulation.
 *--------------------------------------------------------------------*/

extern void ci_get_file(file_t *);
extern void ci_fput(file_t *);
extern file_t *ci_fget(int);
extern int ci_clone_fd(file_t *, file_t **);

struct ci_private_s;
extern struct ci_private_s *ci_fpriv(file_t *);
extern file_t *ci_privf(struct ci_private_s *);

#endif  /* __CI_DRIVER_PLATFORM_SUNOS_KERNEL_H__ */
/*! \cidoxg_end */
