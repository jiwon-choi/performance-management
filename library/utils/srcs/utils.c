#include "utils.h"

pthread_mutex_t g_log_mutex;

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

void write_log(char* msg) {
  time_t tm;
  char str_time[25];

  time(&tm);
  strncpy(str_time, ctime(&tm), 25);
  str_time[24] = 0;
  pthread_mutex_lock(&g_log_mutex);

  char filename[50];
  struct tm s_time;
  localtime_r(&tm, &s_time);
  sprintf(filename, "files/logs/%d-%02d-%02d_log", s_time.tm_year + 1900, s_time.tm_mon + 1, s_time.tm_mday);

  FILE* fp = fopen(filename, "a+");
  fprintf(fp, "[%s] %s\n", str_time, msg);
  fclose(fp);
  pthread_mutex_unlock(&g_log_mutex);
}

double gettimeofnow() {
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return ((double)((tv.tv_sec * 1000000) + tv.tv_usec) / 1000);
}
