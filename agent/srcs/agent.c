#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "parse.h"
#include "queue.h"

#define PORT 8080

int connection() {
  int sock = 0;
  struct sockaddr_in serv_addr;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    exit(EXIT_FAILURE);
  }

  memset(&serv_addr, '\0', sizeof(serv_addr));
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

void* send_packet(void* vparam) {
  struct s_thread_param* data = (struct s_thread_param*)vparam;

  while (1) {
    if (!data->queue) {
      sleep(1);
      continue;
    }

    struct s_packet* pop = pop_queue(&(data->queue));
    struct s_header* header = pop->data;
    if (header->type_of_body == STAT) {
      printf("~~~ pop data is STAT!\n");
    } else if (header->type_of_body == MEM) {
      printf("~~~ pop data is MEM!\n");
    } else if (header->type_of_body == NET) {
      printf("~~~ pop data is NET!\n");
    } else if (header->type_of_body == PROCESS) {
      printf("~~~ pop data is PROCESS!\n");
    }
    if (write(data->socket, pop->data, pop->size) < 0) {
      printf("server closed\n");
      exit(EXIT_FAILURE);
    }
  }
  return (0);
}

int main() {
  pthread_t tid[5];
  struct s_thread_param data;

  data.queue = NULL;
  data.socket = connection();

  pthread_create(&tid[STAT], NULL, parse_stat, &data.queue);
  pthread_create(&tid[MEM], NULL, parse_mem, &data.queue);
  pthread_create(&tid[NET], NULL, parse_net, &data.queue);
  pthread_create(&tid[PROCESS], NULL, parse_process, &data.queue);
  pthread_create(&tid[SEND], NULL, send_packet, &data);

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
