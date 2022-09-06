#include "send.h"
#include <stdio.h>

void* send_packet(void* vparam) {
  struct s_thread_param* param = (struct s_thread_param*)vparam;

  while (1) {
    if (!param->queue) {
      sleep(1);
      continue;
    }

    pthread_mutex_lock(&(param->queue_mutex));
    struct s_packet* pop = dequeue(&(param->queue));
    pthread_mutex_unlock(&(param->queue_mutex));

    struct s_header* header = pop->data;
    if (header->type_of_body == STAT) {
      printf("send STAT!\n");
    } else if (header->type_of_body == MEM) {
      printf("send MEM!\n");
    } else if (header->type_of_body == NET) {
      printf("send NET!\n");
    } else if (header->type_of_body == PROCESS) {
      printf("send PROCESS!\n");
    }
    if (write(param->socket, pop->data, pop->size) < 0) {
      printf("!! server closed\n");
      exit(EXIT_FAILURE);
    }
    free(pop->data);
    pop->data = NULL;
    free(pop);
  }
  return (0);
}
