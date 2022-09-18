#include "init.h"

void init_daemon() {
  pid_t pid = fork();

  if (pid < 0) {
    write(STDERR_FILENO, "Error fork()\n", 13);
    exit(EXIT_FAILURE);
  } else if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  signal(SIGHUP, SIG_IGN);
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  setsid();
}
