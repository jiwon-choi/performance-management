#include "log.h"

void write_log(char* msg) {
  time_t tm;
  char* stm;

  time(&tm);
  stm = ctime(&tm);
  pthread_mutex_lock(&g_log_mutex);
  write(g_log_fd, "[", 1);
  write(g_log_fd, stm, strlen(stm) - 1);
  write(g_log_fd, "] ", 2);
  write(g_log_fd, msg, strlen(msg));
  write(g_log_fd, "\n", 1);
  pthread_mutex_unlock(&g_log_mutex);
}
