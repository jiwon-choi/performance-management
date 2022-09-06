#include "agent.h"

int g_debug_fd;
char g_agent_name[9];

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
    write_log("Invalid address/ Address not supported");
    exit(EXIT_FAILURE);
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    write_log("Connection Failed");
    exit(EXIT_FAILURE);
  }
  return (sock);
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

int main(int argc, char* argv[]) {
  if (argc != 2)
    write(STDERR_FILENO, "Error argc\n", 11);

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
  strlcpy(g_agent_name, argv[1], 9);
  char buf[32];
  sprintf(buf, "Run Agent <%s>", g_agent_name);
  write_log(buf);

  signal(SIGINT, signal_handler);
  signal(SIGQUIT, signal_handler);
  signal(SIGILL, signal_handler);
  signal(SIGABRT, signal_handler);
  signal(SIGIOT, signal_handler);
  signal(SIGKILL, signal_handler);
  signal(SIGBUS, signal_handler);
  signal(SIGSEGV, signal_handler);
  signal(SIGTERM, signal_handler);

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
