/*!************************************************************************\
** \file    nci_link_fns.h
** \author  cgg
**  \brief  Common header for "direct function call" NIC Client Interface
**   \date  2006/12/11
**    \cop  (c) 2006 Solarflare Communications
\**************************************************************************/

#ifndef _EFAB_NCI_LINK_FNS_H
#define _EFAB_NCI_LINK_FNS_H

/* This header is used as a definition of this functional interface used
   between the main stack and, potentially, more than one independently-
   compiled body of code.

   It will be easier to maintain the code-base if no unnecessary definitions
   are included in this header.
*/

#define EFX_NCI_LINK_VERSION 1

typedef struct {
    int version;  /* Expect to be EFX_NCI_LINK_VERSION */
    int /*rc*/ (*interface_open)(void);
    void (*interface_close)(void);

    /* debugging */
    int ci_debug_code_level; /* needs converting to functions */

    /* library - provide in module - not like this */
    int /*rc*/ (*copy_from_user)(void *kdest, void *usrc, unsigned len);
    int /*rc*/ (*copy_to_user)(void *udest, void *ksrc, unsigned len);
    int /*rc*/ (*wdm_ci_complete_io)(ci_os_file fp, PVOID overlapped,
                                     ci_uint32 status, ci_uint32 bytes,
			             int do_completion_port);
    int /*rc*/ (*wdm_schedule_user_apc)(PKTHREAD thread, 
                                        PKNORMAL_ROUTINE callback,
                                        PVOID arg1, PVOID arg2, PVOID arg3);
    void (*ci_KeFlushQueuedDpcs)(void);
    
    /* ID pools - provide in bus driver, not like this? */
    int /*rc*/ (*ci_id_pool_ctor)(ci_id_pool_t* idp, int max_ids,
				  int init_size);
    void (*ci_id_pool_dtor)(ci_id_pool_t* idp);
    
    /* ci_driver operations */
    void (*ci_assert_il_driver_valid)(void);
    efrm_resource_manager_t *(*ci_il_driver_get_rm)(ci_uint32 rm);

    /* memory buffers */
    int /*rc*/ (*ci_contig_shmbuf_mmap)(ci_contig_shmbuf_t* kus,
					unsigned offset,
                                        unsigned long* bytes, void* opaque,
                                        int* map_num, unsigned long* p_offset);
    int /*rc*/ (*ci_shmbuf_alloc)(ci_shmbuf_t* b, unsigned bytes);
    void (*ci_shmbuf_free)(ci_shmbuf_t* b);
    int /*rc*/ (*ci_shmbuf_mmap)(ci_shmbuf_t* b, unsigned offset,
			         unsigned long* bytes, void* opaque,
			         int* map_num, unsigned long* p_offset);

    /* resource managers */
    int /*rc*/ (*efrm_resource_manager_alloc)(ci_resource_alloc_t* alloc,
				              struct efrm_resource_s** rs_out);
    void (*efrm_resource_manager_assert_valid)(efrm_resource_manager_t *rm,
                                               const char *file, int line);
    int /*rc*/ (*efrm_resource_manager_reassign_id)(ci_resource_table_t* rt,
				                    efrm_resource_id_t id,
				                    struct efrm_resource_s *
						        new_rs);

    /* resources */
    int /*rc*/(*efrm_resource_manager_allocate_id)(ci_resource_table_t* rt,
				                   struct efrm_resource_s* rs,
				                   efrm_resource_id_t* id_out);
    void (*efrm_resource_assert_valid)(struct efrm_resource_s*,
                                       int rc_may_be_zero,
                                       const char *file, int line);
    int /*rc*/ (*efrm_resource_default_op)(efrm_resource_t* rs,
					   ci_resource_op_t* op);
    void (*efrm_resource_free_instance)(efrm_resource_handle_t handle, int id,
					struct ci_id_pool_s*,
					ci_irqlock_t* lock);
    int /*rc*/ (*efrm_resource_id_lookup)(efrm_resource_id_t id, 
				          ci_resource_table_t *priv,
				          struct efrm_resource_s **out);
    int /*rc*/ (*efrm_resource_lookup)(efrm_resource_handle_t h,
				       int /*bool*/ check_nonce, 
				       struct efrm_resource_s** rs_out);

    /* tcp helper resource */
    int /*rc*/ (*efab_tcp_helper_bind_os_sock)(tcp_helper_resource_t* trs,
                                               oo_sp sock_id,
					       ci_resource_op_t* op,
                                               ci_boolean_t is_user_addr);
    
    /* endpoint resource */
    int /*rc*/ (*efab_pt_flush)(efrm_resource_t *pt);
    int /*rc*/ (*efrm_pt_pace)(efrm_resource_t *pt, int val);

    /* virtual interface resoruces */
    void (*efrm_vi_resource_mappings)(struct vi_resource_s* vi,
                                      void * out_vi_data);

    /* filter resources */
    int /*rc*/ (*efab_filter_resource_clear)(filter_resource_t* frs);
    int /*rc*/ (*__efab_filter_resource_set)(filter_resource_t* frs, int type, 
				             unsigned saddr_be32,
					     ci_uint16 sport_be16,
				             unsigned daddr_be32,
					     ci_uint16 dport_be16);
    
    /* event queues */
    ci_iohandle_t (*efab_eventq_handle)(vi_resource_t* qrs, int nic_index);
    void (*efab_eventq_kill_callback)(vi_resource_t* virs);
    void (*efab_eventq_register_callback)(vi_resource_t *virs,
					  efab_eventq_handler_fn_t *handler,
                                          void *arg);
    void (*efab_eventq_request_wakeup)(vi_resource_t *virs,
				       unsigned current_ptr, 
				       unsigned nic_index);
    void (*efab_eventq_reset)(vi_resource_t *virs, int nic_index);
    volatile char* (*efab_eventq_timer_reg)(vi_resource_t *virs, int nic_index);

    /* private file information */
    ci_private_t * (*ci_fpriv)(struct file *fp);
    struct file *(*ci_privf)(ci_private_t *rt);
    void (*efab_priv_dump)(ci_resource_table_t *rt, const char *context);

    /* DHCP handling */
    void (*ci_snoop_set_dhcp_handler)(ci_dhcp_handler_fn_t *handler);
    
} efx_nci_fns_t;

#endif /* _EFAB_NCI_LINK_FNS_H */
