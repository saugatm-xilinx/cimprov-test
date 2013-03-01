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

#ifndef __CI_TOOLS_GCC_X86_H__
#define __CI_TOOLS_GCC_X86_H__


/**********************************************************************
 * Free-running cycle counters.
 */

#define CI_HAVE_FRC64
#define CI_HAVE_FRC32

#define _IA64_REG_AR_KR0	3072
#define _IA64_REG_AR_ITC	3116

ci_inline void ci_frc64(ci_uint64* pval) {
  /* copied straight from linux-2.6.17 */
  __asm__ __volatile__ ("mov %0=ar%1" : "=r" (*pval)
			: "i"(_IA64_REG_AR_ITC - _IA64_REG_AR_KR0));
  ci_mb();
}
ci_inline void ci_frc32(ci_uint32* pval) {
  ci_uint64 ret;
  ci_frc64(&ret);
  *pval = (ci_uint32) ret;
}

#define ci_frc_flush() ci_mb()

/**********************************************************************
 * Compare and swap.
 */

#define CI_HAVE_COMPARE_AND_SWAP

#define __cas32(_type,_p,_oldval,_newval)				\
  ci_uint64 _ret;							\
  __asm__ __volatile__ ("mov ar.ccv=%0;;" :: "rO"(_oldval));		\
  __asm__ __volatile__ ("cmpxchg4.acq %0=[%1],%2,ar.ccv":		\
			"=r"(_ret) : "r"(_p), "r"(_newval) : "memory");	\
  return (_type) _ret

#define __cas64(_type,_p,_oldval,_newval)				\
  ci_uint64 _ret;							\
  __asm__ __volatile__ ("mov ar.ccv=%0;;" :: "rO"(_oldval));		\
  __asm__ __volatile__ ("cmpxchg8.acq %0=[%1],%2,ar.ccv":		\
			"=r"(_ret) : "r"(_p), "r"(_newval) : "memory");	\
  return (_type) _ret

ci_inline ci_int32 ci_cas32(volatile ci_int32* p, ci_int32 oldval, ci_int32 newval)
{ __cas32(ci_int32, p, oldval, newval); }

ci_inline ci_uint32 ci_cas32u(volatile ci_uint32* p, ci_uint32 oldval, ci_uint32 newval)
{ __cas32(ci_uint32, p, oldval, newval); }

ci_inline ci_int64 ci_cas64(volatile ci_int64* p, ci_int64 oldval, ci_int64 newval)
{  __cas64(ci_int64, p, oldval, newval); }

ci_inline ci_int64 ci_cas64u(volatile ci_uint64* p, ci_uint64 oldval, ci_uint64 newval)
{  __cas64(ci_uint64, p, oldval, newval); }

ci_inline int ci_cas32_succeed(volatile ci_int32* p, ci_int32 oldval,
			       ci_int32 newval)
{ return (int) (ci_cas32(p, oldval, newval) == oldval); }

ci_inline int ci_cas32_fail(volatile ci_int32* p, ci_int32 oldval,
			    ci_int32 newval)
{ return (int) (ci_cas32(p, oldval, newval) != oldval); }

ci_inline int ci_cas32u_succeed(volatile ci_uint32* p, ci_uint32 oldval,
			       ci_uint32 newval)
{ return (int) (ci_cas32u(p, oldval, newval) == oldval); }

ci_inline int ci_cas32u_fail(volatile ci_uint32* p, ci_uint32 oldval,
			    ci_uint32 newval)
{ return (int) (ci_cas32u(p, oldval, newval) != oldval); }

ci_inline int ci_cas64_succeed(volatile ci_int64* p, ci_int64 oldval,
			       ci_int64 newval)
{ return (int) (ci_cas64(p, oldval, newval) == oldval); }

ci_inline int ci_cas64_fail(volatile ci_int64* p, ci_int64 oldval,
			    ci_int64 newval)
{ return (int) (ci_cas64(p, oldval, newval) != oldval); }

ci_inline int ci_cas64u_succeed(volatile ci_uint64* p, ci_uint64 oldval,
			       ci_uint64 newval)
{ return (int) (ci_cas64u(p, oldval, newval) == oldval); }

ci_inline int ci_cas64u_fail(volatile ci_uint64* p, ci_uint64 oldval,
			    ci_uint64 newval)
{ return (int) (ci_cas64u(p, oldval, newval) != oldval); }

#define ci_cas_uintptr_succeed(p,o,n)				\
  ci_cas64u_succeed((volatile ci_uint64*) (p), (o), (n))

# define ci_cas_uintptr_fail(p,o,n)			\
  ci_cas64u_fail((volatile ci_uint64*) (p), (o), (n))

/**********************************************************************
 * Atomic integer.
 */

typedef struct { volatile ci_int32 n; } ci_atomic_t;

#define CI_ATOMIC_INITIALISER(i)  {(i)}

/*
** int  ci_atomic_read(a)         { return a->n;        }
** void ci_atomic_set(a, v)       { a->n = v;           }
** void ci_atomic_inc(a)          { ++a->n;             }
** void ci_atomic_dec(a)          { --a->n;             }
** int  ci_atomic_inc_and_test(a) { return ++a->n == 0; }
** int  ci_atomic_dec_and_test(a) { return --a->n == 0; }
** void ci_atomic_and(a, v)       { a->n &= v;          }
** void ci_atomic_or(a, v)        { a->n |= v;          }
*/


static inline ci_int32 ci_atomic_read(const ci_atomic_t* a) { return a->n; }
static inline void ci_atomic_set(ci_atomic_t* a, int v) { a->n = v; ci_mb(); }

static inline int ci_atomic_inc(ci_atomic_t* a)
{ 
  ci_int32 old, new;
  do {
    old = ci_atomic_read(a);
    new = old + 1;
  } while (ci_cas32(&a->n, old, new) != old);
  return new;
}

static inline int ci_atomic_dec(ci_atomic_t* a)
{
  ci_int32 old, new;
  do {
    old = ci_atomic_read(a);
    new = old - 1;
  } while (ci_cas32(&a->n, old, new) != old);
  return new;
}

static inline int ci_atomic_inc_and_test(ci_atomic_t* a)
{ return ci_atomic_inc(a) == 0; }

static inline int ci_atomic_dec_and_test(ci_atomic_t* a)
{ return ci_atomic_dec(a) == 0; }

ci_inline int
ci_atomic_xchg (ci_atomic_t *a, int v) {
  ci_uint64 ret;
  __asm__ __volatile__ ("xchg4 %0=[%1],%2" : "=r" (ret)
			: "r" (a->n), "r" (v) : "memory");
  return (int) ret;
}

static inline void ci_atomic32_or(volatile ci_int32* p, ci_uint32 mask)
{ 
  ci_int32 old, new;
  do {
    old = *p;
    new = old | mask;
  } while (ci_cas32(p, old, new) != old);
}
static inline void ci_atomic_or(ci_atomic_t* a, ci_uint32 mask)
{
  ci_atomic32_or(&a->n, mask);
}

static inline void ci_atomic32_and(volatile ci_int32* p, ci_uint32 mask)
{ 
  ci_int32 old, new;
  do {
    old = *p;
    new = old & mask;
  } while (ci_cas32(p, old, new) != old);
}
static inline void ci_atomic_and(ci_atomic_t* a, ci_uint32 mask)
{
  ci_atomic32_and(&a->n, mask);
}

static inline void ci_atomic_add(ci_atomic_t* a, ci_uint32 v)
{ 
  ci_int32 old, new;
  do {
    old = ci_atomic_read(a);
    new = old + v;
  } while (ci_cas32(&a->n, old, new) != old);
}


#define __cas_block_v(_p,_new)			\
  ci_int32 old, new;				\
  do {						\
    old = *_p;					\
    new = _new;					\
  } while (ci_cas32(_p, old, new) != old)

#define __cas_block_r(_p,_new)			\
  __cas_block_v(_p,_new);			\
  return new;

extern int ci_glibc_uses_nptl (void) CI_HF;
extern int ci_glibc_nptl_broken(void) CI_HF;
extern int ci_glibc_gs_get_is_multihreaded_offset (void) CI_HF;
extern int ci_glibc_gs_is_multihreaded_offset CI_HV;

static inline int ci_is_multithreaded (void) {
  return 1;
}

/**********************************************************************
 * Atomic bit field.
 */

typedef ci_uint32  ci_bits;
#define CI_BITS_N			32u

#define CI_BITS_DECLARE(name, n)			\
  ci_bits name[((n) + CI_BITS_N - 1u) / CI_BITS_N]

ci_inline void ci_bits_clear_all(volatile ci_bits* b, int n_bits)
{ memset((void*) b, 0, (n_bits+CI_BITS_N-1u) / CI_BITS_N * sizeof(ci_bits)); }

ci_inline void ci_bit_set(volatile ci_bits* b, int i)
{
  ci_int32 mask, old, new;
  mask = 1 << ( i & 31 );
  do {
    old = b[i>>5];
    new = old | mask;
  } while (ci_cas32(b, old, new) != old);
}

ci_inline void ci_bit_clear(volatile ci_bits* b, int i)
{
  ci_int32 mask, old, new;
  mask = ~(1 << ( i & 31 ));
  do {
    old = b[i>>5];
    new = old & mask;
  } while (ci_cas32(b, old, new) != old);
}

ci_inline int ci_bit_test(volatile ci_bits* b, int i)
{ return b[i >> 5] & (1<<(i & 31)); }

ci_inline int ci_bit_test_and_set(volatile ci_bits *b, int i)
{
  ci_int32 mask, old, new;
  mask = 1 << ( i & 31 );
  do {
    old = b[i>>5];
    new = old | mask;
  } while (ci_cas32(b, old, new) != old);
  return (old & mask) != 0;
}

ci_inline int ci_bit_test_and_clear(volatile ci_bits *b, int i)
{
  ci_int32 mask, old, new;
  mask = ~(1 << ( i & 31 ));
  do {
    old = b[i>>5];
    new = old & mask;
  } while (ci_cas32(b, old, new) != old);
  return (old & ~mask) != 0;
}

#define ci_bit_mask_set(b,m)	ci_atomic32_or((b), (m))
#define ci_bit_mask_clear(b,m)	ci_atomic32_and((b), ~(m))


/**********************************************************************
 * Misc.
 */

# define ci_spinloop_pause()

#define CI_HAVE_ADDC32
#define ci_add_carry32(sum, v)				\
  do {							\
    ci_uint64 temp;					\
    temp=(((ci_uint64)(sum))+((ci_uint32) (v)));	\
    sum=(unsigned int)(temp+(temp>>32));		\
  } while(0)


/* TODO */
ci_inline ci_uint16 ci_bswap16(ci_uint16 v)
{ return CI_BSWAPC_16(v); }

/* TODO */
ci_inline ci_uint32 ci_bswap32(ci_uint32 v)
{ return CI_BSWAPC_32(v); }


/* TODO */
#define ci_prefetch(addr)  do{}while(0)


#endif  /* __CI_TOOLS_GCC_IA64_H__ */
/*! \cidoxg_end */
