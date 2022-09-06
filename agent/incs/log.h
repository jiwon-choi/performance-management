#ifndef LOG_H_
# define LOG_H_

# include <stdio.h>
# include <string.h>
# include <pthread.h>
# include <time.h>
# include <unistd.h>

pthread_mutex_t g_log_mutex;

void write_log(char* msg);

#endif
