#include "server.h"

int g_debug_fd;
extern pthread_mutex_t g_log_mutex;

void listening(int* server_fd, struct sockaddr_in* address) {
  if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    write_log("Socket error");
    exit(EXIT_FAILURE);
  }

  address->sin_family = AF_INET;
  address->sin_addr.s_addr = INADDR_ANY;
  address->sin_port = htons(PORT);
  memset(address->sin_zero, 0, sizeof(address->sin_zero));

  if (bind(*server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
    write_log("Bind error");
    exit(EXIT_FAILURE);
  }
  if (listen(*server_fd, 20) < 0) {
    write_log("Listen error");
    exit(EXIT_FAILURE);
  }
}

void* recv_packet(void* vparam) {
  struct s_thread_param* param = (struct s_thread_param*)vparam;
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
      pthread_mutex_lock(&(param->queue_mutex));
      enqueue(&param->queue, packet);
      pthread_mutex_unlock(&(param->queue_mutex));
    }
  }
  if (rd_size < 0)
    exit(EXIT_FAILURE);
  return (0);
}

void* run_worker(void* vparam) {
  struct s_thread_param* param = (struct s_thread_param*)vparam;

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

void signal_handler(int sig) {
  if (sig == SIGINT) {
    write_log("Signal SIGINT");
  } else if (sig == SIGQUIT) {
    write_log("Signal SIGQUIT");
  } else if (sig == SIGILL) {
    write_log("Signal SIGILL");
  } else if (sig == SIGABRT) {
    write_log("Signal SIGABRT");
  } else if (sig == SIGIOT) {
    write_log("Signal SIGIOT");
  } else if (sig == SIGKILL) {
    write_log("Signal SIGKILL");
  } else if (sig == SIGBUS) {
    write_log("Signal SIGBUS");
  } else if (sig == SIGSEGV) {
    write_log("Signal SIGSEGV");
  } else if (sig == SIGTERM) {
    write_log("Signal SIGTERM");
  } else {
    write_log("Signal another");
  }
  exit(EXIT_FAILURE);
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
  g_debug_fd = dup(STDOUT_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  // chdir("/");
  setsid();

  mkdir("files", 0777);
  pthread_mutex_init(&g_log_mutex, NULL);
  write_log("Run server");

  signal(SIGINT, signal_handler);
  signal(SIGQUIT, signal_handler);
  signal(SIGILL, signal_handler);
  signal(SIGABRT, signal_handler);
  signal(SIGIOT, signal_handler);
  signal(SIGKILL, signal_handler);
  signal(SIGBUS, signal_handler);
  signal(SIGSEGV, signal_handler);
  signal(SIGTERM, signal_handler);

  int server_fd;
  struct sockaddr_in address;
  listening(&server_fd, &address);

  int addr_size = sizeof(address);
  struct s_thread_param data;
  data.queue = NULL;
  pthread_mutex_init(&data.queue_mutex, NULL);

  pthread_t tid;
  for (int i = 0; i < 8; i++) {
    char buf[20];
    pthread_create(&tid, NULL, run_worker, &data);
    pthread_detach(tid);
    sprintf(buf, "Create worker%d", i + 1);
    write_log(buf);
  }

  write_log("Start listen");
  while (1) {
    if ((data.socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addr_size)) < 0) {
      exit(EXIT_FAILURE);
    }
    write_log("Accept an agent");
    pthread_create(&tid, NULL, recv_packet, &data);
    pthread_detach(tid);
    write_log("Create recv thread");
  }

  return (0);
}
