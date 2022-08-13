#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>

#define MAX 100000

struct stat {
  int   user;
  int   sys;
  int   idle;
  int   iowait;
};

struct net {
  char* interface;
  int   receive_bytes;
  int   receive_packets;
  int   transmit_bytes;
  int   transmit_packets;
};

struct mem {
  int   mem_total;
  int   mem_free;
  int   swap_total;
  int   swap_free;
};

struct process {
  int             pid;
  int             ppid;
  int             loginuid;
  long            cutime;
  long            cstime;
  unsigned long   utime;
  unsigned long   stime;
  char*           comm;
  char*           username;
  char*           cmdline[MAX];
};

void parse_process_loginuid(struct process* process, char* filename) {
  FILE* fp;
  struct passwd* pwd;
  char str[MAX];

  fp = fopen(filename, "r");
  process->loginuid = atoi(fgets(str, MAX, fp));
  pwd = getpwuid(process->loginuid);
  process->username = pwd->pw_name;
  fclose(fp);
}

void parse_process_cmdline(struct process* process, char* filename) {
  FILE* fp;
  size_t  size = 1;
  char* str = malloc(sizeof(char) * size);

  fp = fopen(filename, "r");
  int i = 0;
  while (getdelim(&str, &size, 0, fp) != EOF) {
    process->cmdline[i++] = strdup(str);
    memset(str, 0, strlen(str));
  }
  free(str);
  fclose(fp);
}

void parse_process_stat(struct process* process, char* filename) {
  FILE* fp;
  char str[MAX];

  fp = fopen(filename, "r");
  fgets(str, MAX, fp);

  char* word = strtok(str, " ");
  for (int idx = 2; idx < 18; idx++) {
    word = strtok(NULL, "( )");
    if (idx == 2) {
      process->comm = strdup(word);
    } else if (idx == 4) {
      process->ppid = atoi(word);
    } else if (idx == 14) {
      process->utime = strtoul(word, NULL, 10);
    } else if (idx == 15) {
      process->stime = strtoul(word, NULL, 10);
    } else if (idx == 16) {
      process->cutime = strtoul(word, NULL, 10);
    } else if (idx == 17) {
      process->cstime = strtoul(word, NULL, 10);
    }
  }
  fclose(fp);
}

void parse_process() {
  DIR* proc;
  struct dirent* ent;

  proc = opendir("/Users/jiwon/proc/");
  while ((ent = readdir(proc)) != NULL) {
    struct process process;
    if (ent->d_type == DT_DIR && (process.pid = atoi(ent->d_name)) > 0) {
      char filename[MAX];
      sprintf(filename, "/Users/jiwon/proc/%s/stat", ent->d_name);
      parse_process_stat(&process, filename);
      sprintf(filename, "/Users/jiwon/proc/%s/cmdline", ent->d_name);
      parse_process_cmdline(&process, filename);
      sprintf(filename, "/Users/jiwon/proc/%s/loginuid", ent->d_name);
      parse_process_loginuid(&process, filename);
      printf("process %d : %s %d %lu %lu %ld %ld %s\n", process.pid, process.comm, process.ppid, process.utime, process.stime, process.cutime, process.cstime, process.username);
      printf("process %d : %s %s\n", process.pid, process.cmdline[0], process.cmdline[1]);
    }
    // TODO free string
  }
}

void parse_mem() {
  FILE* fp;
  char str[MAX];

  fp = fopen("/Users/jiwon/proc/meminfo", "r");
  struct mem mem;
  while (fgets(str, MAX, fp)) {
    char* word = strtok(str, " :");
    if (strcmp(word, "MemTotal") == 0) {
      mem.mem_total = atoi(strtok(NULL, " "));
    } else if (strcmp(word, "MemFree") == 0) {
      mem.mem_free = atoi(strtok(NULL, " "));
    } else if (strcmp(word, "SwapTotal") == 0) {
      mem.swap_total = atoi(strtok(NULL, " "));
    } else if (strcmp(word, "SwapFree") == 0) {
      mem.swap_free = atoi(strtok(NULL, " "));
    }
  }
  fclose(fp);
  printf("mem : %d %d %d %d\n", mem.mem_total, mem.mem_free, mem.swap_total, mem.swap_free);
}

void parse_net() {
  FILE* fp;
  char str[MAX];
  
  fp = fopen("/Users/jiwon/proc/net/dev", "r");
  while (fgets(str, MAX, fp)) {
    if (!strchr(str, ':'))
      continue;

    char* word = strtok(str, " :");
    if (strcmp(word, "lo") == 0) continue;

    struct net net;
    net.interface = strdup(word);
    word = strtok(NULL, " ");
    net.receive_bytes = atoi(word);
    word = strtok(NULL, " ");
    net.receive_packets = atoi(word);
    for (int i = 0; i < 7; i++)
      word = strtok(NULL, " ");
    net.transmit_bytes = atoi(word);
    word = strtok(NULL, " ");
    net.transmit_packets = atoi(word);

    printf("net : %s %d %d %d %d\n", net.interface, net.receive_bytes, net.receive_packets, net.transmit_bytes, net.transmit_packets);
    free(net.interface);
  }
  fclose(fp);
}

void parse_stat() {
  FILE* fp;
  char str[MAX];

  fp = fopen("/Users/jiwon/proc/stat", "r");
  fgets(str, MAX, fp);

  struct stat stat;
  char* word = strtok(str, " ");
  word = strtok(NULL, " ");
  stat.user = atoi(word);
  word = strtok(NULL, " ");
  word = strtok(NULL, " ");
  stat.sys = atoi(word);
  word = strtok(NULL, " ");
  stat.idle = atoi(word);
  word = strtok(NULL, " ");
  stat.iowait = atoi(word);
  fclose(fp);

  printf("stat : %d %d %d %d\n", stat.user, stat.sys, stat.idle, stat.iowait);
}

int main(int argc, char* argv[]) {
  parse_stat();
  parse_net();
  parse_mem();
  parse_process();
  return (0);
}
