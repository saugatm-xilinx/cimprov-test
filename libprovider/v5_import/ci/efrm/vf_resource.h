/****************************************************************************
 * Driver for Solarflare network controllers -
 *          resource management for Xen backend, OpenOnload, etc
 *
 * This file provides public API for VF resource.
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

#ifndef __CI_EFRM_VF_RESOURCE_H__
#define __CI_EFRM_VF_RESOURCE_H__

#include <ci/efrm/resource.h>
#include <ci/efrm/vi_allocation.h>

struct efrm_vf;
struct efrm_client;

extern void
efrm_vf_manager_params(unsigned *vi_base_out, unsigned *vi_scale_out,
		       unsigned *vf_count_out);

extern struct efrm_resource *
efrm_vf_to_resource(struct efrm_vf *) __attribute__ ((__pure__));
extern struct efrm_vf *
efrm_vf_from_resource(struct efrm_resource *) __attribute__ ((__pure__));

extern int
efrm_vf_resource_alloc(struct efrm_client *, struct efrm_vf **vf_out);
extern void
efrm_vf_resource_release(struct efrm_vf *);
extern void
efrm_vf_resource_free(struct efrm_vf *);

extern int
efrm_vf_get_nic_index(struct efrm_vf *) __attribute__ ((__pure__));


extern int
efrm_vf_alloc_vi_set(int min_vis_in_set, struct efrm_vi_allocation *set_out);
extern int
efrm_vf_free_vi_set(struct efrm_vi_allocation *set);

extern int efrm_vf_vi_setup(struct efrm_vi *vi, const char *name);
extern void efrm_vf_vi_drop(struct efrm_vi *virs);
extern int
efrm_vf_vi_q_init(struct efrm_vi *virs, enum efhw_q_type q_type,
		  int n_q_entries,
		  const dma_addr_t *dma_addrs, int dma_addrs_n,
		  int q_tag_in, unsigned q_flags, struct efrm_vi *evq);


extern int efrm_vf_eventq_callback_registered(struct efrm_vi *virs);
extern void efrm_vf_eventq_callback_kill(struct efrm_vi *virs);
extern void efrm_vf_vi_qmoderate(struct efrm_vi *virs, int usec);
extern int efrm_vf_vi_set_cpu_affinity(struct efrm_vi *virs, int cpu);


#endif /* __CI_EFRM_VF_RESOURCE_H__ */

