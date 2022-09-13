#include "server.h"

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
    // struct s_udp_begin  *begin;
    // struct s_udp_end    *end;

    int read_size = recvfrom(server_fd, buf, 100000, 0, (struct sockaddr*)&client_addr, &client_addr_len);
    if (read_size == sizeof(struct s_udp_begin)) {
      printf("begin agent name %s\n", ((struct s_udp_begin*)buf)->agent_name);
    } else if (read_size == sizeof(struct s_udp_end)) {
      printf("end agent name %s\n", ((struct s_udp_end*)buf)->agent_name);
    } else {
      printf("else!!!!!!!!!!!!\n");
    }
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

void* run_worker(void* vparam) {
  struct s_queue_wrapper* param = (struct s_queue_wrapper*)vparam;

  while (1) {
    if (!param->queue) {
      sleep(1);
      continue;
    }
    pthread_mutex_lock(&(param->queue_mutex));
    struct s_packet* pop = dequeue(&(param->queue));
    pthread_mutex_unlock(&(param->queue_mutex));
    if (!pop) continue;
    struct s_header* header = pop->data;
    if (header->type_of_body == STAT) {
      save_stat(pop);
    } else if (header->type_of_body == MEM) {
      save_mem(pop);
    } else if (header->type_of_body == NET) {
      save_net(pop);
    } else if (header->type_of_body == PROCESS) {
      save_process(pop);
    }
    free(pop->data);
    pop->data = NULL;
    free(pop);
  }
  return (0);
}

int main(void) {
  pid_t pid = fork();

  if (pid < 0) {
    write(STDERR_FILENO, "Error fork()\n", 13);
    exit(EXIT_FAILURE);
  } else if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  signal(SIGHUP, SIG_IGN);
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  setsid();

  mkdir("files", 0777);
  pthread_mutex_init(&g_log_mutex, NULL);
  write_log("Run server");

  set_signal();

  int server_fd;
  struct sockaddr_in address;
  tcp_connection(&server_fd, &address);
  int addr_size = sizeof(address);

  struct s_queue_wrapper wrapper;
  wrapper.queue = NULL;
  pthread_mutex_init(&wrapper.queue_mutex, NULL);

  pthread_t tid;
  for (int i = 0; i < 8; i++) {
    char buf[40];
    pthread_create(&tid, NULL, run_worker, &wrapper);
    pthread_detach(tid);
    sprintf(buf, "Create worker thread %d", i + 1);
    write_log(buf);
  }

  pthread_create(&tid, NULL, udp_connection, NULL);
  pthread_detach(tid);
  write_log("Create UDP thread");

  write_log("Start TCP listen");
  struct s_recv_param* data;
  while (1) {
    data = malloc(sizeof(struct s_recv_param));
    data->qwrapper = &wrapper;
    if ((data->socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addr_size)) < 0) {
      exit(EXIT_FAILURE);
    }
    write_log("Accept an agent");
    pthread_create(&tid, NULL, recv_packet, data);
    pthread_detach(tid);
    write_log("Create recv thread");
  }

  return (0);
}
