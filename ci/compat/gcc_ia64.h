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

/*! \cidoxg_include_ci_compat  */

#ifndef __CI_COMPAT_GCC_IA64_H__
#define __CI_COMPAT_GCC_IA64_H__

/* 
   Barriers to enforce ordering with respect to:

   normal memory use: ci_wmb, ci_rmb, ci_wmb
   IO bus access use: ci_wiob, ci_riob, ci_iob
*/
#define ci_mb()    __asm__ __volatile__("mf": : :"memory")
#define ci_rmb()   ci_mb()
#define ci_wmb()   ci_mb()

#define ci_iob()   __asm__ __volatile__("mf.a": : :"memory")
#define ci_riob()   ci_iob()
#define ci_wiob()   ci_iob()

/* Really these should be in  src/include/ci/driver/platform/... */
typedef unsigned long     	ci_phys_addr_t;
#define ci_phys_addr_fmt  "%lx"

#endif  /* __CI_COMPAT_GCC_IA64_H__ */

/*! \cidoxg_end */
