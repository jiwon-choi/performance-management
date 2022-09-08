#include "../../agent/incs/packet.h"
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

  fprintf(fp, "%s | %-8s | ", header->time, header->agent_name);
  fprintf(fp, "user %-10d | sys %-10d | idle %-10d | iowait %-10d",
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

  fprintf(fp, "%s | %-8s | ", header->time, header->agent_name);
  fprintf(fp, "memtotal %-10d | memfree %-10d | swaptotal %-10d | swapfree %-10d",
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
    fprintf(fp, "%s | %-8s | ", header->time, header->agent_name);
    fprintf(fp, "interface %-15s | Rbytes %-10d | Rpackets %-10d | Tbytes %-10d | Tpackets %-10d",
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
    fprintf(fp, "%s | %-8s | ", header->time, header->agent_name);
    fprintf(fp, "pid %-6u | ppid %-6u | loginuid %-6u | ", chunk->pid, chunk->ppid, chunk->loginuid);
    // fprintf(fp, "cutime %10ld | cstime %10ld | utime %10lu | stime %10lu\n", chunk->cutime, chunk->cstime, chunk->utime, chunk->stime);
    fprintf(fp, "username %-8s | comm %-10s | cmdline : %s", chunk->username, chunk->comm, chunk->cmdline);
    fprintf(fp, "\n");
  }
  fclose(fp);
}
