#define _GNU_SOURCE

#include <arpa/inet.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "incs/packet.h"

int sock;
struct sockaddr_in  serv_addr;
struct s_udp_begin  begin;
struct s_udp_end    end;

ssize_t (*origin_write)(int fd, const void* buf, size_t count);

void __attribute__((constructor)) before_main() {
  origin_write = (ssize_t (*)(int, const void*, size_t))dlsym(RTLD_NEXT, "write");

  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    printf("UDP socket error");
    exit(EXIT_FAILURE);
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(2424);
}

ssize_t write(int fd, const void* buf, size_t count) {
  static int num = 0;
  struct sockaddr_in  peer;
  socklen_t size = sizeof(peer);

  memset(&peer, 0, sizeof(peer));
  getpeername(fd, (struct sockaddr*)&peer, &size);
  strcpy(begin.agent_name, ((struct s_header*)buf)->agent_name);
  begin.pid = getpid();
  strcpy(begin.peer_ip, inet_ntoa(peer.sin_addr));
  begin.port = peer.sin_port;
  begin.pkt_no = num++;

  sendto(sock, &begin, sizeof(struct s_udp_begin), 0, (struct sockaddr*)(&serv_addr), sizeof(serv_addr));
  time(&(begin.begin_time));
  ssize_t send_byte = (*origin_write)(fd, buf, count);
  time(&(end.end_time));
  strcpy(end.agent_name, begin.agent_name);
  end.pid = begin.pid;
  end.send_byte = send_byte;
  end.elapse_time = end.end_time - begin.begin_time;
  sendto(sock, &end, sizeof(struct s_udp_end), 0, (struct sockaddr*)(&serv_addr), sizeof(serv_addr));

  return (send_byte);
}
