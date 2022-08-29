#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

#include "queue.h"

#define PORT 8080

void listening(int* server_fd, struct sockaddr_in* address) {
  if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    printf("socket()");
    exit(EXIT_FAILURE);
  }

  address->sin_family = AF_INET;
  address->sin_addr.s_addr = INADDR_ANY;
  address->sin_port = htons(PORT);
  memset(address->sin_zero, 0, sizeof(address->sin_zero));

  if (bind(*server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
    printf("bind()");
    exit(EXIT_FAILURE);
  }
  if (listen(*server_fd, 20) < 0) {
    printf("listen()");
    exit(EXIT_FAILURE);
  }
}

void* recv_packet(void* vparam) {
  struct s_thread_param* param = (struct s_thread_param*)vparam;
  int rd_size;
  char buf[100000] = { 0, };

  while ((rd_size = read(param->socket, buf, 100000)) > 0) {
    buf[rd_size] = 0;
    char* pbuf = buf;

    while (rd_size > 0) {
      struct s_packet* packet = malloc(sizeof(struct s_packet));
      struct s_header* header = (struct s_header*)pbuf;
      if (header->type_of_body == STAT) {
        packet->size = sizeof(struct s_header) + sizeof(struct s_stat) * header->number_of_body;
      } else if (header->type_of_body == MEM) {
        packet->size = sizeof(struct s_header) + sizeof(struct s_mem) * header->number_of_body;
      } else if (header->type_of_body == NET) {
        packet->size = sizeof(struct s_header) + sizeof(struct s_net) * header->number_of_body;
      } else if (header->type_of_body == PROCESS) {
        packet->size = sizeof(struct s_header) + sizeof(struct s_process) * header->number_of_body;
      }
      packet->data = malloc(packet->size);
      memcpy(packet->data, pbuf, packet->size);
      packet->next = NULL;
      rd_size -= packet->size;
      pbuf += packet->size;

      if (header->type_of_body == STAT) {
        printf("~~~ pop data is STAT!\n");
      } else if (header->type_of_body == MEM) {
        printf("~~~ pop data is MEM!\n");
      } else if (header->type_of_body == NET) {
        printf("~~~ pop data is NET!\n");
      } else if (header->type_of_body == PROCESS) {
        printf("~~~ pop data is PROCESS!\n");
      }
      insert_queue(&param->queue, packet);
    }
  }
  if (rd_size < 0)
    exit(EXIT_FAILURE);
  return (0);
}

int main(void) {
  int server_fd;
  struct sockaddr_in address;
  listening(&server_fd, &address);

  int addr_size = sizeof(address);
  struct s_thread_param data;
  pthread_t tid;

  while (1) {
    printf("[start listen...]\n");
    if ((data.socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addr_size)) < 0) {
      exit(EXIT_FAILURE);
    }
    pthread_create(&tid, NULL, recv_packet, &data);
    pthread_detach(tid);
  }

    /*
    {
    printf("stat : %d %d %d %d\n", packet.body.stat.user, packet.body.stat.sys, packet.body.stat.idle, packet.body.stat.iowait);
    printf("mem : %d %d %d %d\n", packet.body.mem.mem_total, packet.body.mem.mem_free, packet.body.mem.swap_total, packet.body.mem.swap_free);
    for (int i = 0; i < packet.header.net_size; i++) {
      struct s_net* p = packet.body.net + sizeof(struct s_net) * i;
      printf("net %d : %s %d %d %d %d\n", i, p->interface, p->receive_bytes, p->receive_packets, p->transmit_bytes, p->transmit_packets);
    }
    for (int i = 0; i < packet.header.process_size; i++) {
      struct s_process* p = packet.body.process + sizeof(struct s_process) * i;
      printf("process %d : %s %d %lu %lu %ld %ld %s\n", p->pid, p->comm, p->ppid, p->utime, p->stime, p->cutime, p->cstime, p->username);
      printf("process %d : %s %s\n", p->pid, p->cmdline[0], p->cmdline[1]);
    }
    printf("-------------------------------------\n");
    }
    */

  return (0);
}
