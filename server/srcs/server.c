#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>

#include "queue.h"
#include "get.h"

#define PORT 4242

int g_stderrFd;

void listening(int* server_fd, struct sockaddr_in* address) {
  if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    write(g_stderrFd, "[ERROR] socket()\n", 8 + 9);
    exit(EXIT_FAILURE);
  }

  address->sin_family = AF_INET;
  address->sin_addr.s_addr = INADDR_ANY;
  address->sin_port = htons(PORT);
  memset(address->sin_zero, 0, sizeof(address->sin_zero));

  if (bind(*server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
    write(g_stderrFd, "[ERROR] bind()\n", 8 + 7);
    exit(EXIT_FAILURE);
  }
  if (listen(*server_fd, 20) < 0) {
    write(g_stderrFd, "[ERROR] listen()\n", 8 + 9);
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
      insert_queue(&param->queue, packet);
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
    struct s_packet* pop = pop_queue(&(param->queue));
    pthread_mutex_unlock(&(param->queue_mutex));
    if (!pop) continue;
    struct s_header* header = pop->data;
    if (header->type_of_body == STAT) {
      get_stat(pop);
    } else if (header->type_of_body == MEM) {
      get_mem(pop);
    } else if (header->type_of_body == NET) {
      get_net(pop);
    } else if (header->type_of_body == PROCESS) {
      get_process(pop);
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
    write(STDERR_FILENO, "[ERROR] fork()\n", 8 + 7);
    exit(EXIT_FAILURE);
  } else if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  signal(SIGHUP, SIG_IGN);
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  g_stderrFd = dup(STDERR_FILENO);
  close(STDERR_FILENO);
  chdir("/");
  setsid();


  int server_fd;
  struct sockaddr_in address;
  listening(&server_fd, &address);

  int addr_size = sizeof(address);
  struct s_thread_param data;
  data.queue = NULL;
  pthread_mutex_init(&data.queue_mutex, NULL);

  pthread_t tid;
  for (int i = 0; i < 8; i++) {
    pthread_create(&tid, NULL, run_worker, &data);
    pthread_detach(tid);
  }

  while (1) {
    // write(g_stderrFd, "[start listen...]\n");
    if ((data.socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addr_size)) < 0) {
      exit(EXIT_FAILURE);
    }
    pthread_create(&tid, NULL, recv_packet, &data);
    pthread_detach(tid);
  }

  return (0);
}
