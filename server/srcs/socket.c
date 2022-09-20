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
  write_log("TCP ready");
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
  write_log("UDP ready");

  while (1) {
    char buf[100000] = { 0, };
    client_addr_len = sizeof(client_addr);
    char msg[100];
    struct s_udp_begin* begin;
    struct s_udp_end* end;

    int read_size;
    if ((read_size = recvfrom(server_fd, buf, sizeof(struct s_udp_begin), 0, (struct sockaddr*)&client_addr, &client_addr_len)) < 0)
      continue;

    if (read_size != sizeof(struct s_udp_begin))
      continue;
    begin = (struct s_udp_begin*) buf;
    sprintf(msg, "Recv %s udp begin %dbytes", begin->agent_name, read_size);
    write_log(msg);

    if ((read_size = recvfrom(server_fd, buf, sizeof(struct s_udp_end), 0, (struct sockaddr*)&client_addr, &client_addr_len)) < 0)
      continue;

    if (read_size != sizeof(struct s_udp_end))
      continue;
    end = (struct s_udp_end*)buf;
    sprintf(msg, "Recv %s udp begin %dbytes", end->agent_name, read_size);
    write_log(msg);

    save_udp(begin, end);
  }
  close(server_fd);
  return (0);
}

void* recv_packet(void* vparam) {
  struct s_recv_param* param = (struct s_recv_param*)vparam;
  int rd_size;
  char buf[sizeof(struct s_header)] = { 0, };
  char agent_name[9] = "unknown";
  char msg[512];

  while (1) {
    int total_rd_size = 0;
    double begin_time = gettimeofnow();
    while (total_rd_size < (int)sizeof(struct s_header)) {
      if ((rd_size = read(param->socket, buf, sizeof(struct s_header))) <= 0) {
        goto EXIT;
      }
      total_rd_size += rd_size;
      sprintf(msg, "Receive %s header %dbytes, total %dbytes.", agent_name, rd_size, total_rd_size);
      write_log(msg);
    }

    struct s_header* header = (struct s_header*)buf;
    int packet_size;
    if (header->type_of_body == STAT) {
      packet_size = sizeof(struct s_header) + sizeof(struct s_stat) * header->number_of_body;
    } else if (header->type_of_body == MEM) {
      packet_size = sizeof(struct s_header) + sizeof(struct s_mem) * header->number_of_body;
    } else if (header->type_of_body == NET) {
      packet_size = sizeof(struct s_header) + sizeof(struct s_net) * header->number_of_body;
    } else if (header->type_of_body == PROCESS) {
      packet_size = sizeof(struct s_header) + sizeof(struct s_process) * header->number_of_body;
    } else {
      sprintf(msg, "Wrong data type");
      write_log(msg);
      break;
    }
    strcpy(agent_name, header->agent_name);

    struct s_packet* packet = malloc(sizeof(struct s_packet));
    packet->size = packet_size;
    packet->data = malloc(packet->size);
    packet->next = NULL;

    memcpy(packet->data, header, sizeof(struct s_header));
    void* body = packet->data + sizeof(struct s_header);
    while (total_rd_size < packet->size) {
      if ((rd_size = read(param->socket, body, packet->size - sizeof(struct s_header))) <= 0) {
        free(packet->data);
        packet->data = NULL;
        free(packet);
        packet = NULL;
        goto EXIT;
      }
      total_rd_size += rd_size;
      sprintf(msg, "Receive %s body %dbytes, total %dbytes", agent_name, rd_size, total_rd_size);
      write_log(msg);
    }
    double end_time = gettimeofnow();
    sprintf(msg, "Receive total %fms", end_time - begin_time);
    write_log(msg);

    pthread_mutex_lock(&(param->qwrapper->queue_mutex));
    enqueue(&param->qwrapper->queue, packet);
    pthread_mutex_unlock(&(param->qwrapper->queue_mutex));
  }

EXIT:
  sprintf(msg, "%s tcp disconnected", agent_name);
  write_log(msg);
  close(param->socket);
  free(vparam);
  return (0);
}
