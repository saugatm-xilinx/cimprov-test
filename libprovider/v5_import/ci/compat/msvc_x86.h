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

#ifndef __CI_COMPAT_MSVC_X86_H__
#define __CI_COMPAT_MSVC_X86_H__

/*! 
   Barriers to enforce ordering with respect to:

   normal memory use: ci_wmb, ci_rmb, ci_wmb
   IO bus access use: ci_wiob, ci_riob, ci_iob
*/

ci_inline void
ci_x86_mb() 	
{
  /* NB KeMemoryBarrier is available in latest DDK */

#if 0

/*! 
   I'm worried about code which uses segments breaking this implementation of
   mb().  
*/
  __asm				
  {				
   lock add dword ptr [ESP], 0h		
  }				
#else
   volatile unsigned bar, *foo = &bar;

  __asm				
 {				
   mov  esi, foo
   lock add  dword ptr [esi], 10h		
 }				

#endif
}


/* ?? FIXME: Can we not achieve this without emitting a nop? */
#define ci_compiler_barrier()  __asm { nop }


ci_inline void
ci_x86_sfence() 
{
  /*! ?? \TODO get MASM version from compile - think that 6.15 has sfence */
#define masm_sfence __asm _emit 0x0F __asm _emit 0xAE __asm _emit 0xF8 
  __asm				
  {				
    masm_sfence
  }				
}

ci_inline void
ci_x86_lfence() 
{
  /*! ?? \TODO get MASM version from compile - think that 6.15 has lfence */
#define masm_lfence __asm _emit 0x0F __asm _emit 0xAE __asm _emit 0xE8 
  __asm				
  {				
    masm_lfence
  }				
}

/*! Comment? */
ci_inline void
ci_x86_mfence() 
{
  /*! ?? \TODO get MASM version from compile - think that 6.15 has mfence */
#define masm_mfence __asm _emit 0x0F __asm _emit 0xAE __asm _emit 0xF0 
  __asm				
  {				
    masm_mfence
  }				
}

/* x86 processors to P4 Xeon store in-order unless executing streaming
   extensions or when using writecombining.

   Hence we do not define ci_wmb to use sfence by default. Requirement is that
   we do not use writecombining to memory and any code which uses SSE
   extensions must call sfence directly 

   We need to track non intel clones which may support out of order store.

 */

#if CI_CPU_OOS
# if CI_CPU_HAS_SSE
#  define ci_wmb()	ci_x86_sfence()
# else
#  define ci_wmb()	ci_x86_mb()
# endif
#else
# define ci_wmb()       ci_compiler_barrier()
#endif

#if CI_CPU_HAS_SSE2
# define ci_rmb()	ci_x86_lfence()
# define ci_mb()	ci_x86_mfence()
# if CI_CPU_HAS_SSE
#  define ci_wiob()	ci_x86_sfence()
# else
#  define ci_wiob()	ci_x86_mb()
# endif
# define ci_riob()	ci_x86_lfence()
# define ci_iob()	ci_x86_mfence()
#else
# define ci_rmb()	ci_x86_mb()
# define ci_mb()   	ci_x86_mb()
# define ci_wiob()  	ci_x86_mb()
# define ci_riob()  	ci_x86_mb()
# define ci_iob()  	ci_x86_mb()
#endif


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
