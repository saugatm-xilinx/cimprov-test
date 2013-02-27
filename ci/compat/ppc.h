/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_OPEN>
** \author  
**  \brief  
**   \date  
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_compat */

#ifndef __CI_COMPAT_PPC_H__
#define __CI_COMPAT_PPC_H__


#define CI_MY_BYTE_ORDER   CI_BIG_ENDIAN

 
#define CI_WORD_SIZE     4
#define CI_PTR_SIZE      8

#ifdef __KERNEL__

#include <asm/page.h>

#define CI_PAGE_SHIFT    PAGE_SHIFT
#define CI_PAGE_SIZE     PAGE_SIZE
#define CI_PAGE_MASK     PAGE_MASK

#else /* !__KERNEL__ */

#include <unistd.h>

/* The page size is not known at compile time.  Try to cache it. */
static int ci_page_size(void) __attribute__((pure, unused));
static int ci_page_size(void)
{
  static int page_size;
  int ret = *(volatile int *)&page_size;
  if( ret == 0 ) {
    ret = sysconf(_SC_PAGESIZE);
    *(volatile int *)&page_size = ret;
  }
  return ret;
}
static int ci_page_shift(void) __attribute__((pure, unused));
static int ci_page_shift(void)
{
  static int page_shift = -1;
  int ret = *(volatile int *)&page_shift;
  if( ret < 0 ) {
    ret = __builtin_ffs(ci_page_size()) - 1;
    *(volatile int *)&page_shift = ret;
  }
  return ret;
}
#define CI_PAGE_SHIFT	ci_page_shift()
#define CI_PAGE_SIZE	ci_page_size()
#define CI_PAGE_MASK	(ci_page_size() - 1)

#endif

#endif  /* __CI_COMPAT_PPC_H__ */

/*! \cidoxg_end */
