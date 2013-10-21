#ifndef __CI_DRIVER_EFAB_WDF_EFAB_NIC_H__
#define __CI_DRIVER_EFAB_WDF_EFAB_NIC_H__


#define IOPAGELIST_SHIFT    8
#define IOPAGELIST_SIZE     (1 << IOPAGELIST_SHIFT)
#define IOPAGELIST_MASK     ((IOPAGELIST_SIZE) - 1)
#define IOPAGELIST_LOWTIDE  (IOPAGELIST_SIZE >> 1)
#define IOPAGELIST_HIGHTIDE (IOPAGELIST_SIZE-1)


typedef struct iopagelist_s {
  ci_uint32 first;
  ci_uint32 last;
  ci_uint32 n_iopages;
  KSPIN_LOCK lock;
  union {
  WDFWORKITEM wdf;
  PIO_WORKITEM wdm;
  } workitem;
  ci_uint32 allocqueued;
  ci_uint32 allocate;
  struct efhw_iopage ptrlist[IOPAGELIST_SIZE];
} iopagelist_t;


/* todo better place for this */
#define EFHW_BAR_NUM_MAX 5

typedef struct {
  struct efhw_nic nic;

  /* WDF bus driver FDO context */
  WDFDEVICE device_object;
  WDFDMAENABLER dma_enabler;
  ci_phys_addr_t pci_addr[EFHW_BAR_NUM_MAX];
  iopagelist_t iopagepool;

} wdf_efab_nic_t;


#define WDF_EFAB_NIC(enic)  CI_CONTAINER(wdf_efab_nic_t, nic, (enic))


#endif  /* __CI_DRIVER_EFAB_WDF_EFAB_NIC_H__ */
