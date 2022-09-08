#include "send.h"

extern int g_socket;

void* send_packet(void* vparam) {
  struct s_queue_wrapper* param = (struct s_queue_wrapper*)vparam;

  g_socket = tcp_connect();
  signal(SIGPIPE, tcp_reconnect);

  while (1) {
    if (!param->queue) {
      sleep(1);
      continue;
    }

    struct s_packet* tmp = peek(&(param->queue));
    if (write(g_socket, tmp->data, tmp->size) < 0) {
      sleep(1);
      continue;
    }
    write_log("Send a packet");
    pthread_mutex_lock(&(param->queue_mutex));
    struct s_packet* pop = dequeue(&(param->queue));
    pthread_mutex_unlock(&(param->queue_mutex));
    free(pop->data);
    pop->data = NULL;
    free(pop);
  }
  return (0);
}
