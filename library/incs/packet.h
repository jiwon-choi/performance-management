#ifndef PACKET_H_
# define PACKET_H_

# include <time.h>

# define STAT_LOCATION "/proc/stat"
# define MEM_LOCATION "/proc/meminfo"
# define NET_LOCATION "/proc/net/dev"
# define PROC_LOCATION "/proc/"

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
    char    agent_name[9];
    char    time[25];
    int     type_of_body;
    int     number_of_body;
  };

  struct s_udp_begin {
    char          agent_name[9];
    unsigned int  pid;
    char          peer_ip[15];
    int           port;
    time_t        begin_time;
    int           pkt_no;
  };

  struct s_udp_end {
    char          agent_name[9];
    unsigned int  pid;
    int           send_byte;
    time_t        end_time;
    time_t        elapse_time;
  };
# pragma pack(pop)

struct s_packet {
  int     size;
  void*   data;
  struct s_packet* next;
};

#endif
