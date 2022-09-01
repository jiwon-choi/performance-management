#include "../../agent/incs/packet.h"
#include <stdio.h>
#include <sys/stat.h>

void get_stat(struct s_packet* packet) {
  FILE* fp = fopen("data/stat", "a+");
  if (!fp) {
    mkdir("data/", 0755);
    fp = fopen("data/stat", "a+");
  }

  struct s_header* header = packet->data;
  struct s_stat* body = packet->data + sizeof(struct s_header);

  fprintf(fp, "%4d-%02d-%02d %02d:%02d:%02d | ",
          header->time.tm_year, header->time.tm_mon, header->time.tm_mday, header->time.tm_hour, header->time.tm_min, header->time.tm_sec);
  fprintf(fp, "user %d | sys %d | idle %d | iowait %d",
          body->user, body->sys, body->idle, body->iowait);
  fprintf(fp, "\n");
  fclose(fp);
}

void get_mem(struct s_packet* packet) {
  FILE* fp = fopen("data/mem", "a+");
  if (!fp) {
    mkdir("data/", 0755);
    fp = fopen("data/mem", "a+");
  }

  struct s_header* header = packet->data;
  struct s_mem* body = packet->data + sizeof(struct s_header);

  fprintf(fp, "%4d-%02d-%02d %02d:%02d:%02d | ",
          header->time.tm_year, header->time.tm_mon, header->time.tm_mday, header->time.tm_hour, header->time.tm_min, header->time.tm_sec);
  fprintf(fp, "memtotal %d | memfree %d | swaptotal %d | swapfree %d",
          body->mem_total, body->mem_free, body->swap_total, body->swap_free);
  fprintf(fp, "\n");
  fclose(fp);
}

void get_net(struct s_packet* packet) {
  FILE* fp = fopen("data/net", "a+");
  if (!fp) {
    mkdir("data/", 0755);
    fp = fopen("data/net", "a+");
  }

  struct s_header* header = packet->data;

  for (int idx = 0; idx < header->number_of_body; idx++) {
    struct s_net* chunk = packet->data + sizeof(struct s_header) + sizeof(struct s_net) * idx;
    fprintf(fp, "%4d-%02d-%02d %02d:%02d:%02d | ",
            header->time.tm_year, header->time.tm_mon, header->time.tm_mday, header->time.tm_hour, header->time.tm_min, header->time.tm_sec);
    fprintf(fp, "interface %s | Rbytes %d | Rpackets %d | Tbytes %d | Tpackets %d",
            chunk->interface, chunk->receive_bytes, chunk->receive_packets, chunk->transmit_bytes, chunk->transmit_packets);
    fprintf(fp, "\n");
  }
  fclose(fp);
}

void get_process(struct s_packet* packet) {
  FILE* fp = fopen("data/process", "a+");
  if (!fp) {
    mkdir("data/", 0755);
    fp = fopen("data/process", "a+");
  }
  struct s_header* header = packet->data;

  for (int idx = 0; idx < header->number_of_body; idx++) {
    struct s_process* chunk = packet->data + sizeof(struct s_header) + sizeof(struct s_process) * idx;
    fprintf(fp, "%4d-%02d-%02d %02d:%02d:%02d | ",
            header->time.tm_year, header->time.tm_mon, header->time.tm_mday, header->time.tm_hour, header->time.tm_min, header->time.tm_sec);
    fprintf(fp, "pid %u | ppid %u | loginuid %u | comm %s | username %s | ",
            chunk->pid, chunk->ppid, chunk->loginuid, chunk->comm, chunk->username);
    fprintf(fp, "cutime %ld | cstime %ld | utime %lu | stime %lu | ",
            chunk->cutime, chunk->cstime, chunk->utime, chunk->stime);
    fprintf(fp, "cmdline : %s", chunk->cmdline);
    fprintf(fp, "\n");
  }
  fclose(fp);
}