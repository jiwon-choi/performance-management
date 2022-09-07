#include "connect.h"

int g_socket;

void tcp_reconnect() {
  write_log("Attempt to reconnection");
  close(g_socket);
  g_socket = tcp_connect();
  write_log("Succeeded to reconnection");
}

int tcp_connect() {
  int new_socket;
  while (1) {
    struct sockaddr_in serv_addr;
    if ((new_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      sleep(1);
      continue;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
      write_log("Invalid address/ Address not supported");
      close(new_socket);
      sleep(1);
      continue;
    }

    if (connect(new_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
      write_log("Failed to reconnection");
      close(new_socket);
      sleep(1);
      continue;
    }
    break;
  }
  return (new_socket);
}
