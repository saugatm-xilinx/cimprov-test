/**************************************************************************\
*//*! \file timer.h
** <L5_PRIVATE L5_HEADER >
** \author  aam
**  \brief  ISCSI system timer wrapper
**   \date  2006/02/16
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_iscsi */

#ifndef __CI_ISCSI_TIMER_H__
#define __CI_ISCSI_TIMER_H__

/* ci_timer structures must be initialised by ci_timer_init before a
 * timeout is set on them. They can only handle one simultaneous timeout.
 * ci_timer_init may be used again to change the timers info, but this
 * must not be done while a timeout is set.
 */

/* Callback function type */
typedef void(*ci_timer_fn)(void *data);


#ifdef __linux__

#include <linux/timer.h>

/* Timer object */
typedef struct timer_list ci_timer;

#elif defined(__ci_wdm__) || defined (__ci_wdf__)

/* Timer object */
typedef struct {
  KTIMER timer;
  KDPC dpc;
  ci_timer_fn fn;
  void *data;
  int pending;
} ci_timer;

#elif defined(__ci_storport__)

 typedef ULONG ci_timer;

#else
#  error Unknown platform
#endif


/* Initialise timer object */
extern void ci_timer_init(ci_timer *timer, ci_timer_fn fn, void *data);

/* Set a callback */
extern int ci_timer_set(ci_timer *timer, ci_uint32 time_msec);

/* Cancel a callback.
 * Does not guarantee that the timeout is not already firing in
 * another thread / on another CPU.
 */
extern int ci_timer_cancel(ci_timer *timer);

/* Is a callback currently set.
 * Returns 1 if callback is set, 0 otherwise.
 */
extern int ci_timer_is_set(ci_timer *timer);

#endif

/*! \cidoxg_end */

