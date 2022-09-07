#include "send.h"

void* send_packet(void* vparam) {
  struct s_thread_param* param = (struct s_thread_param*)vparam;

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
    write_log("send a packet");
    pthread_mutex_lock(&(param->queue_mutex));
    struct s_packet* pop = dequeue(&(param->queue));
    pthread_mutex_unlock(&(param->queue_mutex));
    free(pop->data);
    pop->data = NULL;
    free(pop);
  }
  return (0);
}
