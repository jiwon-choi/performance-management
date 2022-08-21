#ifndef STRUCT_H_
# define STRUCT_H_

# define MAX 10000

# pragma pack(push, 1)

struct stat {
  int   user;
  int   sys;
  int   idle;
  int   iowait;
};

struct mem {
  int   mem_total;
  int   mem_free;
  int   swap_total;
  int   swap_free;
};

struct net {
  char          interface[16];
  int           receive_bytes;
  int           receive_packets;
  int           transmit_bytes;
  int           transmit_packets;
  struct net*   next;
};

struct process {
  int               pid;
  int               ppid;
  int               loginuid;
  long              cutime;
  long              cstime;
  unsigned long     utime;
  unsigned long     stime;
  char              comm[256];
  char              username[8];
  char              cmdline[4096][128];
  struct process*   next;
};

struct data {
  struct stat     stat;
  struct mem      mem;
  struct net*     net;
  struct process* process;
};

# pragma pack(pop)

#endif
