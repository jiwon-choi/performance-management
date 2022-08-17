#include "parse.h"

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080

int main(void) {
  int sock = 0;
  struct sockaddr_in serv_addr;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    exit(EXIT_FAILURE);
  }

  memset(&serv_addr, '0', sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    printf("\nInvalid address/ Address not supported \n");
    exit(EXIT_FAILURE);
  }

  int connect_return;
  if ((connect_return = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0) {
    printf("Connection Failed, %d\n", connect_return);
    exit(EXIT_FAILURE);
  }

  struct data data;
  parse_data(&data);

  printf("stat : %d %d %d %d\n", data.stat.user, data.stat.sys, data.stat.idle, data.stat.iowait);
  printf("mem : %d %d %d %d\n", data.mem.mem_total, data.mem.mem_free, data.mem.swap_total, data.mem.swap_free);
  printf("net : %s %d %d %d %d\n", data.net.interface, data.net.receive_bytes, data.net.receive_packets, data.net.transmit_bytes, data.net.transmit_packets);
  printf("process %d : %s %d %lu %lu %ld %ld %s\n", data.process.pid, data.process.comm, data.process.ppid, data.process.utime, data.process.stime, data.process.cutime, data.process.cstime, data.process.username);
  printf("process %d : %s %s\n", data.process.pid, data.process.cmdline[0], data.process.cmdline[1]);

  send(sock, &data, sizeof(data), 0);

  return (0);
}
