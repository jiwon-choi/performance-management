#include "agent.h"

int g_debug_fd;
char g_agent_name[9];
pthread_mutex_t g_log_mutex;

int main(int argc, char* argv[]) {
  if (argc != 2)
    write(STDERR_FILENO, "Please enter a agent name\n", 26);

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
  strncpy(g_agent_name, argv[1], 9);
  g_agent_name[8] = 0;
  char buf[32];
  sprintf(buf, "Run Agent <%s>", g_agent_name);
  write_log(buf);

  set_signal();

  pthread_t tid[5];
  struct s_thread_param param;

  param.queue = NULL;
  pthread_mutex_init(&param.queue_mutex, NULL);

  pthread_create(&tid[STAT], NULL, parse_stat, &param);
  pthread_create(&tid[MEM], NULL, parse_mem, &param);
  pthread_create(&tid[NET], NULL, parse_net, &param);
  pthread_create(&tid[PROCESS], NULL, parse_process, &param);
  pthread_create(&tid[SEND], NULL, send_packet, &param);

  pthread_join(tid[STAT], NULL);
  pthread_join(tid[MEM], NULL);
  pthread_join(tid[NET], NULL);
  pthread_join(tid[PROCESS], NULL);
  pthread_join(tid[SEND], NULL);

  pthread_mutex_destroy(&param.queue_mutex);

  return (0);
}
