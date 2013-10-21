/**************************************************************************\
*//*! \file win_idc.h - header file for windows inter-driver comms module.
** <L5_PRIVATE L5_HEADER >
** \author  mch
**  \brief  Header file for inter-driver comms module.
**   \date  2005/10
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef _WIN_IDC_H_
#define _WIN_IDC_H_

/*! \cidoxg_driver_win */

/* Common definitions */

#define IDC_TBL_HDR_MAGIC       (0x11112222)
#define IDC_IF_HDR_MAGIC        (0x22223333)
#define IDC_TBL_VER             (0)
#define IDC_IF_VER              (0)

/* Definitions for the various module types that export interface tables. */
#define   IDC_MOD_TYPE_NDIS             (0)
#define   IDC_MOD_TYPE_WDM              (1)

typedef enum {
  IDC_SECT_STARTING = 0,
  IDC_SECT_AVAIL,
  IDC_SECT_CLOSING,
} IDC_SECT_STATE;

/* N.B This struct is used by both client and server,
   so you probably don't want to change it without keeping
   both drivers in step. */
typedef struct idc_tbl_hdr_s {
  ULONG magic;                  /* Lock not required (unchanged for lifetime). */
  ULONG version;                /* Lock not required (unchanged for lifetime). */
  ULONG module_type;            /* Lock not required (unchanged for lifetime). */
  ULONG module_instance;        /* Lock not required (unchanged for lifetime). */
  KSPIN_LOCK section_lock;
  IDC_SECT_STATE sect_state;    /* Use section lock once IoNotifications enabled. */
  LIST_ENTRY client_entries;    /* Use section lock. */
  ULONG interface_offset;       /* Lock not required (unchanged for lifetime). */
} IDC_TBL_HDR_T, *PIDC_TBL_HDR_T;

/*Expect to have entered this function holding
  the section lock, with the final IRQL to return to specified.
  Will leave function at final irql, holding no locks. */
typedef NTSTATUS (*p_srv_cleanup)(PVOID pCommonEntry, KIRQL FinalIrql);

/*N.B This struct is used by both client and server,
  so you probably don't want to change it without keeping
  both drivers in step. */
typedef struct idc_common_entry_s {
  p_srv_cleanup       cleanup_function;       /* Lock not required (unchanged for lifetime). */
  LIST_ENTRY          section_link;           /* Use section lock. */
} IDC_COMMON_ENTRY_T, *PIDC_COMMON_ENTRY_T;

/* Common declarations of link structures - Windows specific. */

typedef unsigned int LINK_IDC_LINK_ID;

typedef enum link_idc_linkstate {
  /* WIL_NEITHER       - Entry does not exist. */
  WIL_NDIS_ONLY,
  WIL_WDM_ONLY,
  WIL_BOTH,
  WIL_ASYNC_CLOSE
} LINK_IDC_LINKSTATE_T;

#define LINK_IDC_NW_LINK_MAGIC   (0x84211248)

/* N.B This struct is used by both client and server,
   so you probably don't want to change it without keeping
   both drivers in step. */
typedef struct link_idc_nw_link_s {
  ULONG                 magic;
  LIST_ENTRY            list_field;      /* parent lock */
  KSPIN_LOCK            lock;
  LINK_IDC_LINK_ID       id;
  LINK_IDC_LINKSTATE_T   state;          /* use lock */
  PVOID                 ndis_tbl_handle; /* use lock */
  PVOID                 ndis_tbl;        /* use lock */
  struct efhw_nic_s     *ndis_nic;       /* use lock */
  struct efhw_nic_s     *wdm_nic;        /* use lock */
} LINK_IDC_NW_LINK_T, *PLINK_IDC_NW_LINK_T;

/* Client Internal */

NTSTATUS idc_cli_pnp_handler (PVOID notification_structure, PVOID context);

#define IDC_CLI_STRUCT_MAGIC    (0xABCDDCBA)
#define IDC_CLI_ENTRY_MAGIC     (0xBABABABA)
#define IDC_CLI_REF_MAGIC       (0xCACACACA)

typedef enum {
  IDC_STRUCT_AVAIL = 0,
  IDC_STRUCT_CLOSING
} IDC_STRUCT_STATE;


typedef enum {
  IDC_ENT_AVAIL = 0,
  IDC_ENT_CLOSING
} IDC_ENT_STATE;

typedef VOID (*idc_close_notify_func)(IN PVOID handle, IN PVOID int_table, IN PVOID context);
typedef VOID (*idc_new_notify_func)(IN PVOID idc_cli_struct, IN PVOID context);

typedef struct idc_cli_struct_t {
  ULONG                     magic;            /* Lock not required (unchanged for lifetime). */
  PDEVICE_OBJECT            fdo;              /* Lock not required (unchanged for lifetime). */
  KSPIN_LOCK                cli_lock;         /* Protects list of entries against concurrent modification. */
  LIST_ENTRY                entry_list;       /* Use Cli Lock. */
  IDC_STRUCT_STATE          state;            /* Use Cli Lock - prevents a fight between shutting down & new interface PnP notifies. */
  NPAGED_LOOKASIDE_LIST     int_ref_alloc_list;/* Lock not required - allocs are atomic */
  idc_new_notify_func       new_func;          /* Lock not required (unchanged for lifetime). */
  PVOID                     new_context;       /* Lock not required (unchanged for lifetime). */
  PVOID                     notification_entry;/* Lock not required (PnP thread only). */
  NPAGED_LOOKASIDE_LIST     entry_alloc_list;  /* Lock not required (PnP thread only). */
} IDC_CLI_STRUCT_T, *PIDC_CLI_STRUCT_T, **PPIDC_CLI_STRUCT_T;

typedef struct idc_cli_entry_s {
  ULONG               magic;                    /* Lock not required (unchanged for lifetime). */
  PIDC_CLI_STRUCT_T   p_cli_struct;             /* Lock not required (unchanged for lifetime). */
  LIST_ENTRY          entry_link;               /* Use Cli Lock. */
  IDC_COMMON_ENTRY_T  common_info;              /* Section lock required for *some* of this structure. */
  KSPIN_LOCK          entry_lock;               /* Protects state, refs, events and int, but not the Entry link. */
  IDC_ENT_STATE       state;                    /* Use entry lock. */
  LIST_ENTRY          ref_list;                 /* Use entry lock. */
  KEVENT              wait_no_refs;             /* Can signal from within entry lock. */
  PVOID               int_table;                /* Lock not required (unchanged for lifetime). */
  UNICODE_STRING      symbolic_link_name;
} IDC_CLI_ENTRY_T, *PIDC_CLI_ENTRY_T, **PPIDC_CLI_ENTRY_T;

typedef struct idc_cli_int_ref_s {
  ULONG                   magic;                   /* Lock not required (unchanged for lifetime). */
  PIDC_CLI_ENTRY_T        p_entry;                 /* Lock not required (unchanged for lifetime). */
  LIST_ENTRY              ref_link;                /* Use entry lock. */
  idc_close_notify_func   close_func;              /* Lock not required (unchanged for lifetime). */
  PVOID                   close_context;           /* Lock not required (unchanged for lifetime). */
  PVOID                   temp_handle;             /* Only used in temporary copies. */
} IDC_CLI_INT_REF_T, *PIDC_CLI_INT_REF_T, **PPIDC_CLI_INT_REF_T;

/* Client External */

#define FIND_FLAG_TYPE    (0x1)
#define FIND_FLAG_INST    (0x2)

NTSTATUS idc_cli_startup (IN PDRIVER_OBJECT driver_object, 
                          IN idc_new_notify_func notify_func,
                          IN PVOID notify_context,
                          OUT PPIDC_CLI_STRUCT_T p_idc_cli );

NTSTATUS idc_cli_shutdown( IN PIDC_CLI_STRUCT_T p_idc_cli);

/* Find an interface table, increment reference and setup closure callback for that table. */
NTSTATUS idc_cli_find_an_interface_tbl (IN PIDC_CLI_STRUCT_T p_idc_cli,
                                        IN ULONG module_type,
                                        IN ULONG module_instance,
                                        IN ULONG find_flags,  
                                        IN idc_close_notify_func close_func,
                                        IN PVOID close_context,
                                        OUT PVOID *handle,
                                        OUT PVOID *p_int_table);

/* Find another interface table, according to criteria, whilst releasing reference on old table. */
NTSTATUS idc_cli_find_another_interface_tbl(IN ULONG module_type,
                                            IN ULONG module_instance,
                                            IN ULONG find_flags,
                                            IN PVOID previous_handle, 
                                            OUT PVOID *new_handle,
                                            OUT PVOID *p_int_table);

/* Get another reference and callback to an existing table. */
NTSTATUS idc_cli_get_interface_tbl(IN PVOID existing_handle, 
                                   IN idc_close_notify_func new_close_func,
                                   IN PVOID new_close_context,
                                   OUT PVOID *new_handle,
                                   OUT PVOID *p_int_tbl);

/* Release reference and callback to existing table. */
NTSTATUS idc_cli_put_interface_tbl(IN PVOID handle);

/* Server internal */

#define IDC_SRV_STRUCT_MAGIC     (0x12344321)

typedef struct idc_srv_struct_s {
  ULONG                     magic;
  UNICODE_STRING            device_intf_sym_name;
  HANDLE                    kernel_addr_sect;
  HANDLE                    kernel_dir_object;
  PVOID                     int_table;
} IDC_SRV_STRUCT_T, *PIDC_SRV_STRUCT_T, **PPIDC_SRV_STRUCT_T;

/* Server External */

NTSTATUS idc_srv_create_tables(IN PDEVICE_OBJECT pdo, 
                               IN ULONG table_size, 
                               OUT PPIDC_SRV_STRUCT_T p_idc_srv);

NTSTATUS idc_srv_get_table_addr(IN PIDC_SRV_STRUCT_T p_idc_srv,
                                OUT PVOID* p_interface_table);

NTSTATUS idc_srv_activate_tables(IN PIDC_SRV_STRUCT_T p_idc_srv);

NTSTATUS idc_srv_deactivate_tables(IN PIDC_SRV_STRUCT_T p_idc_srv);

NTSTATUS idc_srv_destroy_tables(IN PIDC_SRV_STRUCT_T p_idc_srv);

#endif
