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

#ifndef __CI_DRIVER_PLATFORM_WIN32_UL_H__
#define __CI_DRIVER_PLATFORM_WIN32_UL_H__


/*--------------------------------------------------------------------
 *
 *
 *--------------------------------------------------------------------*/

typedef void *    PFILE_OBJECT;

/*--------------------------------------------------------------------
 *
 * ci_waitable_t
 *
 *--------------------------------------------------------------------*/

#define CI_BLOCKING_CTX_ARG(x)

typedef int ci_waitable_t;
typedef int ci_waitable_timeout_t;
typedef int ci_waiter_t;

/*--------------------------------------------------------------------
 *
 * wait_queue 
 *
 *--------------------------------------------------------------------*/

/* we could use a windows implementations of pthreads - instead of our hacked
   up cithread condition variable 
*/

/*! Comment? */
typedef struct {
  cithread_cond_t    cv;
  CRITICAL_SECTION   mutex;
} ci_waitq_t;




typedef int ci_waitq_timeout_t;
typedef int ci_waitq_waiter_t;

/*! Comment? */
ci_inline void ci_waitq_ctor(ci_waitq_t* wq) {
  InitializeCriticalSection(&wq->mutex);
  cithread_cond_ctor(&wq->cv, &wq->mutex);
}

/*! Comment? */
ci_inline void ci_waitq_dtor(ci_waitq_t* wq) {
  cithread_cond_dtor(&wq->cv);  
  DeleteCriticalSection(&wq->mutex);
}

/*! Comment? */
ci_inline void ci_waitq_wakeup(ci_waitq_t* wq) {
  EnterCriticalSection(&wq->mutex);
  LeaveCriticalSection(&wq->mutex);
  cithread_cond_broadcast(&wq->cv); 
}

#define ci_waitq_init_timeout(t, timeval)			\
  do {								\
    if( ci_waitq_wait_forever(timeval) )			\
      *(t) = -1;						\
    else							\
      *(t) = (timeval)->tv_sec * 1000000 + (timeval)->tv_usec;	\
  } while(0)

#define ci_waitq_waiter_pre(waiter, wq)			\
  EnterCriticalSection(&(wq)->mutex)

#define ci_waitq_waiter_exclusive_pre  ci_waitq_waiter_pre

#define ci_waitq_waiter_wait(waiter, wq, cond)	\
  do {						\
    int rc;					\
    if( !(cond) ) {				\
      rc = cithread_cond_wait(&(wq)->cv, 0);	\
      ci_assert(rc == 0);			\
    }						\
  } while(0)

#define ci_waitq_waiter_timedwait(waiter, wq, cond, tmo)		   \
  do {									   \
    if( !(cond) ) {							   \
      do {								   \
        int rc;								   \
        if( *(tmo) >= 0 )  rc = cithread_cond_timedwait(&(wq)->cv, (tmo)); \
        else               rc = cithread_cond_wait(&(wq)->cv, 0);	   \
        if( rc == 0 )  break;						   \
        /*! \TODO ?? timeout ought to be time remaining in usec */	   \
        if( rc == ETIMEDOUT ) { *(tmo) = 0; break; }			   \
        /* Otherwise we got EINTR.  Keep waiting. */			   \
      } while( !(cond) );						   \
    }									   \
  } while(0)

#define ci_waitq_waiter_again(waiter, wq)

#define ci_waitq_waiter_post(waiter, wq)		\
  LeaveCriticalSection(&(wq)->mutex);

#define ci_waitq_waiter_signalled(waiter, wq)  0

#define ci_waitq_waiter_timedout(timeout)      (*(timeout) == 0)


#endif  /* __CI_DRIVER_PLATFORM_WIN32_UL_H__ */

/*! \cidoxg_end */
