#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>

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
# pragma pack(pop)

struct s_packet {
  int     size;
  void*   data;
  struct s_packet* next;
};






ssize_t (*origin_write)(int fd, const void* buf, size_t count);

ssize_t write(int fd, const void* buf, size_t count) {
  // printf("Hooked!\n");
  origin_write = (ssize_t (*)(int, const void*, size_t))dlsym(RTLD_NEXT, "write");
  ssize_t rtn = (*origin_write)(fd, buf, count);
  return (rtn);
}

// gcc -fPIC -shared -o hook.so hooking.c -ldl
