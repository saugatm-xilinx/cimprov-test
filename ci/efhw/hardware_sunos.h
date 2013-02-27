
#ifndef __CI_EFHW_HARDWARE_SUNOS_H__
#define __CI_EFHW_HARDWARE_SUNOS_H__

#include <ci/efhw/hardware_ci2linux.h>

/**********************************************************************
 * register access
 */

typedef ddi_acc_handle_t		ci_iohandle_t;

#error rewrite this code!
/* This code should be rewritten to provide readl/writel.
 *
 * If we always use one ddi_acc_handle_t, just keep it in a global variable
 * and use directly.
 *
 * If we use a few of ddi_acc_handle_t, we should create an array of
 * handles and determine index of the handle by the most significant byte
 * of efhw_ioaddr_t. */

ci_inline void
ci_put8(ci_iohandle_t handle, efhw_ioaddr_t addr, ci_uint8 v)
{
  ddi_put8(handle, (ci_int8 *)addr, v);
}

ci_inline ci_uint8
ci_get8(ci_iohandle_t handle, efhw_ioaddr_t addr)
{
  return ddi_get8(handle, (ci_uint8 *)addr);
}


ci_inline void
ci_put16(ci_iohandle_t handle, efhw_ioaddr_t addr, ci_uint16 v)
{
  ddi_put16(handle, (ci_uint16 *)addr, v);
}

ci_inline ci_uint16
ci_get16(ci_iohandle_t handle, efhw_ioaddr_t addr)
{
  return ddi_get16(handle, (ci_uint16 *)addr);
}


ci_inline void
ci_put32(ci_iohandle_t handle, efhw_ioaddr_t addr, ci_uint32 v)
{
  ddi_put32(handle, (ci_uint32 *)addr, v);
}

ci_inline ci_uint32
ci_get32(ci_iohandle_t handle, efhw_ioaddr_t addr)
{
  return ddi_get32(handle, (ci_uint32 *)addr);
}

ci_inline void
ci_put64(ci_iohandle_t handle, efhw_ioaddr_t addr, ci_uint64 v)
{
  ddi_put64(handle, (ci_uint64 *)addr, v);
}

ci_inline ci_uint64
ci_get64(ci_iohandle_t handle, efhw_ioaddr_t addr)
{
  return ddi_get64(handle, (ci_uint64 *)addr);
}


#endif /* __CI_EFHW_HARDWARE_SUNOS_H__ */
