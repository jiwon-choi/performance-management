#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include "../../agent/incs/struct.h"

#define PORT 8080

int main(void) {
  int server_fd;
  struct sockaddr_in address;
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);
  memset(address.sin_zero, '\0', sizeof address.sin_zero);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 10) < 0) {
    exit(EXIT_FAILURE);
  }

  while (1) {
    int new_socket;
    int addr_size = sizeof(address);

    printf("\n+++++++ Waiting for new connection ++++++++\n");
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addr_size)) < 0) {
      exit(EXIT_FAILURE);
    }

    struct data data;
    if (read(new_socket, &data, sizeof(data)) < 0) {
      exit(EXIT_FAILURE);
    }

    printf("stat : %d %d %d %d\n", data.stat.user, data.stat.sys, data.stat.idle, data.stat.iowait);
    printf("mem : %d %d %d %d\n", data.mem.mem_total, data.mem.mem_free, data.mem.swap_total, data.mem.swap_free);
    printf("net : %s %d %d %d %d\n", data.net.interface, data.net.receive_bytes, data.net.receive_packets, data.net.transmit_bytes, data.net.transmit_packets);
    printf("process %d : %s %d %lu %lu %ld %ld %s\n", data.process.pid, data.process.comm, data.process.ppid, data.process.utime, data.process.stime, data.process.cutime, data.process.cstime, data.process.username);
    printf("process %d : %s %s\n", data.process.pid, data.process.cmdline[0], data.process.cmdline[1]);

    write(new_socket, "OK", strlen("OK"));
    close(new_socket);
  }
  return (0);
}
