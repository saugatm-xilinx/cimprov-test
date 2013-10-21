/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER >
** \author  
**  \brief  Common defs for win32 kernel-mode drivers.
**   \date  
**    \cop  (c) 2004-2005 Level 5 Networks Limited.
**              2006 Solarflare Communications
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_driver_platform  */

#ifndef __CI_DRIVER_PLATFORM_WIN32_KERNEL_H__
#define __CI_DRIVER_PLATFORM_WIN32_KERNEL_H__

#include <onload/osfile.h>  /* for ci_os_file */

#define ci_is_sysadmin() (1)

#define  PCI_BASE_ADDRESS_SPACE	        0x01	/* 0 = memory, 1 = I/O */
#define  PCI_BASE_ADDRESS_SPACE_IO      0x01
#define  PCI_BASE_ADDRESS_SPACE_MEMORY  0x00
#define  PCI_BASE_ADDRESS_MEM_MASK	(~0x0fUL)

#define FD_READ    0x01 /* Readiness for reading notification */
#define FD_WRITE   0x02 /* Readiness for writing notification */
#define FD_OOB     0x04 /* The arrival of OOB data notification */
#define FD_ACCEPT  0x08 /* Incoming connections notification */
#define FD_CONNECT 0x10 /* Completed connection (?) notification */
#define FD_CLOSE   0x20 /* Socket closure notification */

#define AF_UNSPEC  0

#define SHUT_RD    1
#define SHUT_WR    2
#define SHUT_RDWR  3


/*--------------------------------------------------------------------
 *
 * ci_contig_shmbuf_t: A (potentially) large buffer that is contiguous in
 * the driver address space, and may be mapped to userlevel.
 *
 *--------------------------------------------------------------------*/

typedef struct {
  char*         p;
  unsigned      bytes;
} ci_contig_shmbuf_t;


ci_inline int ci_contig_shmbuf_alloc(ci_contig_shmbuf_t* kus, unsigned bytes) 
{
  ci_assert(bytes > 0);
  ci_assert(kus);
  kus->bytes = CI_ROUND_UP(bytes, CI_PAGE_SIZE);
  kus->p = ci_alloc(kus->bytes);
  if (kus->p == NULL)  return -ENOMEM;
  return 0;
}

ci_inline void ci_contig_shmbuf_free(ci_contig_shmbuf_t* kus) 
{
  ci_assert(kus);  
  ci_assert(kus->p);
  ci_free(kus->p);
  CI_DEBUG_ZERO(kus);
}

ci_inline caddr_t ci_contig_shmbuf_ptr(ci_contig_shmbuf_t* kus)
{ return kus->p; }

ci_inline size_t ci_contig_shmbuf_size(ci_contig_shmbuf_t* kus)
{ return kus->bytes; }

extern int ci_contig_shmbuf_mmap(ci_contig_shmbuf_t* kus, unsigned offset,
                                unsigned long* bytes, void* opaque,
                                int* map_num, unsigned long* p_offset);


#define AF_INET         2               /* internetwork: UDP, TCP, etc. */

// FIXME, the following function is a dummy that needs to be implemented
#define ci_waitable_init_timeout(t, timeval) \
  do { \
    (t)->hnsecs = 0;    \
    (t)->timedout = 0;  \
    ci_assert(0);       \
  } while (0);

typedef int socklen_t;

/*--------------------------------------------------------------------
 *
 * ci_os_file support: functions 
 *
 *--------------------------------------------------------------------*/

#ifdef __ci_wdf__
ci_inline PFILE_OBJECT ci_os_file_wdmfo(ci_os_file obj)
{   return WdfFileObjectWdmGetFileObject(obj);
}
ci_inline ci_os_file ci_os_file_from_wdmfo(PFILE_OBJECT pfileobj)
{ PDEVICE_OBJECT wdm_dev = IoGetRelatedDeviceObject(pfileobj);
  if (NULL == wdm_dev)
    return CI_OS_FILE_BAD;
  else {
    WDFDEVICE wdf_dev = WdfWdmDeviceGetWdfDeviceHandle(wdm_dev);
    if (NULL == wdf_dev)
      return CI_OS_FILE_BAD;
    else
      return WdfDeviceGetFileObject(wdf_dev, pfileobj);
  }
}
#else
ci_inline PFILE_OBJECT ci_os_file_wdmfo(ci_os_file obj)
{   return obj;
}
ci_inline ci_os_file ci_os_file_from_wdmfo(PFILE_OBJECT pfileobj)
{   return pfileobj;
}
#endif

ci_inline void
ci_get_file(ci_os_file file)
{
  ObReferenceObject(ci_os_file_wdmfo(file));
}

ci_inline void
ci_fput(ci_os_file file)
{
  ObDereferenceObject(ci_os_file_wdmfo(file));
}

ci_inline ci_os_file
ci_fget(HANDLE handle)
{
  PFILE_OBJECT file = NULL;

  /* This call increments the use count on the PFILE_OBJECT */
  ObReferenceObjectByHandle(handle, GENERIC_ALL, NULL,
			    KernelMode, &file, NULL);

  return ci_os_file_from_wdmfo(file);
}

extern struct ci_private_s *ci_fpriv(ci_os_file);
extern ci_os_file ci_privf(struct ci_private_s *);

/* Any better suggestions on where to put this define welcome */
#define EFAB_MAX_NO_MULTICAST_ADDRESSES   32

#endif  /* __CI_DRIVER_PLATFORM_WIN32_KERNEL_H__ */
/*! \cidoxg_end */
