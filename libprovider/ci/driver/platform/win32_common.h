/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER >
** \author  
**  \brief  Defs common to all win32 code in or using drivers.
**   \date  
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_driver_platform  */

#ifndef __CI_DRIVER_PLATFORM_WIN32_H__
#define __CI_DRIVER_PLATFORM_WIN32_H__


/*--------------------------------------------------------------------
 *
 * Win2000 IOC magic numbers
 *
 *--------------------------------------------------------------------*/

#define EFAB_FILE_DEVICE  	0x00008000
#define EFAB_IOC_OFF  		0x00000800

#define _IOC( _FN_ )   CTL_CODE(EFAB_FILE_DEVICE,   \
			        (_FN_ + EFAB_IOC_OFF),  \
                                METHOD_BUFFERED,   \
                                FILE_ANY_ACCESS)


#if !defined(__ci_driver__)
# include <winioctl.h>
# include <winsock.h>
#endif


#endif  /* __CI_DRIVER_PLATFORM_WIN32_H__ */

/*! \cidoxg_end */
