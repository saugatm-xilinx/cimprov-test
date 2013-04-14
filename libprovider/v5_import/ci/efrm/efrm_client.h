#ifndef __EFRM_CLIENT_H__
#define __EFRM_CLIENT_H__


struct efrm_client;


struct efrm_client_callbacks {
	void (*post_reset)(struct efrm_client *, void *user_data);
};


struct efrm_client_attr {
	/* Bitmask of supported ring sizes indexed by efhw_q_type. */
	unsigned vi_ring_sizes[3];
	unsigned vi_ring_doorbell_off[3];
};


/* Selects an arbitrary interface. */
#define EFRM_IFINDEX_DEFAULT  -1


/* NB. Callbacks may be invoked even before this returns. */
extern int  efrm_client_get(int ifindex, struct efrm_client_callbacks *,
			    void *user_data, struct efrm_client **client_out);
extern void efrm_client_put(struct efrm_client *);
extern void efrm_client_add_ref(struct efrm_client *);

extern
const struct efrm_client_attr *efrm_client_get_attr(struct efrm_client *);
extern struct efhw_nic *efrm_client_get_nic(struct efrm_client *);
extern int efrm_client_get_ifindex(struct efrm_client *);

extern int efrm_nic_present(int ifindex);

#if 0
/* For each resource type... */
extern void efrm_x_resource_resume(struct x_resource *);
#endif


#endif  /* __EFRM_CLIENT_H__ */
