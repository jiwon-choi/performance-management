#include "../../agent/incs/packet.h"
#include <stdio.h>

void get_stat(struct s_packet* packet) {
  struct s_stat* body = packet->data + sizeof(struct s_header);

  printf("STAT | user %d | sys %d | idle %d | iowait %d\n",
          body->user, body->sys, body->idle, body->iowait);
}

void get_mem(struct s_packet* packet) {
  struct s_mem* body = packet->data + sizeof(struct s_header);

  printf("MEM | memtotal %d | memfree %d | swaptotal %d | swapfree %d\n",
          body->mem_total, body->mem_free, body->swap_total, body->swap_free);
}

void get_net(struct s_packet* packet) {
  struct s_header* header = packet->data;

  for (int idx = 0; idx < header->number_of_body; idx++) {
    struct s_net* chunk = packet->data + sizeof(struct s_header) + sizeof(struct s_net) * idx;
    printf("NET | interface %s | Rbytes %d | Rpackets %d | Tbytes %d | Tpackets %d\n",
            chunk->interface, chunk->receive_bytes, chunk->receive_packets, chunk->transmit_bytes, chunk->transmit_packets);
  }
}

void get_process(struct s_packet* packet) {
  struct s_header* header = packet->data;

  for (int idx = 0; idx < header->number_of_body; idx++) {
    struct s_process* chunk = packet->data + sizeof(struct s_header) + sizeof(struct s_process) * idx;
    printf("PROC | pid %u | ppid %u | loginuid %u | comm %s | username %s\n",
            chunk->pid, chunk->ppid, chunk->loginuid, chunk->comm, chunk->username);
    printf("     | cutime %ld | cstime %ld | utime %lu | stime %lu\n",
            chunk->cutime, chunk->cstime, chunk->utime, chunk->stime);
    printf("     | cmdline : %s\n", chunk->cmdline);
  }
}