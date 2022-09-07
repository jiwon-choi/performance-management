#include "../../agent/incs/struct.h"
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

void save_stat(struct s_packet* packet) {
  FILE* fp = fopen("files/data/stat", "a+");
  if (!fp) {
    mkdir("files/data", 0777);
    fp = fopen("files/data/stat", "a+");
  }

  struct s_header* header = packet->data;
  struct s_stat* body = packet->data + sizeof(struct s_header);

  fprintf(fp, "[%s] ", header->time);
  fprintf(fp, "user %d | sys %d | idle %d | iowait %d",
          body->user, body->sys, body->idle, body->iowait);
  fprintf(fp, "\n");
  fclose(fp);
}

void save_mem(struct s_packet* packet) {
  FILE* fp = fopen("files/data/mem", "a+");
  if (!fp) {
    mkdir("files/data", 0777);
    fp = fopen("files/data/mem", "a+");
  }

  struct s_header* header = packet->data;
  struct s_mem* body = packet->data + sizeof(struct s_header);

  fprintf(fp, "[%s] ", header->time);
  fprintf(fp, "memtotal %d | memfree %d | swaptotal %d | swapfree %d",
          body->mem_total, body->mem_free, body->swap_total, body->swap_free);
  fprintf(fp, "\n");
  fclose(fp);
}

void save_net(struct s_packet* packet) {
  FILE* fp = fopen("files/data/net", "a+");
  if (!fp) {
    mkdir("files/data", 0777);
    fp = fopen("files/data/net", "a+");
  }

  struct s_header* header = packet->data;

  for (int idx = 0; idx < header->number_of_body; idx++) {
    struct s_net* chunk = packet->data + sizeof(struct s_header) + sizeof(struct s_net) * idx;
    fprintf(fp, "[%s] ", header->time);
    fprintf(fp, "interface %s | Rbytes %d | Rpackets %d | Tbytes %d | Tpackets %d",
            chunk->interface, chunk->receive_bytes, chunk->receive_packets, chunk->transmit_bytes, chunk->transmit_packets);
    fprintf(fp, "\n");
  }
  fclose(fp);
}

void save_process(struct s_packet* packet) {
  FILE* fp = fopen("files/data/process", "a+");
  if (!fp) {
    mkdir("files/data", 0777);
    fp = fopen("files/data/process", "a+");
  }
  struct s_header* header = packet->data;

  for (int idx = 0; idx < header->number_of_body; idx++) {
    struct s_process* chunk = packet->data + sizeof(struct s_header) + sizeof(struct s_process) * idx;
    fprintf(fp, "[%s] ", header->time);
    fprintf(fp, "pid %u | ppid %u | loginuid %u | comm %s | username %s | ",
            chunk->pid, chunk->ppid, chunk->loginuid, chunk->comm, chunk->username);
    fprintf(fp, "cutime %ld | cstime %ld | utime %lu | stime %lu | ",
            chunk->cutime, chunk->cstime, chunk->utime, chunk->stime);
    fprintf(fp, "cmdline : %s", chunk->cmdline);
    fprintf(fp, "\n");
  }
  fclose(fp);
}
