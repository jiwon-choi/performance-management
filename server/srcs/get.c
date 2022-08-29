#include "../../agent/incs/packet.h"
#include <stdio.h>

void get_stat(struct s_packet* packet) {
  FILE* fp = fopen("files/data", "a+");
  struct s_stat* body = packet->data + sizeof(struct s_header);

  fprintf(fp, "STAT | user %d | sys %d | idle %d | iowait %d\n",
          body->user, body->sys, body->idle, body->iowait);
  fclose(fp);
}

void get_mem(struct s_packet* packet) {
  FILE* fp = fopen("files/data", "a+");
  struct s_mem* body = packet->data + sizeof(struct s_header);

  fprintf(fp, "MEM | memtotal %d | memfree %d | swaptotal %d | swapfree %d\n",
          body->mem_total, body->mem_free, body->swap_total, body->swap_free);
  fclose(fp);
}

void get_net(struct s_packet* packet) {
  FILE* fp = fopen("files/data", "a+");
  struct s_header* header = packet->data;

  for (int idx = 0; idx < header->number_of_body; idx++) {
    struct s_net* chunk = packet->data + sizeof(struct s_header) + sizeof(struct s_net) * idx;
    fprintf(fp, "NET | interface %s | Rbytes %d | Rpackets %d | Tbytes %d | Tpackets %d\n",
            chunk->interface, chunk->receive_bytes, chunk->receive_packets, chunk->transmit_bytes, chunk->transmit_packets);
  }
  fclose(fp);
}

void get_process(struct s_packet* packet) {
  FILE* fp = fopen("files/data", "a+");
  struct s_header* header = packet->data;

  for (int idx = 0; idx < header->number_of_body; idx++) {
    struct s_process* chunk = packet->data + sizeof(struct s_header) + sizeof(struct s_process) * idx;
    fprintf(fp, "PROC | pid %u | ppid %u | loginuid %u | comm %s | username %s\n",
            chunk->pid, chunk->ppid, chunk->loginuid, chunk->comm, chunk->username);
    fprintf(fp, "     | cutime %ld | cstime %ld | utime %lu | stime %lu\n",
            chunk->cutime, chunk->cstime, chunk->utime, chunk->stime);
    fprintf(fp, "     | cmdline : %s\n", chunk->cmdline);
  }
  fclose(fp);
}