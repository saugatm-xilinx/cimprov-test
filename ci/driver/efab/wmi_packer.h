/**************************************************************************\
*//*! \file wmi_packer.h
** <L5_PRIVATE L5_HEADER >
** \author  aam
**  \brief  Generic wmi buffer access helper
**   \date  2006/07/13
**    \cop  (c) Solarflare Communications Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_iscsi */

#ifndef __CI_ISCSI_WMI_PACKER_H__
#define __CI_ISCSI_WMI_PACKER_H__

/*! Information is exchanged between WMI drivers and the system / applications
 * via buffers laid out in the format of various system defined structs.
 * Unfortunately these structs contain arrays that are sized at runtime, and
 * in particular strings are represented as arrays of WCHARs that are of a
 * fixed size in the struct definitions, but may be different sizes in the
 * buffers. This means that casting a buffer pointer to a struct is unreliable
 * at best, and also that sizeof(struct) cannot be used in relation to buffer
 * sizes. Matters are complicated further by issues of alignment.
 *
 * To alleviate these problems, the wmi packer util class is provided. A
 * WmiPacker runs over a wmi buffer adding or extracting data, and handling
 * the various sizing and alignment requirements. For this to work all data
 * must be added / extracted in order from the start of the buffer.
 *
 * It is also necessary to pretend to pack data into a buffer, without actually
 * doing so, do determine how large a buffer is needed. WmiPackers will accept
 * and supply data even after the buffer has been exhausted, keeping track of
 * the size required. The data to be put into the buffer in this circumstance
 * is simply thrown away, the data supplied from the buffer will be 0.
 *
 * The following code is not optimised for speed. It is assumed that if you're
 * handling wmi calls things are going to be slow anyway.
 *
 * This was originally written for the iSCSI driver, however there is no reason
 * it could not be used for other modules.
 *
 * This module should only be used at IRQL = PASSIVE_LEVEL.
 */

/*! WMI packer. This should be considered an opaque object and none of
 * it's members should be manipulated directly
 */
typedef struct _wmiPacker {
  char *pBuffer;     /* target buffer */
  ci_uint32 nSize;   /* size of buffer */
  ci_uint32 nIndex;  /* current position in buffer */
} WmiPacker;

/*! Setup a wmi packer to handle the specified buffer.
 * \param   pPacker          packer to initialise
 * \param   pBuffer          buffer to pack (may be NULL iff size is 0)
 * \param   nBufferSize      size of buffer (in bytes)
 */
extern void
ci_wmiPackerInit(WmiPacker *pPacker, void *pBuffer, ci_uint32 nBufferSize);

/*! Reset the walker to start again at the beginning of its buffer. Note that
 * anything already written in the buffer will still be there.
 * \param   pPacker          packer to reset
 */
extern void
ci_wmiPackerReset(WmiPacker *pPacker);

/*! Print out the state of this packer to the system log
 * \param   pPacker          packer to print
 */
extern void
ci_wmiPackerDump(WmiPacker *pPacker);

/*! Print out the contents of our wmi buffer
 * \param   pPacker          packer to print
 */
extern void
ci_wmiPackerDumpBuffer(WmiPacker *pPacker);

/*! Report the size of buffer needed for the data packed so far.
 * \param   pPacker          packer to query
 * \return  required size of buffer (in bytes)
 */
extern ci_uint32
ci_wmiPackerGetSize(WmiPacker *pPacker);

/*! Determine whether the data packed has overflowed the provided buffer
 * \param   pPacker          packer to query
 * \return  CI_TRUE if the packed data is too big for the provided buffer,
 *          CI_FALSE otherwise
 */
extern ci_boolean_t
ci_wmiPackerOverflowed(WmiPacker *pPacker);

/*! Get a pointer to an unsigned long at the current position in the buffer,
 * and advance as if we had added or retrieved an unsigned long. This is
 * provided to allow for fields that must be updated after other data is
 * processed, eg array size counters.
 * If the buffer has already overflowed a pointer to a junk field will be
 * returned. This allows use of the returned pointer without having to check
 * for a NULL, although obviously the value of the field it points to is
 * garbage.
 * \param   pPacker          packer to get from
 * \return  pointer to data in buffer
 */
extern ULONG *
ci_wmiPackerGetPointer(WmiPacker *pPacker);

/*! Skip over the specified number of bytes in the buffer, as if we had added
 * or retrieved those bytes.
 * Note that byte alignment is assumed.
 * \param   pPacker          packer to manipulate
 * \param   nCount           number of bytes to skip
 */
extern void
ci_wmiPackerSkipBytes(WmiPacker *pPacker, ci_uint32 nCount);


/*! Methods to add basic data to a buffer */

/*! Add a bool to the buffer.
 * \param   pPacker          packer to add to
 * \param   data             data to add
 */
extern void
ci_wmiPackerAddBool(WmiPacker *pPacker, BOOLEAN data);

/*! Add a char to the buffer.
 * \param   pPacker          packer to add to
 * \param   data             data to add
 */
extern void
ci_wmiPackerAddChar(WmiPacker *pPacker, UCHAR data);

/*! Add an unsigned short to the buffer.
 * \param   pPacker          packer to add to
 * \param   data             data to add
 */
extern void
ci_wmiPackerAddShort(WmiPacker *pPacker, USHORT data);

/*! Add an unsigned long to the buffer.
 * \param   pPacker          packer to add to
 * \param   data             data to add
 */
extern void
ci_wmiPackerAddLong(WmiPacker *pPacker, ULONG data);

/*! Add an unsigned long long to the buffer.
 * \param   pPacker          packer to add to
 * \param   data             data to add
 */
extern void
ci_wmiPackerAddLongLong(WmiPacker *pPacker, ULONGLONG data);

/*! Add a fixed size array of bytes to the buffer.
 * \param   pPacker          packer to add to
 * \param   nSize            size of byte array to add
 * \param   data             data to add, NULL to add 0s
 */
extern void
ci_wmiPackerAddBytes(WmiPacker *pPacker, ci_uint32 nSize, const char *data);

/*! Add a terminated char* string to the buffer.
 * Whilst we can make strings only as big as they need to be, some of
 * Microsofts own code assumes they are the maximum size they can be.
 * (See MSiSCSI_HBAInformation all text fields, and 
 * MSiSCSI_RedirectPortalInfoClass TextAddress fields in ISCSI_IP_Address.)
 * Therefore we always make them the maximum size. Note that we still need to
 * handle smaller strings on unpacking.
 * \param   pPacker          packer to add to
 * \param   nMaxSize         maximum size (in characters) of wmi string in
 *                           buffer (excluding size field)
 * \param   data             string to add
 */
extern void
ci_wmiPackerAddString(WmiPacker *pPacker, ci_uint32 nMaxSize,
                      const char *data);

/*! Add a WMI style string to the buffer.
 * \param   pPacker          packer to add to
 * \param   nMaxSize         maximum size (in characters) of wmi string in
 *                           buffer (excluding size field)
 * \param   data             string to add
 */
extern void
ci_wmiPackerAddWMIString(WmiPacker *pPacker, ci_uint32 nMaxSize,
                         const WCHAR *data);

/*! Add a counted wide char* string to the buffer.
 * \param   pPacker          packer to add to
 * \param   nMaxSize         maximum size of wmi string (excluding size field)
 * \param   nSize            size of string to add (in characters)
 * \param   data             string to add
 */
extern void
ci_wmiPackerAddLenString(WmiPacker *pPacker, ci_uint32 nMaxSize,
                         ci_uint32 nSize, const WCHAR *data);

/*! Skip (and zero out) bytes to move us to the required alignment.
 * \param   pPacker          packer to add to
 * \param   nAlign           required alignment in bytes (a power of 2)
 */
extern void
ci_wmiPackerAddAlign(WmiPacker *pPacker, int nAlign);


/*! Methods to get basic data from a buffer */

/*! Get a bool from the buffer.
 * \param   pPacker          packer to get from
 * \return  data from buffer
 */
extern BOOLEAN
ci_wmiPackerGetBool(WmiPacker *pPacker);

/*! Get an unsigned char from the buffer.
 * \param   pPacker          packer to get from
 * \return  data from buffer
 */
extern UCHAR
ci_wmiPackerGetChar(WmiPacker *pPacker);

/*! Get an unsigned short from the buffer.
 * \param   pPacker          packer to get from
 * \return  data from buffer
 */
extern USHORT
ci_wmiPackerGetShort(WmiPacker *pPacker);

/*! Get an unsigned long from the buffer.
 * \param   pPacker          packer to get from
 * \return  data from buffer
 */
extern ULONG
ci_wmiPackerGetLong(WmiPacker *pPacker);

/*! Get an unsigned long long from the buffer.
 * \param   pPacker          packer to get from
 * \return  data from buffer
 */
extern ULONGLONG
ci_wmiPackerGetLongLong(WmiPacker *pPacker);

/*! Get a fixed size array of bytes from the buffer.
 * Note that the returned pointer points to memory within the buffer, which
 * must not be deleted, and will be NULL if we have overflowed.
 * \param   pPacker          packer to get from
 * \param   nSize            size of byte array to get
 * \return  data from buffer, or NULL if packer has overflowed
 */
extern char *
ci_wmiPackerGetBytes(WmiPacker *pPacker, ci_uint32 nSize);

/*! Copy a fixed size array of bytes from the buffer.
 * \param   pPacker          packer to get from
 * \param   nSize            size of byte array to get
 * \param   pData            location to store retreive data
 */
extern void
ci_wmiPackerCopyBytes(WmiPacker *pPacker, ci_uint32 nSize, char *pData);

/*! Get a wmi string from the buffer (in wmi format).
 * Note that the returned pointer points to memory within the buffer, which
 * must not be deleted, and will be NULL if we have overflowed.
 * \param   pPacker          packer to get from
 * \return  data from buffer, or NULL if packer has overflowed
 */
extern WCHAR *
ci_wmiPackerGetString(WmiPacker *pPacker);

/*! Copy a wmi string from the buffer into the given fixed size char array
 * \param   pPacker          packer to get from
 * \param   nMaxSize         size of destination array (including terminator)
 * \param   pData            destination array to store retreive data
 * \return  number of characters of string in source string
 */
extern ci_uint32
ci_wmiPackerCopyString(WmiPacker *pPacker, ci_uint32 nMaxSize, char *pData);

/*! Copy a wmi string from the buffer into the given fixed size wide char array
 * \param   pPacker          packer to get from
 * \param   nMaxSize         size of destination array (including terminator)
 * \param   pData            destination array to store retreive data
 * \return  number of characters of string in source string
 */
extern ci_uint32
ci_wmiPackerCopyStringW(WmiPacker *pPacker, ci_uint32 nMaxSize, WCHAR *pData);

/*! Skip bytes to move us to the required alignment.
 * \param   pPacker          packer to add to
 * \param   nAlign           required alignment in bytes (a power of 2)
 */
extern void
ci_wmiPackerGetAlign(WmiPacker *pPacker, int nAlign);

/*! Get the all remaining space as a variable sized buffer
 * Note that the returned pointer points to memory within the buffer, which
 * must not be deleted, and will be NULL if we have overflowed.
 * \param   pPacker          packer to add to
 * \param   pSize            location to store available buffer size
 * \return  pointer to buffer, NULL for overflow
 */
extern void *
ci_wmiPackerGetVariable(WmiPacker *pPacker, ci_uint32 *pSize);


/*! Methods to handle higher level structures */

/*! Add an IP address struct (ISCSI_IP_Address) to the buffer.
 * Note that we currently only support IPv4 addresses.
 * \param   pPacker          packer to add to
 * \param   address          address to add
 */
extern void
ci_wmiPackerAddIPAddr(WmiPacker *pPacker, ULONG address);

/*! Get an IP address struct (ISCSI_IP_Address) from the buffer.
 * Note that we currently only support IPv4 addresses.
 * \param   pPacker          packer to add to
 * \param   pAddress         location to store retrieved address
 * \return  CI_TRUE on success, CI_FALSE if address is not IPv4
 */
extern ci_boolean_t
ci_wmiPackerGetIPAddr(WmiPacker *pPacker, ULONG *pAddress);

/*! Get an IP address struct (ISCSI_IP_Address) from the buffer, including the
 * address type.
 * Note that we currently only support IPv4 addresses.
 * \param   pPacker          packer to add to
 * \param   pAddress         location to store retrieved IPv4 address
 * \return  address type
 */
extern ULONG
ci_wmiPackerGetIPAddrType(WmiPacker *pPacker, ULONG *pAddress);

/*! Add a portal struct (ISCSI_TargetPortal) to the buffer.
 * Note that we currently only support IPv4 addresses.
 * \param   pPacker          packer to add to
 * \param   address          address to add
 * \param   port             TCP port number to add
 */
extern void
ci_wmiPackerAddPortal(WmiPacker *pPacker, ULONG address, ULONG port);

/*! Get an IP address struct (ISCSI_IP_Address) from the buffer.
 * Note that we currently only support IPv4 addresses.
 * \param   pPacker          packer to add to
 * \param   pAddress         location to store retrieved address
 * \param   pPort            location to store retrieved port
 * \return  CI_TRUE on success, CI_FALSE if address is not IPv4
 */
extern ci_boolean_t
ci_wmiPackerGetPortal(WmiPacker *pPacker, ULONG *pAddress, ULONG *pPort);

#endif //#ifndef __CI_ISCSI_WMI_PACKER_H__
