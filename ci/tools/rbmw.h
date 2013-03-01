/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_HEADER >
** \author  djr
**  \brief  Functions to generate and analyse RBMW packets.
**   \date  2003/03/28
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/*! \cidoxg_include_ci_tools */

#ifndef __CI_TOOLS_RBMW_H__
#define __CI_TOOLS_RBMW_H__

#include <ci/tools.h>
#include <ci/net/rbmw.h>


/**********************************************************************
 * Packet parser interface.
 */

/*! RBMW state */
typedef struct {
  int		version;
  ci_uint32*	buf;
  ci_uint32*	buf_start;
  ci_uint32*	buf_end;
  unsigned	x25_crc;
  unsigned	usb_crc;
} ci_rbmw_state;

typedef struct ci_rbmw_visitor_s ci_rbmw_visitor;

/*! Comment? */
struct ci_rbmw_visitor_s {
  ci_rbmw_state* state;
  void (*error)(ci_rbmw_visitor*, int error_no, const char* msg);
  int  (*start_of_packet)(ci_rbmw_visitor*, ci_rbmw_packet_header*);
  void (*start_of_burst)(ci_rbmw_visitor*, unsigned offset_w, unsigned flags);
  void (*padding)(ci_rbmw_visitor*);
  void (*escape)(ci_rbmw_visitor*, unsigned flags);
  void (*data)(ci_rbmw_visitor*, unsigned offset,
	       unsigned word, unsigned flags);
  void (*checksum)(ci_rbmw_visitor*, int okay, unsigned crc);
};



/*! Comment? */
extern int ci_rbmw_packet_parse(ci_rbmw_visitor* v,
				const void* packet, int packet_bytes);

  /*! This is not a protocol-version-2 dumper.  It is just an inferior
  ** version of ci_rbmw_packet_dump_v2() that uses the visitor above.
  ** There is no reason to use this function at the moment.
  */
extern int ci_rbmw_packet_dump_v2(void (*log_fn)(const char* msg),
				  const void* packet, int packet_bytes);



/**********************************************************************
 * RBMW receive simulator.
 */

/*! RBMW receive simulator */
typedef struct {
  ci_uint32*		base;
  unsigned		bytes;
  unsigned		seq;
  unsigned		disabled;
} ci_rbmw_simulator_ap;


/*! Comment? */
typedef struct {
  ci_rbmw_visitor	visitor;
  ci_rbmw_simulator_ap*	ap;
  int			n_ap;
  ci_rbmw_simulator_ap* current_ap;
  int			create_ap_on_demand;
  int			sob;
  ci_uint32*		burst;
  ci_uint32*		burst_p;
  ci_uint32*		burst_end;
  int			burst_offset_w;
  ci_uint32		sob_mask;
  ci_uint32		eob_mask;
  int			packet_i;
} ci_rbmw_simulator;


/*! Comment? */
extern int ci_rbmw_simulator_ctor(ci_rbmw_simulator*);
/*! Comment? */
extern void ci_rbmw_simulator_dtor(ci_rbmw_simulator*);

/*! Comment? */
extern int ci_rbmw_simulator_ap_alloc(ci_rbmw_simulator*, int ap_id,
				      int bytes, void* opt_buffer);

/*! Comment? */
extern int ci_rbmw_simulator_handle_packet(ci_rbmw_simulator*,
					   const void* packet,
					   int packet_bytes);


/**********************************************************************
 * Packet generator interface.
 */

/*! Comment? */
typedef struct {
  int		   version;
  ci_uint32*       buf;
  ci_uint32*       buf_start;
  ci_uint32*       buf_end;
  unsigned         x25_crc;
  unsigned         usb_crc;
  /* [x25_crc] field used for version 2 32-bit crc. */
} ci_rbmw_packet_generator;


/*! \i_pkt_gen */
extern void ci_rbmw_packet_generator_init(ci_rbmw_packet_generator* pg,
					  void* buf, int buf_bytes, int vers);

/*! \i_pkt_gen */
extern int ci_rbmw_packet_generate_header(ci_rbmw_packet_generator* pg,
					 const void* eth_dest,
					 const void* eth_src,
					 unsigned ci_src, unsigned ci_seq,
					 unsigned ci_ap);
  /*!< Returns number of bytes written, or -ve on error. */

/*! \i_pkt_gen */
extern int ci_rbmw_packet_generate_burst(ci_rbmw_packet_generator* pg,
					const void** src_data,
					int* src_data_len,
					unsigned offset_bytes);
  /*!< Returns number of bytes written. */

/*! \i_pkt_gen */
extern int ci_rbmw_packet_dump(void (*log_fn)(const char* msg),
			      const void* packet, int packet_bytes);
  /*!< Returns 0, or one of the following error codes if the packet is not
  ** well formed (may not be first error though):
  **
  **    -EMSGSIZE   packet too small
  **    -EDOM       wrong ether-type
  **    -ENOTSUP    unknown version
  **    -EPROTO     protocol error
  **    -EBADMSG    checksum error
  **    -EIO        packet ended mid-burst
  */


#endif  /* __CI_TOOLS_RBMW_H__ */

/*! \cidoxg_end */
