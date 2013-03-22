/****************************************************************************
 * Driver for Solarflare network controllers -
 *          resource management for Xen backend, OpenOnload, etc
 *
 * This file provides private API for VF resource.
 *
 * Copyright 2011-2011: Solarflare Communications Inc,
 *                      9501 Jeronimo Road, Suite 250,
 *                      Irvine, CA 92618, USA
 *
 * Developed and maintained by Solarflare Communications:
 *                      <linux-xen-drivers@solarflare.com>
 *                      <onload-dev@solarflare.com>
 *
 * Certain parts of the driver were implemented by
 *          Alexandra Kossovsky <Alexandra.Kossovsky@oktetlabs.ru>
 *          OKTET Labs Ltd, Russia,
 *          http://oktetlabs.ru, <info@oktetlabs.ru>
 *          by request of Solarflare Communications
 *
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, incorporated herein by reference.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 ****************************************************************************
 */


#ifndef __CI_EFRM_VF_RESOURCE_INTERNAL_H__
#define __CI_EFRM_VF_RESOURCE_INTERNAL_H__

#include <ci/efrm/vf_resource.h>

#define EFRM_VF_MAX_VI_COUNT 64
#define EFRM_VF_NAME_LEN 32

#define MAC_ADDR_FMT							\
	"%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ADDR_VAL(_addr)						\
	(_addr)[0], (_addr)[1], (_addr)[2],				\
	(_addr)[3], (_addr)[4], (_addr)[5]

/* OS-dependent part should define this struct for real. */
struct pci_dev;

/* DMA mapping */
struct efrm_dma_map {
	dma_addr_t dma_addr;
	void *base_ptr;
};

/* VF VI */
struct efrm_vf_vi {
	/* Filled at VF PCI probe time: */
	int index;                      /* instance number of this VI */
	u32 irq;                        /* IRQ vector */

	/* Filled at allocation time: */
	char name[EFRM_VF_NAME_LEN];    /* human-readable name */
	struct efrm_vi *virs;   /* VI resource we are used with */

	/* VI-in-VF specific data: */
	struct tasklet_struct irq_tasklet;
	u32 irq_usec;
};

/* VF itself */
struct efrm_vf {
	struct efrm_resource rs;
	struct list_head link;
	int nic_index;

	/* Number of this VF and VIs */
	int pci_dev_fn;
	int vi_base;
	u8 vi_count; /* Real number of VIs: rxq+txq+evq+irq */

	struct pci_dev *pci_dev;

	void *bar; /* BAR0 */

	struct efrm_dma_map req;
	unsigned int req_seq;
	struct efrm_dma_map status;

	/* Data from the status page: */
	u8 vi_scale;
	u8 max_txq_count;
	u8 mac_addr[ETH_ALEN];

	u8 irq_count;

	struct efrm_vf_vi vi[EFRM_VF_MAX_VI_COUNT];

	struct efrm_buddy_allocator vi_instances;
};
#define efrm_vf(rs1)  container_of((rs1), struct efrm_vf, rs)
#define vi_to_vf(vi) container_of(vi, struct efrm_vf, vi[vi->index])

extern int efrm_vf_probed(struct efrm_vf *vf);
extern void efrm_vf_removed(struct efrm_vf *vf);
extern int efrm_vf_interrupts_probe(struct efrm_vf *vf);


#endif /* __CI_EFRM_VF_RESOURCE_INTERNAL_H__ */

