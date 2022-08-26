#ifndef STRUCT_H_
# define STRUCT_H_

# define STAT_LOCATION "/Users/jiwon/proc/stat"
# define MEM_LOCATION "/Users/jiwon/proc/meminfo"
# define NET_LOCATION "/Users/jiwon/proc/net/dev"
# define PROC_LOCATION "/Users/jiwon/proc/"

# define MAX 10000
# define CMDLINE_MAX 512

# define SEND 0

enum type {
  STAT = 1,
  MEM,
  NET,
  PROCESS,
};

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
  unsigned int    pid;
  unsigned int    ppid;
  unsigned int    loginuid;
  long            cutime;
  long            cstime;
  unsigned long   utime;
  unsigned long   stime;
  char            comm[256];
  char            username[8];
  char            cmdline[CMDLINE_MAX];
};

struct s_header {
  int   type_of_body;
  int   number_of_body;
};

struct s_packet {
  int     size;
  void*   data;
  struct s_packet* next;
};
# pragma pack(pop)

#endif
