#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "parse.h"

#define PORT 8080

int connection() {
  int sock = 0;
  struct sockaddr_in serv_addr;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    exit(EXIT_FAILURE);
  }

  memset(&serv_addr, '\0', sizeof(serv_addr)); // 0!!! 이라니!!!
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    printf("\nInvalid address/ Address not supported \n");
    exit(EXIT_FAILURE);
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("Connection Failed\n");
    exit(EXIT_FAILURE);
  }
  return (sock);
}

void* print_queue(void* queue) {
  while (1) {
    struct s_packet* p = *(struct s_packet**)queue;
    if (!p) {
      printf("~~~~ queue is empty\n");
      sleep(3);
      continue;
    }

    int idx = 0;
    while (p) {
      struct s_header* header = p->data;
      if (header->type_of_body == STAT) {
        printf("~~~ print queue %d STAT!\n", idx);
      } else if (header->type_of_body == MEM) {
        printf("~~~ print queue %d MEM!\n", idx);
      } else if (header->type_of_body == NET) {
        printf("~~~ print queue %d NET!\n", idx);
      } else if (header->type_of_body == PROCESS) {
        printf("~~~ print queue %d PROCESS!\n", idx);
      }
      p = p->next;
      idx++;
    }
    printf("\n");
    sleep(3);
  }
  return (0);
}

int main() {
  struct s_packet* queue = NULL;
  pthread_t tid[5];

  // int sock = connection();

  pthread_create(&tid[STAT], NULL, parse_stat, &queue);
  pthread_create(&tid[MEM], NULL, parse_mem, &queue);
  pthread_create(&tid[NET], NULL, parse_net, &queue);
  pthread_create(&tid[PROCESS], NULL, parse_process, &queue);
  pthread_create(&tid[SEND], NULL, print_queue, &queue);

  pthread_join(tid[STAT], NULL);
  pthread_join(tid[MEM], NULL);
  pthread_join(tid[NET], NULL);
  pthread_join(tid[PROCESS], NULL);
  pthread_join(tid[SEND], NULL);

  /*
  {
  // printf("header : net %d, process %d \n", packet.header.net_size, packet.header.process_size);
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
