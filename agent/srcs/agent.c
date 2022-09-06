#include "agent.h"

static
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
