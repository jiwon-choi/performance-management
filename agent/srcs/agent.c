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

int main() {
  struct s_packet packet;

  while (1) {
    // int sock = connection();
    // init header

    parse_data(&packet);
    // write(sock, &(packet.body.stat), sizeof(struct s_stat));
    // write(sock, &(packet.body.mem), sizeof(struct s_mem));
    // write(sock, &(packet.body.net), sizeof(struct s_net) * packet.header.net_size);
    // write(sock, &(packet.body.process), sizeof(struct s_process) * packet.header.process_size);

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
    sleep(2);
  }
  return (0);
}
