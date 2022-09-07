#include "log.h"

void write_log(char* msg) {
  time_t tm;
  char str_time[25];

  time(&tm);
  strncpy(str_time, ctime(&tm), 25);
  str_time[24] = 0;
  pthread_mutex_lock(&g_log_mutex);
  FILE* fp = fopen("files/log", "a+");
  fprintf(fp, "[%s] %s\n", str_time, msg);
  fclose(fp);
  pthread_mutex_unlock(&g_log_mutex);
}
