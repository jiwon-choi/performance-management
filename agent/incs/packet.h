#ifndef STRUCT_H_
# define STRUCT_H_

# define MAX 10000

# pragma pack(push, 1)

struct s_stat {
  int   user;
  int   sys;
  int   idle;
  int   iowait;
};

struct s_mem {
  int   mem_total;
  int   mem_free;
  int   swap_total;
  int   swap_free;
};

struct s_net {
  char  interface[16];
  int   receive_bytes;
  int   receive_packets;
  int   transmit_bytes;
  int   transmit_packets;
};

struct s_process {
  int             pid;
  int             ppid;
  int             loginuid;
  long            cutime;
  long            cstime;
  unsigned long   utime;
  unsigned long   stime;
  char            comm[256];
  char            username[8];
  char            cmdline[MAX]; // TODO null character -> space 변환해서 통째로!
};

struct s_header {
  int     net_size;
  int     process_size;
};

struct s_body {
  struct s_stat   stat;
  struct s_mem    mem;
  void*           net;
  void*           process;
};

struct s_packet {
  struct s_header header;
  struct s_body   body;
};

# pragma pack(pop)

#endif
