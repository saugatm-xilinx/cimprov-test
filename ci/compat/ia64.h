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

#ifndef __CI_COMPAT_IA64_H__
#define __CI_COMPAT_IA64_H__


#define CI_MY_BYTE_ORDER   CI_LITTLE_ENDIAN

#define CI_WORD_SIZE       8
#define CI_PTR_SIZE        8

#ifndef __KERNEL__

/* At user level we might be compiling against the wrong set of kernel
   headers, so define these to be appropriate.  You'll have to edit
   this when you change kernel page size to make sure they're
   consistent.  This just isn't going to work as not everywhere uses
   CI_PAGE_* */

#define CI_PAGE_SHIFT      14 /*PAGE_SHIFT*/
#define CI_PAGE_SIZE       (1 << CI_PAGE_SHIFT)
#define CI_PAGE_MASK       (~(CI_PAGE_SIZE - 1))

#else

/* In the kernel these should get picked up from the kernel headers
   appropriately */
#define CI_PAGE_SHIFT      PAGE_SHIFT
#define CI_PAGE_SIZE       PAGE_SIZE
#define CI_PAGE_MASK       PAGE_MASK

#endif

#endif  /* __CI_COMPAT_IA64_H__ */

/*! \cidoxg_end */
