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


/**********************************************************************
 * Free-running cycle counters.
 */

#define CI_HAVE_FRC32

ci_inline void ci_frc32(ci_uint32* pval)
{
  *pval=(ci_uint32)__rdtsc();
}

#ifdef CI_HAVE_INT64

#define CI_HAVE_FRC64

ci_inline void ci_frc64(ci_uint64* pval)
{
  *pval=__rdtsc();
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

  typedef struct { volatile long n; } ci_atomic_t;

# define CI_ATOMIC_INITIALISER(i)  {(i)}

  ci_inline void ci_atomic_set(ci_atomic_t* v, int i) { v->n = i; ci_wmb();  }
  ci_inline int  ci_atomic_read(const ci_atomic_t* v) { return v->n; }

  ci_inline void ci_atomic_inc(ci_atomic_t* v)
  { InterlockedIncrement(&v->n); }

  ci_inline void ci_atomic_dec(ci_atomic_t* v)
  { InterlockedDecrement(&v->n); }

  ci_inline int ci_atomic_inc_and_test(ci_atomic_t* v)
  { return InterlockedIncrement(&v->n) == 0; }

  ci_inline int ci_atomic_dec_and_test(ci_atomic_t* v)
  { return InterlockedDecrement(&v->n) == 0; }

  ci_inline void ci_atomic_and(ci_atomic_t* v, int i)
  { InterlockedAnd(&v->n, i); }

  ci_inline void ci_atomic_or(ci_atomic_t* v, int i)
  { InterlockedOr(&v->n, i); }

  ci_inline int ci_atomic_xchg (ci_atomic_t *v, int i)
  { return InterlockedExchange(&(v->n), i); }

  ci_inline void ci_atomic_add(ci_atomic_t* v, int i)
  { InterlockedAdd(&v->n, i); }

  ci_inline void ci_atomic32_and(volatile ci_uint32* p, ci_uint32 mask)
  { InterlockedAnd((volatile LONG*)p, mask); }

  ci_inline void ci_atomic32_or(volatile ci_uint32* p, ci_uint32 mask)
  { InterlockedOr((volatile LONG*)p, mask); }

  ci_inline void ci_atomic32_add(volatile ci_uint32* p, ci_uint32 mask)
  { InterlockedAdd((volatile LONG*)p, mask); }

  ci_inline void ci_atomic32_inc(volatile ci_uint32* p)
  { InterlockedIncrement((volatile LONG*) p); }

  ci_inline int ci_atomic32_dec_and_test(volatile ci_uint32* p)
  { return InterlockedDecrement((volatile LONG*) p) == 0; }


/**********************************************************************
 * atomic compare and swap.
 */

/* Use intrinsic functions in all cases (WDM, NDIS and user-level) */

#define CI_HAVE_COMPARE_AND_SWAP

ci_inline int
ci_cas32_succeed(volatile ci_int32* p, ci_int32 oldval, ci_int32 newval)
{
  return (_InterlockedCompareExchange((volatile LONG*)p, newval, oldval)
          == oldval);
}

ci_inline int
ci_cas32_fail(volatile ci_int32* p, ci_int32 oldval, ci_int32 newval)
{
  return (_InterlockedCompareExchange((volatile LONG*)p, newval, oldval)
          != oldval);
}

ci_inline int
ci_cas32u_succeed(volatile ci_uint32* p, ci_uint32 oldval, ci_uint32 newval)
{
  return (_InterlockedCompareExchange((volatile LONG*)p, (LONG)newval,
                                      (LONG)oldval) == (LONG)oldval);
}

ci_inline int
ci_cas32u_fail(volatile ci_uint32* p, ci_uint32 oldval, ci_uint32 newval)
{
  return (_InterlockedCompareExchange((volatile LONG*)p, (LONG)newval,
                                      (LONG)oldval) != (LONG)oldval);
}

ci_inline int
ci_cas_uintptr_succeed(volatile ci_uintptr_t* p, ci_uintptr_t oldval,
                       ci_uintptr_t newval)
{
  return (_InterlockedCompareExchangePointer((volatile PVOID*)p, (PVOID)newval,
                                             (PVOID)oldval) == (PVOID)oldval);
}

ci_inline int
ci_cas_uintptr_fail(volatile ci_uintptr_t* p, ci_uintptr_t oldval,
                    ci_uintptr_t newval)
{
  return (_InterlockedCompareExchangePointer((volatile PVOID*)p, (PVOID)newval,
                                             (PVOID)oldval) != (PVOID)oldval);
}

ci_inline int
ci_cas64_succeed(volatile ci_int64* p, ci_int64 oldval, ci_int64 newval)
{
    return (_InterlockedCompareExchange64((volatile __int64*)p, newval, oldval)
            == oldval);
}

ci_inline int 
ci_cas64_fail(volatile ci_int64* p, ci_int64 oldval, ci_int64 newval)
{
    return (_InterlockedCompareExchange64((volatile __int64*)p, newval, oldval)
            != oldval);
}

#ifndef __KERNEL__
/**********************************************************************
 * Counting semaphores.
 */
typedef HANDLE ci_count_sema_t;

#ifdef __ci_storport__
  extern ci_count_sema_t CreateSemaphore(PVOID,int,int,PVOID);
  extern void CloseHandle ( HANDLE );
  extern int ReleaseSemaphore ( ci_count_sema_t, int, PVOID );
#endif

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

typedef long ci_bits;
#define CI_BITS_N			32u

#define CI_BITS_DECLARE(name, n)			\
  ci_bits name[((n) + CI_BITS_N - 1u) / CI_BITS_N]

ci_inline void ci_bits_clear_all(volatile ci_bits* b, int n_bits)
{ memset((void*) b, 0, (n_bits+CI_BITS_N-1u) / CI_BITS_N * sizeof(ci_bits)); }

ci_inline void ci_bit_set(volatile ci_bits* b, int i) {
  _InterlockedOr(&b[i / CI_BITS_N], 1u << (i % CI_BITS_N));
}

ci_inline void ci_bit_clear(volatile ci_bits* b, int i) {
  _InterlockedAnd(&b[i / CI_BITS_N], ~(1u << (i % CI_BITS_N)));
}

ci_inline int  ci_bit_test(volatile ci_bits* b, int i) {
  return b[i / CI_BITS_N] & (1u << (i % CI_BITS_N)) ? 1 : 0;
}

ci_inline int ci_bit_test_and_set(volatile ci_bits* b, int i) {
  return _interlockedbittestandset(&b[i / CI_BITS_N], i % CI_BITS_N);
}

ci_inline int ci_bit_test_and_clear(volatile ci_bits* b, int i) {
  return _interlockedbittestandreset(&b[i / CI_BITS_N], i % CI_BITS_N);
}

/* These mask ops only work within a single ci_bits word. */
#define ci_bit_mask_set(b,m)	ci_atomic32_or((b), (m))
#define ci_bit_mask_clear(b,m)	ci_atomic32_and((b), ~(m))


/**********************************************************************
 * Misc.
 */

ci_inline void ci_spinloop_pause()
{
  _mm_pause();
}

#define CI_HAVE_ADDC32
ci_inline void _ci_add_carry32(unsigned int *sum, ci_uint32 v)
{ 
  ci_uint64 temp;
  temp=(((ci_uint64)*sum)+v);
  *sum=(unsigned int)(temp+(temp>>32));
}

#define ci_add_carry32(sum, v) { _ci_add_carry32(&(sum), (v)); }


ci_inline ci_uint16 ci_bswap16(ci_uint16 v)
{ return _byteswap_ushort(v); }

ci_inline ci_uint32 ci_bswap32(ci_uint32 v)
{ return _byteswap_ulong(v); }


#define ci_prefetch(addr)  _mm_prefetch((char*) (addr), _MM_HINT_T0)


#endif  /* __CI_TOOLS_MSVC_X86_H__ */

/*! \cidoxg_end */
