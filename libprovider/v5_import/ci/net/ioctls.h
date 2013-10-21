/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER >
** \author  adp
**  \brief  Ioctls for ioctl() call compatibilty 
**   \date  2004/7/29
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_net  */

#ifndef __CI_NET_IOCTLS_H__
#define __CI_NET_IOCTLS_H__

#ifdef  __sun__
# include  <sys/filio.h>
# include  <sys/sockio.h>
#endif

#ifdef __linux__
# define SIOCINQ  FIONREAD
# define SIOCOUTQ TIOCOUTQ
# ifndef SIOCGSTAMPNS
#  define SIOCGSTAMPNS 0x8907
# endif
#endif /* __linux__ */

#endif /* __CI_NET_IOCTLS_H__ */
