#ifndef AGENT_H_
# define AGENT_H_

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/stat.h>

# include "connect.h"
# include "parse.h"
# include "send.h"
# include "sig.h"
# include "queue.h"

extern pthread_mutex_t g_log_mutex;

#endif
