#ifndef UTILS_H_
# define UTILS_H_

# include <pthread.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <time.h>
# include <unistd.h>

void init_daemon();
void write_log(const char* msg);
double gettimeofnow();

#endif
