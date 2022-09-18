#include "socket.h"

extern pthread_mutex_t g_log_mutex;

void tcp_connection(int* server_fd, struct sockaddr_in* address) {
  if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    write_log("TCP socket error");
    exit(EXIT_FAILURE);
  }

  memset(address, 0, sizeof(*address));
  address->sin_family = AF_INET;
  address->sin_addr.s_addr = INADDR_ANY;
  address->sin_port = htons(PORT);

  if (bind(*server_fd, (struct sockaddr *)address, sizeof(*address)) == -1) {
    write_log("TCP bind error");
    exit(EXIT_FAILURE);
  }
  if (listen(*server_fd, 20) < 0) {
    write_log("TCP listen error");
    exit(EXIT_FAILURE);
  }
}

void* udp_connection() {
  int server_fd;

  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len;

  if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    write_log("UDP socket error");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(2424);

  if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    write_log("UDP bind error");
    exit(EXIT_FAILURE);
  }

  while (1) {
    char buf[100000] = { 0, };
    client_addr_len = sizeof(client_addr);
    struct s_udp_begin* begin;
    struct s_udp_end* end;

    int read_size;
    if ((read_size = recvfrom(server_fd, buf, sizeof(struct s_udp_begin), 0, (struct sockaddr*)&client_addr, &client_addr_len)) < 0)
      continue;

    if (read_size != sizeof(struct s_udp_begin))
      continue;
    begin = (struct s_udp_begin*) buf;

    if ((read_size = recvfrom(server_fd, buf, sizeof(struct s_udp_end), 0, (struct sockaddr*)&client_addr, &client_addr_len)) < 0)
      continue;

    if (read_size != sizeof(struct s_udp_end))
      continue;
    end = (struct s_udp_end*)buf;

    save_udp(begin, end);
  }
  close(server_fd);
  return (0);
}

void* recv_packet(void* vparam) {
  struct s_recv_param* param = (struct s_recv_param*)vparam;
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
      pthread_mutex_lock(&(param->qwrapper->queue_mutex));
      enqueue(&param->qwrapper->queue, packet);
      pthread_mutex_unlock(&(param->qwrapper->queue_mutex));
      char buf[32];
      sprintf(buf, "Receive %s's data", header->agent_name);
      write_log(buf);
    }
  }
  free(vparam);
  return (0);
}
