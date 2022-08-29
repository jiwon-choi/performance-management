#include "agent.h"

int connection() {
  int sock = 0;
  struct sockaddr_in serv_addr;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    exit(EXIT_FAILURE);
  }

  memset(&serv_addr, '\0', sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    printf("\nInvalid address/ Address not supported \n");
    exit(EXIT_FAILURE);
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("Connection Failed\n");
    exit(EXIT_FAILURE);
  }
  return (sock);
}

void* send_packet(void* vparam) {
  struct s_thread_param* param = (struct s_thread_param*)vparam;

  while (1) {
    if (!param->queue) {
      sleep(1);
      continue;
    }

    pthread_mutex_lock(&(param->queue_mutex));
    struct s_packet* pop = pop_queue(&(param->queue));
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

int main() {
  pthread_t tid[5];
  struct s_thread_param data;

  data.queue = NULL;
  data.socket = connection();
  pthread_mutex_init(&data.queue_mutex, NULL);

  pthread_create(&tid[STAT], NULL, parse_stat, &data);
  pthread_create(&tid[MEM], NULL, parse_mem, &data);
  pthread_create(&tid[NET], NULL, parse_net, &data);
  pthread_create(&tid[PROCESS], NULL, parse_process, &data);
  pthread_create(&tid[SEND], NULL, send_packet, &data);

  pthread_join(tid[STAT], NULL);
  pthread_join(tid[MEM], NULL);
  pthread_join(tid[NET], NULL);
  pthread_join(tid[PROCESS], NULL);
  pthread_join(tid[SEND], NULL);

  pthread_mutex_destroy(&data.queue_mutex);

  return (0);
}
