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

#ifndef __CI_TOOLS_MSVC_X86_H__
#define __CI_TOOLS_MSVC_X86_H__

#ifdef __cplusplus
extern "C" {
#endif


/**********************************************************************
 * Free-running cycle counters.
 */

#define CI_HAVE_FRC32

ci_inline void ci_frc32(ci_uint32* pval)
{
  unsigned _h, _l;

  __asm
  {
    rdtsc
    mov _h, EDX
    mov _l, EAX
  }

  *pval = _l;
}


#ifdef CI_HAVE_INT64

#define CI_HAVE_FRC64

ci_inline void ci_frc64(ci_uint64* pval)
{
  unsigned _h, _l;

  __asm
  {
    rdtsc
    mov _h, EDX
    mov _l, EAX
  }

  *pval = ( ((ci_uint64) _h << 32) | ((ci_uint64) _l));
}

#endif

#define ci_frc_flush()  /* ?? need a pipeline barrier */


/**********************************************************************
 * Atomic integer
 */

/*
** int  ci_atomic_read(a)         { return a->n;        }
** void ci_atomic_set(a, v)       { a->n = v;           }
** void ci_atomic_inc(a)          { ++a->n;             }
** void ci_atomic_dec(a)          { --a->n;             }
** int  ci_atomic_inc_and_test(a) { return ++a->n == 0; }
** int  ci_atomic_dec_and_test(a) { return --a->n == 0; }
** void ci_atomic_and(a, v)       { a->n &= v;          }
** void ci_atomic_or(a, v)        { a->n |= v;          }
** void ci_atomic_xchg(a, v)      { t = a->n; a->n = v; return t; }
*/

/* Use intrinsic functions in all cases (WDM, NDIS and user-level) */

  typedef struct { long volatile n; } ci_atomic_t;

# define CI_ATOMIC_INITIALISER(i)  {(i)}

  ci_inline void ci_atomic_set(ci_atomic_t* v, int i) 
  { v->n = (LONG)i; ci_wmb();  }
  ci_inline int  ci_atomic_read(const ci_atomic_t* v)       
  { return (int)v->n; }

  ci_inline void ci_atomic_inc(ci_atomic_t* v)
  { _InterlockedIncrement(&(v->n)); }

  ci_inline void ci_atomic_dec(ci_atomic_t* v)
  { _InterlockedDecrement(&(v->n)); }

  ci_inline int ci_atomic_inc_and_test(ci_atomic_t* v)
  { return _InterlockedIncrement(&(v->n)) == 0; }

  ci_inline int ci_atomic_dec_and_test(ci_atomic_t* v)
  { return _InterlockedDecrement(&(v->n)) == 0; }

  ci_inline int ci_atomic_xchg (ci_atomic_t *v, int i)
  { return _InterlockedExchange(&(v->n), i); }

  ci_inline void ci_atomic32_and(volatile ci_uint32* p, ci_uint32 mask)
  {
    __asm
    {
      mov EDX, mask
      mov EDI, p
      lock and [EDI], EDX
    }
  }

  ci_inline void ci_atomic32_or(volatile ci_uint32* p, ci_uint32 mask)
  {
    __asm
    {
      mov EDX, mask
      mov EDI, p
      lock or [EDI], EDX
    }
  }

  ci_inline void ci_atomic32_add(volatile ci_uint32* p, ci_uint32 v)
  {
    __asm
    {
      mov EDX, v
      mov EDI, p
      lock add [EDI], EDX
    }
  }

  ci_inline void ci_atomic32_inc(volatile ci_uint32* p)
  { _InterlockedIncrement((volatile long*) p); }

  ci_inline int ci_atomic32_dec_and_test(volatile ci_uint32* p)
  { return _InterlockedDecrement((volatile long*) p) == 0; }

#define ci_atomic_or(a, v)   ci_atomic32_or ((ci_uint32*) &(a)->n, (v))
#define ci_atomic_and(a, v)  ci_atomic32_and((ci_uint32*) &(a)->n, (v))
#define ci_atomic_add(a, v)  ci_atomic32_add((ci_uint32*) &(a)->n, (v))


/**********************************************************************
 * atomic compare and swap.
 */

/* Use our magic inline assembler for all cases (WDM, NDIS and user-level) */

#define CI_HAVE_COMPARE_AND_SWAP

/*!< compare integer at address with an assumed old value - if it is identical
 *   atomically swap its value with a new value
 *
 *    \returns non-zero if the swap took place
 */
ci_inline int
ci_cas32_succeed(volatile ci_int32* p, long oldval, long newval)
{
  char rc = 0;

  __asm
  {
    mov EDX, newval
    mov EAX, oldval
    mov EDI, p
    lock cmpxchg [EDI], EDX
    sete rc
  }

  return rc;
}

/*!< compare integer at address with an assumed old value - if it is identical
 *   atomically swap its value with a new value
 *
 *    \returns non-zero if the swap did not take place
 */
ci_inline int
ci_cas32_fail(volatile ci_int32* p, long oldval, long newval)
{
  char rc = 1;

  __asm
  {
    mov EDX, newval
    mov EAX, oldval
    mov EDI, p
    lock cmpxchg [EDI], EDX
    setne rc
  }

  return rc;
}

/*!< compare unsigned integer at address with an assumed old value - if it is
 *   identical atomically swap its value with a new value
 *
 *    \returns non-zero if the swap took place
 */
ci_inline int
ci_cas32u_succeed(volatile ci_uint32* p, ci_uint32 oldval, ci_uint32 newval)
{
  char rc = 0;

  __asm
  {
    mov EDX, newval
    mov EAX, oldval
    mov EDI, p
    lock cmpxchg [EDI], EDX
    sete rc
  }

  return rc;
}

/*!< compare unsigned integer at address with an assumed old value - if it is
 *   identical atomically swap its value with a new value
 *
 *    \returns non-zero if the swap did not take place
 */
ci_inline int
ci_cas32u_fail(volatile ci_uint32* p, ci_uint32 oldval, ci_uint32 newval)
{
  char rc = 1;

  __asm
  {
    mov EDX, newval
    mov EAX, oldval
    mov EDI, p
    lock cmpxchg [EDI], EDX
    setne rc
  }

  return rc;
}

/*!< compare ci_uintptr_t at address with an assumed old value - if it is
 *   identical atomically swap its value with a new value
 *
 *    \returns non-zero if the swap took place
 */
ci_inline int
ci_cas_uintptr_succeed(volatile ci_uintptr_t *p, ci_uintptr_t oldval,
                       ci_uintptr_t newval)
{
  return ci_cas32u_succeed((volatile ci_uint32 *)p, (ci_uint32)oldval,
                           (ci_uint32)newval);
}

/*!< compare ci_uintptr_t at address with an assumed old value - if it is
 *   identical atomically swap its value with a new value
 *
 *    \returns non-zero if the swap did not take place
 */
ci_inline int
ci_cas_uintptr_fail(volatile ci_uintptr_t *p, ci_uintptr_t oldval,
                    ci_uintptr_t newval)
{
  return ci_cas32u_fail((volatile ci_uint32 *)p, (ci_uint32)oldval,
                        (ci_uint32)newval);
}

#if !defined(__KERNEL__) && !defined(__ci_storport__)

/**********************************************************************
 * Counting semaphores.
 */
typedef HANDLE ci_count_sema_t;

ci_inline int ci_count_sema_ctor(ci_count_sema_t *count_sema, int max_count)
{
  *count_sema = CreateSemaphore(NULL, 0, max_count, NULL);
  return 0;
}

ci_inline int ci_count_sema_dtor(ci_count_sema_t *count_sema)
{
  CloseHandle(*count_sema);
  return 0;
}

ci_inline int ci_count_sema_add(ci_count_sema_t *count_sema, int count)
{
  if (!ReleaseSemaphore(*count_sema, count, NULL))
  {
    return -1;
  }
  return 0;
}

ci_inline int ci_count_sema_inc(ci_count_sema_t *count_sema)
{
  return ci_count_sema_add(count_sema, 1);
}

#endif

/**********************************************************************
 * Atomic bit field.
 */

typedef unsigned  ci_bits;
#define CI_BITS_N			32u

#define CI_BITS_DECLARE(name, n)			\
  ci_bits name[((n) + CI_BITS_N - 1u) / CI_BITS_N]

ci_inline void ci_bits_clear_all(volatile ci_bits* b, int n_bits)
{ memset((void*) b, 0, (n_bits+CI_BITS_N-1u) / CI_BITS_N * sizeof(ci_bits)); }

ci_inline void ci_bit_set(volatile ci_bits* b, int i) {
  __asm
  {
    mov EDX, i
    mov EDI, b
    lock bts [EDI], EDX
  }
}

ci_inline void ci_bit_clear(volatile ci_bits* b, int i) {
  __asm
  {
    mov EDX, i
    mov EDI, b
    lock btr [EDI], EDX
  }
}

ci_inline int  ci_bit_test(volatile ci_bits* b, int i) {
  char rc;

  __asm
  {
    mov EDX, i
    mov EDI, b
    bt [EDI], EDX
    setc rc
  }

  return rc;
}

ci_inline int ci_bit_test_and_set(volatile ci_bits* b, int i) {
  char rc;

  __asm
  {
    mov EDX, i
    mov EDI, b
    lock bts [EDI], EDX
    setc rc
  }

  return rc;
}

ci_inline int ci_bit_test_and_clear(volatile ci_bits* b, int i) {
  char rc;

  __asm
  {
    mov EDX, i
    mov EDI, b
    lock btr [EDI], EDX
    setc rc
  }

  return rc;
}

ci_inline void __ci_bits_or(volatile ci_bits* b, unsigned mask) {
  __asm
  {
    mov EDX, mask
    mov EDI, b
    lock or [EDI], EDX
  }
}

ci_inline void __ci_bits_and(volatile ci_bits* b, unsigned mask) {
  __asm
  {
    mov EDX, mask
    mov EDI, b
    lock and [EDI], EDX
  }
}

#define ci_bit_mask_set(b,m)		__ci_bits_or((b), (m))
#define ci_bit_mask_clear(b,m)		__ci_bits_and((b), ~(m))


/**********************************************************************
 * Misc.
 */

ci_inline void ci_spinloop_pause()
{
  __asm
  {
    pause
  }
}

#define CI_HAVE_ADDC32
ci_inline void _ci_add_carry32(unsigned int *sum, ci_uint32 v)
{
  __asm
  {
    mov  ESI,  sum
    mov  EAX,  v
    add  EAX, [ESI]
    adc  EAX,  0
    mov [ESI], EAX
  }
}

#define ci_add_carry32(sum, v) { _ci_add_carry32(&(sum), (v)); }

/**********************************************************************
 * Byte swap.
 */

ci_inline ci_uint16 ci_bswap16(ci_uint16 v)
{ return _byteswap_ushort(v); }

ci_inline ci_uint32 ci_bswap32(ci_uint32 v)
{ return _byteswap_ulong(v); }


#define ci_prefetch(addr)                       \
  do {                                          \
    char* __p = (char*) (addr);                 \
    __asm { prefetcht0 __p };                   \
  } while(0)


#ifdef __cplusplus
}
#endif

#endif  /* __CI_TOOLS_MSVC_X86_H__ */

/*! \cidoxg_end */
