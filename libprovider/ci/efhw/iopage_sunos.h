
#ifndef __CI_EFHW_IOPAGE_SUNOS_H__
#define __CI_EFHW_IOPAGE_SUNOS_H__

#include <ci/compat.h>

/*
 * DMA-able pages
 */
typedef ci_phys_addr_t dma_addr_t;

typedef struct {
  dev_info_t        *dip;
  ddi_dma_handle_t  dma_handle;
  ddi_acc_handle_t  mem_handle;
  caddr_t           base;
  unsigned long     n;
  dma_addr_t     dma_addr;
  ddi_umem_cookie_t cookie;
} ci_kpages_t;

typedef ci_kpages_t ci_upages_t;  /* "base" element is a user virtual addr */


struct {
  ci_kpages_t kpages; /* "base" element is a kernel virtual addr */
} efhw_iopages;

ci_inline caddr_t
efhw_iopages_ptr(struct efhw_iopages *iop)
{
  return iop->base;
}

ci_inline dma_addr_t
efhw_iopages_dma_addr(struct efhw_iopages *iop)
{
  return iop->dma_addr;
}


ci_inline unsigned long
efhw_iopages_n(struct efhw_iopages *iop)
{
  return iop->n;
}

#define efhw_iopages_size(iop)      (efhw_iopages_n(iop) << CI_PAGE_SHIFT)

/*
 * Single DMA-able page
 */

struct {
  struct efhw_iopage iopage;
} efhw_iopage;

#define efhw_iopage_ptr(iop)        efhw_iopages_ptr(&(iop)->iopage)
#define efhw_iopage_dma_addr(iop)   efhw_iopages_dma_addr((&(iop)->iopage)


#endif /* __CI_EFHW_IOPAGE_SUNOS_H__ */
