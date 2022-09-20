#include "agent.h"

char g_agent_name[9];
extern pthread_mutex_t g_log_mutex;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Please enter a agent name\n");
    exit(EXIT_FAILURE);
  }

  init_daemon();

  mkdir("files", 0777);
  mkdir("files/logs", 0777);
  pthread_mutex_init(&g_log_mutex, NULL);
  strncpy(g_agent_name, argv[1], 9);
  g_agent_name[8] = 0;
  char buf[32];
  sprintf(buf, "Run Agent <%s>", g_agent_name);
  write_log(buf);

  set_signal();

  pthread_t tid[5];
  struct s_queue_wrapper qwrapper;

  qwrapper.queue = NULL;
  pthread_mutex_init(&qwrapper.queue_mutex, NULL);

  pthread_create(&tid[STAT], NULL, parse_stat, &qwrapper);
  pthread_create(&tid[MEM], NULL, parse_mem, &qwrapper);
  pthread_create(&tid[NET], NULL, parse_net, &qwrapper);
  pthread_create(&tid[PROCESS], NULL, parse_process, &qwrapper);
  pthread_create(&tid[SEND], NULL, send_packet, &qwrapper);

  pthread_join(tid[STAT], NULL);
  pthread_join(tid[MEM], NULL);
  pthread_join(tid[NET], NULL);
  pthread_join(tid[PROCESS], NULL);
  pthread_join(tid[SEND], NULL);

  pthread_mutex_destroy(&qwrapper.queue_mutex);

  return (0);
}
