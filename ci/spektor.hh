/**************************************************************************\
*//*! \file
** <L5_PRIVATE L5_SOURCE>
** \author  David Riddoch
**  \brief  Packet interceptor, forwarder, munger.
**   \date  2005/02/12
**    \cop  (c) Level 5 Networks Limited.
** </L5_PRIVATE>
*//*
\**************************************************************************/

/* TODO:
**
**  - circular capture buffer
**  - triggers (w programmable position in buffer)
**  - interface
**  - program language
*/

/*! \cidoxg_tools_misc */

extern "C" {
#define  __STDC_FORMAT_MACROS
#include <ci/tools.h>
#include <ci/net/ethernet.h>
#include <ci/net/ipv4.h>
}

#include <string>
#include <vector>
#include <functional>
#include <deque>


class Nic;
class Spektor;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class Timestamp {
public:
  typedef ci_uint64	T;

private:
  static T		s_start_of_day;
  static unsigned	s_cycles_per_usec;

  static Timestamp	instance;
  Timestamp();

public:
  static inline T start_of_day() { return s_start_of_day; }

  static inline void stamp(T* pts) { ci_frc64(pts); }

  static inline unsigned to_usec(T ts)
  { return (unsigned) (ts / s_cycles_per_usec); }

  static inline T from_usec(unsigned usec)
  { return (T) usec * (T) s_cycles_per_usec; }

  static inline T from_nsec(unsigned nsec)
  { return (T) nsec * (T) s_cycles_per_usec / 1000; }

  static inline void to_timeval(T ts, struct timeval* tv) {
    ts -= s_start_of_day;
    ts /= s_cycles_per_usec;
    tv->tv_sec = ts / 1000000;
    tv->tv_usec = ts % 1000000;
  }

};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

typedef struct {
  Timestamp::T	timestamp;
  ci_uint16	len;
  ci_uint16	port;
  ci_uint16	pad;
  ci_octet      rx_prefix[16];
  ci_octet	payload[1];
} pkt_payload_t;


typedef struct {
  ci_dllink	link;
  unsigned	id;
  int		refs;
  Nic*		nic;
  int		fwd;
  pkt_payload_t p;
} pkt_t;


inline void pkt_hold(pkt_t* pkt) { ++pkt->refs; }

#define lnk2pkt(l)	CI_CONTAINER(pkt_t, link, (l))

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

inline ci_ether_hdr* ether_hdr(pkt_t* pkt)
{ return (ci_ether_hdr*) pkt->p.payload; }

inline char* ether_payload(pkt_t* pkt)
{ return (char*) (ether_hdr(pkt) + 1); }


inline ci_ip4_hdr* ip_hdr(pkt_t* pkt)
{ return (ci_ip4_hdr*) ether_payload(pkt); }

inline char* ip_payload(pkt_t* pkt) {
  ci_ip4_hdr* ip = ip_hdr(pkt);
  return (char*) ip + CI_IP4_IHL(ip);
}


inline ci_tcp_hdr* tcp_hdr(pkt_t* pkt)
{ return (ci_tcp_hdr*) ip_payload(pkt); }

inline char* tcp_payload(pkt_t* pkt) {
  ci_tcp_hdr* tcp = tcp_hdr(pkt);
  return CI_TCP_PAYLOAD(tcp);
}

inline unsigned tcp_paylen(pkt_t* pkt) {
  ci_ip4_hdr* ip = ip_hdr(pkt);
  ci_tcp_hdr* tcp = tcp_hdr(pkt);
  return CI_TCP_PAYLEN(ip, tcp);
}


inline ci_udp_hdr* udp_hdr(pkt_t* pkt)
{ return (ci_udp_hdr*) ip_payload(pkt); }

inline char* udp_payload(pkt_t* pkt)
{ return (char*) (udp_hdr(pkt) + 1); }


inline int is_arp(pkt_t* pkt)
{ return ether_hdr(pkt)->ether_type == CI_ETHERTYPE_ARP; }

inline int is_ip(pkt_t* pkt)
{ return ether_hdr(pkt)->ether_type == CI_ETHERTYPE_IP; }

inline int is_tcp(pkt_t* pkt)
{ return is_ip(pkt) && ip_hdr(pkt)->ip_protocol == IPPROTO_TCP; }

inline int is_udp(pkt_t* pkt)
{ return is_ip(pkt) && ip_hdr(pkt)->ip_protocol == IPPROTO_UDP; }


inline int is_tcp_and_has_flags(pkt_t* pkt, unsigned flags)
{ return is_tcp(pkt) && (tcp_hdr(pkt)->tcp_flags & flags) == flags; }


extern void tcp_change_payload(ci_ip4_hdr*, ci_tcp_hdr*,
			       const void* new_payload, unsigned len);
extern ci_octet* tcp_find_option(ci_tcp_hdr* tcp, ci_octet option_type);
extern void tcp_sack_shrink(ci_ip4_hdr* ip, ci_tcp_hdr* tcp);
extern void tcp_sack_embelish(ci_ip4_hdr* ip, ci_tcp_hdr* tcp);
extern void tcp_sack_trash(ci_ip4_hdr* ip, ci_tcp_hdr* tcp);


extern void udp_change_payload(ci_ip4_hdr*, ci_udp_hdr*,
			       const void* new_payload, unsigned len);


extern void icmp_dest_unreach(pkt_t* reply, pkt_t* pkt, int code);


inline std::string int_to_string(int i, int f_width=1) {
  char buf[20];
  sprintf(buf, "%0*d", f_width, i);
  return std::string(buf);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

struct pkt_to_const_int : public std::unary_function<pkt_t*, int> {
  int	m_i;
  inline pkt_to_const_int(int i) : m_i(i) {}
  int operator()(pkt_t*) const { return m_i; }
};


struct pkt_to_rand_int : public std::unary_function<pkt_t*, int> {
  int	m_min, m_max;
  inline pkt_to_rand_int(int min, int max) : m_min(min), m_max(max) {}
  int operator()(pkt_t*) const
  { return m_min + rand() % (m_max - m_min + 1); }
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class PktLogger {
public:
  virtual ~PktLogger();
  virtual std::string describe() = 0;
  virtual void log(pkt_t*) = 0;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class PktDumper {
public:
  virtual ~PktDumper();
  virtual std::string describe() = 0;
  virtual bool can_dump() = 0;
  virtual void dump(pkt_payload_t* p, int caplen) = 0;
  virtual void close() = 0;
};


class PktFileDumper : public PktDumper {
public:
  virtual ~PktFileDumper();
  virtual void open(const char* fname) = 0;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class PktBuffer : public PktLogger {
  PktDumper*	m_dumper;
  ci_bufrange	m_buf;
  unsigned	m_rec_size;
  bool		m_wrapped;
  char*		m_full_ptr;
  int		m_trigger_pc;

public:
  PktBuffer(PktDumper* dumper, unsigned buf_size, unsigned hdr_bytes = 0);

  inline PktDumper* dumper() { return m_dumper; }
  inline void circular()     { m_full_ptr = 0;  }
  int  default_capture_bytes();
  void dump_and_reset();
  void full();
  void mark(const char* str);
  void trigger();

  virtual std::string describe();
  virtual void log(pkt_t* pkt);
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class PktHandler {
  int		m_id;
  PktHandler**	m_replace_ptr;
  PktHandler*	m_replacement;
  PktHandler*	m_next;
  int		m_loops;		// for breaking loops in recursions

  friend class Program;

  static int	s_id_gen;

protected:
  typedef std::deque<PktHandler*> queue;
  static void __program_dump(PktHandler* prog, unsigned indent, queue& subs);

public:
  inline PktHandler()
    : m_id(s_id_gen++), m_replace_ptr(0), m_replacement(0),
      m_next(0), m_loops(0)
  {}
  virtual ~PktHandler();

  virtual std::string do_describe() = 0;
  virtual PktHandler* do_handle(pkt_t*) = 0;
  virtual void get_subs(queue& subs);

  inline PktHandler* next()                   { return m_next; }

  inline void set_replace_ptr(PktHandler** r) { m_replace_ptr = r; }
  inline void set_replacement(PktHandler* h)  { m_replacement = h; }
  inline PktHandler* replacement()            { return m_replacement; }

  inline std::string id_str() const
  { return int_to_string(m_id, 2); }

  inline std::string describe()
  { return id_str() + ":" + do_describe(); }

  void replace_me();

  inline PktHandler* replace_and_handle(pkt_t* pkt) {
    replace_me();
    return m_replacement;
  }

  static void chain_run(PktHandler* h, pkt_t* pkt);

  static void chain_clear_loop_flag(PktHandler* h);
  static void program_clear_loop_flag(PktHandler* h);

  static void chain_dump(PktHandler* h, unsigned indent=0);
  static void program_dump(PktHandler* prog, unsigned indent=0);

};


inline void anchor_ph(PktHandler** ph, PktHandler* h)
{ *ph = h;  if( h )  h->set_replace_ptr(ph); }

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class Nic {
protected:
  unsigned  m_n_ports;
public:
  inline unsigned n_ports() const { return m_n_ports; }
  virtual ~Nic() {}
  virtual void tx(pkt_t* pkt) = 0;
  virtual void pkt_release(pkt_t* pkt) = 0;
  virtual int  poll(Spektor*) = 0;
  //  virtual void wait() = 0;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class Task {
public:
  virtual ~Task();
  virtual void timeout() = 0;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class Spektor {

  class TaskEntry {
  public:
    inline TaskEntry(Task* t, Timestamp::T when)
      : m_timestamp(when), m_task(t) {}

    int operator<(const TaskEntry& a) const
    { return m_timestamp < a.m_timestamp; }

    Timestamp::T  m_timestamp;
    Task*	  m_task;
  };

  int		m_max_ports;
  int		m_n_ports;
  PktHandler**	m_handlers;	// rx handlers, indexed by port
  Nic**		m_nics;
  int		m_n_nics;
  Timestamp::T	m_timestamp;
  bool		m_sleep;
  std::string	m_capture_filename;

  // Stuff to do in future.  Managed as a heap.
  std::vector<TaskEntry> m_tasks;

public:
  Spektor(int max_ports);

  static Spektor* instance;

  inline std::string& capture_filename() { return m_capture_filename; }
  inline void set_capture_filename(const char* f) { m_capture_filename = f; }

  inline void register_nic(Nic* nic)
  { m_nics[m_n_nics++] = nic; }

  inline void set_pkt_handler(unsigned port, PktHandler* h) {
    ci_assert(port < (unsigned) m_max_ports);
    anchor_ph(&m_handlers[port], h);
    m_n_ports = CI_MAX(m_n_ports, (int) port + 1);
  }

  inline void forward(pkt_t* pkt) {
    // ?? todo: more general forwarding and multiple nics
    pkt->p.port = ! pkt->p.port;
    m_nics[0]->tx(pkt);
  }
  inline void reflect(pkt_t* pkt)
  { m_nics[0]->tx(pkt); }

  void handle(pkt_t* pkt);
  void event_loop();
  void dump_handlers();

  inline void          timestamp() { Timestamp::stamp(&m_timestamp); }
  inline Timestamp::T& now()       { return m_timestamp; }

  inline bool       is_task()   { return ! m_tasks.empty(); }
  inline TaskEntry& task_head() { return m_tasks[0]; }

  void task_schedule(Task* t, unsigned when_usec_rel);
  void task_dequeue();

};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class TkEcho : public Task {
  std::string m_str;
  virtual void timeout();
public:
  inline TkEcho(const char* msg) : m_str(msg) {}
};


class TkSetHandler : public Task {
  unsigned	m_port;
  PktHandler*	m_handler;
  virtual void timeout();
public:
  inline TkSetHandler(unsigned port, PktHandler* h)
    : m_port(port), m_handler(h) {}
};


class TkForward : public Task {
  pkt_t*	m_pkt;
  virtual void timeout();
public:
  inline TkForward(pkt_t* pkt) : m_pkt(pkt)  { pkt_hold(pkt); }
};


class TkRunProgram : public Task {
  PktHandler*	m_prog;
  pkt_t*	m_pkt;
  virtual void timeout();
public:
  inline TkRunProgram(PktHandler* prog, pkt_t* pkt)
    : m_prog(prog), m_pkt(pkt)  { pkt_hold(pkt); }
};


class TkSplice : public Task {
  PktHandler**	m_where;
  PktHandler*	m_handler;
  virtual void timeout();
public:
  inline TkSplice(PktHandler** where, PktHandler* h)
    : m_where(where), m_handler(h) {}
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class PhStop : public PktHandler {
public:
  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};


class PhOnceAndCont : public PktHandler {
public:
  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};


class PhForNPkts : public PktHandler {
  int	m_n;
  int	m_i;
public:
  inline PhForNPkts(int n) : m_n(n), m_i(0) { ci_assert(n > 0); }

  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};


class PhOnce : public PhForNPkts {
public:
  inline PhOnce() : PhForNPkts(1) {}
};


class PhForRandPkts : public PktHandler {
  int	m_min, m_max;
  int	m_n, m_i;
public:
  inline PhForRandPkts(int min, int max) : m_min(min), m_max(max)
  { reset(); }

  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);

  inline void reset() {
    m_n = m_min + rand() % (m_max - m_min + 1);
    m_i = 0;
  }
};


class PhPassBytes : public PktHandler {
  ci_uint64	m_n;
public:
  inline PhPassBytes(ci_uint64 n) : m_n(n) { ci_assert(n > 0); }

  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};


class PhWhileIntvlLessThanUsec : public PktHandler {
  int		m_usec;	// -ve means not seen any packets yet
  Timestamp::T	m_last;
public:
  inline PhWhileIntvlLessThanUsec(int usec) : m_usec(-usec)
  { ci_assert(usec > 0); }

  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};


class PhForUsec : public PktHandler {
  int		m_usec;	// -ve means not seen any packets yet
  Timestamp::T	m_start;
public:
  inline PhForUsec(int usec) : m_usec(-usec) { ci_assert(usec > 0); }

  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};


class PhDrop : public PktHandler {
  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};


class PhNoop : public PktHandler {
  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};


class PhEcho : public PktHandler {
  std::string	m_str;
public:
  inline PhEcho(const char* str) : m_str(strdup(str)) {}

  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};


class PhLog : public PktHandler {
  PktLogger*	m_logger;
public:
  inline PhLog(PktLogger* l) : m_logger(l) {}

  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};


class PhCapture : public PhLog {
  static PktBuffer* s_buffer;
public:
  PhCapture();
  virtual std::string do_describe();
  static inline PktBuffer* buffer() { return s_buffer; }
  static inline void set_buffer(PktBuffer* b) { s_buffer = b; }
};


class PhMark : public PktHandler {
  std::string	m_str;
public:
  PhMark(const char* str);

  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};


class PhSetFwdPortHandler : public PktHandler {
  PktHandler*	m_chain;
public:
  inline PhSetFwdPortHandler(PktHandler* chain) : m_chain(chain) {}

  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
  virtual void get_subs(queue& subs);
};


class PhSetThisPortHandler : public PktHandler {
  PktHandler*	m_chain;
public:
  inline PhSetThisPortHandler(PktHandler* chain)
  { anchor_ph(&m_chain, chain); }

  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
  virtual void get_subs(queue& subs);
};


class PhMTU : public PktHandler {
  unsigned	m_mtu;
public:
  inline PhMTU(unsigned mtu) : m_mtu(mtu) {}

  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};


class PhProtocol : public PktHandler {
  PktHandler *m_ip, *m_arp, *m_tcp, *m_udp, *m_icmp;
public:
  inline PhProtocol() : m_ip(0), m_arp(0), m_tcp(0), m_udp(0), m_icmp(0) {}

  inline void set_ip  (PktHandler* h) { anchor_ph(&m_ip,   h); }
  inline void set_arp (PktHandler* h) { anchor_ph(&m_arp,  h); }
  inline void set_tcp (PktHandler* h) { anchor_ph(&m_tcp,  h); }
  inline void set_udp (PktHandler* h) { anchor_ph(&m_udp,  h); }
  inline void set_icmp(PktHandler* h) { anchor_ph(&m_icmp, h); }

  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
  virtual void get_subs(queue& subs);
};


class PhThrottle : public PktHandler {
  ci_ptr_fifo	m_tasks;
  Timestamp::T	m_next_t;
  unsigned	m_mbps;
public:
  PhThrottle(unsigned megabit_per_sec, unsigned q_len);
  void task_done(void* t);
  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};


class PhSackShrink : public PktHandler {
  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};


class PhSackEmbelish : public PktHandler {
  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};


class PhSackTrash : public PktHandler {
  virtual std::string do_describe();
  virtual PktHandler* do_handle(pkt_t* pkt);
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

template<class GenUsec> class PhDelayProgram : public PktHandler {
  PktHandler*	m_prog;
  GenUsec	m_usec;
public:
  inline PhDelayProgram(PktHandler* prog, const GenUsec& usec)
    : m_prog(prog), m_usec(usec) {}

  virtual std::string do_describe() {
    std::string s("Forward");
    if( m_prog )  s = m_prog->describe();
    return std::string("PhDelayProgram(") + s + ")";
  }

  virtual PktHandler* do_handle(pkt_t* pkt) {
    Spektor::instance->task_schedule(new TkRunProgram(m_prog, pkt),
				     m_usec(pkt));
    pkt->fwd = 0;
    return next();
  }

  virtual void get_subs(queue& subs)
  { subs.push_back(m_prog); }
};


template <class GenUsec>
inline PhDelayProgram<GenUsec>*
mk_PhDelayProgram(PktHandler* prog, const GenUsec& usec)
{
  return new PhDelayProgram<GenUsec>(prog, usec);
}


inline PktHandler* mk_PhDelayProgram(PktHandler* prog, int usec)
{
  return mk_PhDelayProgram(prog, pkt_to_const_int(usec));
}


template <class GenUsec>
inline PhDelayProgram<GenUsec>* mk_PhDelay(const GenUsec& usec)
{
  return mk_PhDelayProgram(0, usec);
}


inline PktHandler* mk_PhDelay(int usec)
{
  return mk_PhDelayProgram(0, pkt_to_const_int(usec));
}


inline PktHandler* mk_PhRandDelayProgram(PktHandler* prog,
					 int min_usec, int max_usec)
{
  return mk_PhDelayProgram(prog, pkt_to_rand_int(min_usec, max_usec));
}


inline PktHandler* mk_PhRandDelay(int min_usec, int max_usec)
{
  return mk_PhDelayProgram(0, pkt_to_rand_int(min_usec, max_usec));
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

template<class Pred> class PhIf : public PktHandler {
  Pred		m_test;
  PktHandler*	m_true;
  PktHandler*	m_false;
public:
  inline PhIf(const Pred& test, PktHandler* iftrue, PktHandler* iffalse = 0)
    : m_test(test) {
    anchor_ph(&m_true, iftrue);
    anchor_ph(&m_false, iffalse);
  }

  virtual std::string do_describe() {
    std::string t("Forward");
    std::string f("Forward");
    if( m_true  )  t = m_true->id_str();
    if( m_false )  f = m_false->id_str();
    return std::string("PhIf(cond, ") + t + ", " + f + ")";
  }

  virtual PktHandler* do_handle(pkt_t* pkt) {
    if( m_test(pkt) )  chain_run(m_true, pkt);
    else               chain_run(m_false, pkt);
    return next();
  }

  virtual void get_subs(queue& subs) {
    subs.push_back(m_true);
    subs.push_back(m_false);
  }
};


template <class _Result>
inline PhIf<std::pointer_to_unary_function<pkt_t*, _Result> >*
mk_PhIf(_Result (*fn)(pkt_t* pkt), PktHandler* iftrue, PktHandler* iffalse = 0)
{
  return new PhIf<std::pointer_to_unary_function<pkt_t*, _Result> >
    (std::ptr_fun(fn), iftrue, iffalse);
}


template <class Pred>
inline PhIf<Pred>*
mk_PhIf(Pred pred, PktHandler* iftrue, PktHandler* iffalse = 0)
{
  return new PhIf<Pred>(pred, iftrue, iffalse);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

template<class Pred> class PhWhile : public PktHandler {
  Pred		m_test;
public:
  inline PhWhile(const Pred& test)
    : m_test(test) {}

  virtual std::string do_describe()
  { return std::string("PhWhile( ?? )"); }

  virtual PktHandler* do_handle(pkt_t* pkt) {
    if( ! m_test(pkt) )  return replace_and_handle(pkt);
    else                 return next();
  }
};


template <class _Result>
inline PhWhile<std::pointer_to_unary_function<pkt_t*, _Result> >*
mk_PhWhile(_Result (*fn)(pkt_t* pkt))
{
  return new PhWhile<std::pointer_to_unary_function<pkt_t*, _Result> >
    (std::ptr_fun(fn));
}


template <class Pred>
inline PhWhile<Pred>* mk_PhWhile(Pred pred)
{
  return new PhWhile<Pred>(pred);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

class Program {
  PktHandler*	m_head;
  PktHandler*	m_current;
  PktHandler*	m_tail;
public:
  inline Program() : m_head(0), m_current(0), m_tail(0) {}

  inline Program& First(PktHandler* h)
  { return Then(h); }

  inline Program& Then(PktHandler* h) {
    // No need to set_replace_ptr, as it'll propagate.
    if( m_current )  m_current->set_replacement(h);
    else             m_head = h;
    m_tail = m_current = h;
    return *this;
  }

  inline Program& And(PktHandler* h) {
    ci_assert(m_tail);
    ci_assert(! m_tail->m_next);
    anchor_ph(&m_tail->m_next, h);
    m_tail = h;
    return *this;
  }

  inline Program& BeforeNext(PktHandler* h)
  { return Then(new PhOnceAndCont).And(h); }

  inline void Loop()
  { Then(beginning()); }

  inline operator PktHandler*()  const { return m_head; }
  inline PktHandler* beginning() const { return m_head; }

  inline PktHandler* release() {
    PktHandler* ret = m_head;
    m_head = m_current = m_tail = 0;
    return ret;
  }
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

extern PktFileDumper* mk_pcap_dumper();
extern Nic* mk_efab_nic(int if_indexes[], int n_interfaces);
extern PktLogger* mk_logger();


/*! \cidoxg_end */
