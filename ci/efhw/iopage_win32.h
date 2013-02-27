
#ifndef __CI_EFHW_IOPAGE_WIN32_H__
#define __CI_EFHW_IOPAGE_WIN32_H__

#include <ci/efhw/common_ci2linux.h>


/*--------------------------------------------------------------------
 *
 * struct efhw_page: A single page of memory.  Directly mapped in the
 * driver, and can be mapped to userlevel.
 *
 *--------------------------------------------------------------------*/

#define __va(phys)  (void*)(phys)
#define __pa(virt)  (ci_phys_addr_t)(virt)

struct efhw_page {
  void * kva;
};

ci_inline char* efhw_page_ptr(struct efhw_page* p)
{ return p->kva; }

ci_inline unsigned efhw_page_pfn(struct efhw_page* p)
{ return (unsigned) (__pa(p->kva) >> CI_PAGE_SHIFT); }

ci_inline void efhw_page_mark_invalid(struct efhw_page* p)
{ p->kva = 0; }

ci_inline int efhw_page_is_valid(struct efhw_page* p)
{ return p->kva != NULL; }

ci_inline void efhw_page_init_from_va(struct efhw_page* p, void* va)
{ p->kva = va; }

/*--------------------------------------------------------------------
 *
 * struct efhw_iopage: A single page of memory.  Directly mapped in the
 * driver, and can be mapped to userlevel.  Can also be accessed by the
 * NIC.
 *
 *--------------------------------------------------------------------*/

struct efhw_iopage {
  struct efhw_page  p;
  PHYSICAL_ADDRESS  dma_addr;
  void *            wdf_cbuffer;
};


ci_inline dma_addr_t efhw_iopage_dma_addr(struct efhw_iopage* p)
{ return ci_dma_addr_cast_in(p->dma_addr); }

#define efhw_iopage_ptr(iop)              efhw_page_ptr(&(iop)->p)


/*--------------------------------------------------------------------
 *
 * struct efhw_iopages: A set of pages that are contiguous in physical
 * memory.  Directly mapped in the driver, and can be mapped to userlevel.
 * Can also be accessed by the NIC.
 *
 * NB. The O/S may be unwilling to allocate many, or even any of these.  So
 * only use this type where the NIC really needs a physically contiguous
 * buffer.
 *
 *--------------------------------------------------------------------*/

struct efhw_iopages {
  caddr_t           kva; 
  unsigned          order;
  PHYSICAL_ADDRESS  dma_addr;
  void *            wdf_cbuffer;
};


ci_inline char* efhw_iopages_ptr(struct efhw_iopages* p)
{ return (char*)p->kva; }

/* efhw_iopages_pfn is only used by linux nopage code and therefore
 * not needed on Windows 
ci_inline unsigned efhw_iopages_pfn(struct efhw_iopages* p)
{ return (unsigned) (__pa(p->kva) >> CI_PAGE_SHIFT); }

**/

ci_inline dma_addr_t efhw_iopages_dma_addr(struct efhw_iopages* p)
{ return ci_dma_addr_cast_in(p->dma_addr); }

ci_inline unsigned efhw_iopages_size(struct efhw_iopages* p)
{ return 1u << (p->order + CI_PAGE_SHIFT); }

#endif /* __CI_EFHW_IOPAGE_WIN32_H__ */
