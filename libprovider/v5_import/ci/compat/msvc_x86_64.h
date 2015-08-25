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

#ifndef __CI_COMPAT_MSVC_X86_64_H__
#define __CI_COMPAT_MSVC_X86_64_H__

#include <intrin.h>

/*! 
   Barriers to enforce ordering with respect to:

   normal memory use: ci_wmb, ci_rmb, ci_mb
   IO bus access use: ci_wiob, ci_riob, ci_iob
*/


ci_inline void ci_x86_mb(void)
{  
  volatile long foo;
  /* This will generate an instruction with the lock prefix, which will
   * prevent the processor reordering past it in either direction. 
   * According to MSDN it is also a compiler barrier. (The value of the 
   * addend has no significance, but the x86 version uses 10h and 
   * symmetry is nice...) */
  InterlockedAdd(&foo, 0x10);
}


#define ci_compiler_barrier()  _WriteBarrier()

#define ci_wmb _WriteBarrier

/* TODO: we can probably do cleverer things with SSE2, but this gets things
 * working for now. */
# define ci_rmb()       ci_x86_mb()
# define ci_mb()        ci_x86_mb()
# define ci_wiob()      ci_x86_mb()
# define ci_riob()      ci_x86_mb()
# define ci_iob()       ci_x86_mb()

/*--------------------------------------------------------------------
 *
 * Windows uses a struct PHYSICAL_ADDRESS, which can't be directly manipulated.
 *
 * We use LONGLONG which has compiler support, but this assumes that compiler
 * support is available for arithmetic operations on this type and that casts
 * will work
 *
 *--------------------------------------------------------------------*/
typedef LONGLONG    ci_phys_addr_t;

#define ci_phys_addr_fmt "%I64x"

#define ci_phys_addr_cast_in(x)	        ((x).QuadPart)
#define ci_phys_addr_cast_out(x)        *((PHYSICAL_ADDRESS *) &(x))
#define ci_dma_addr_cast_in(x)		((x).QuadPart)
#define ci_dma_addr_cast_out(x)	        *((PHYSICAL_ADDRESS *) &(x))

#endif  /* __CI_COMPAT_MSVC_X86_H__ */

/*! \cidoxg_end */
