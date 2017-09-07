#ifndef SOLARFLARE_SF_PCI_IDS_H
#define SOLARFLARE_SF_PCI_IDS_H 1

#define SFU_DEVICE_TYPE(pci_id) ((pci_id) & 0x0F00)

namespace solarflare
{
    // These values allows to distinguish NIC models by PCI device ID
    enum sfu_device_types {
      SFU_DEVICE_TYPE_FALCON     = 0x0700,
      SFU_DEVICE_TYPE_SIENA      = 0x0800,
      SFU_DEVICE_TYPE_HUNTINGTON = 0x0900,
      SFU_DEVICE_TYPE_MEDFORD    = 0x0A00
    };
};

#endif
