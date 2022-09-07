#ifndef AGENT_H_
# define AGENT_H_

# include <arpa/inet.h>
# include <netinet/in.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/socket.h>
# include <sys/stat.h>

# include "log.h"
# include "parse.h"
# include "send.h"
# include "sig.h"
# include "queue.h"

# define PORT 4242

extern pthread_mutex_t g_log_mutex;

#endif
