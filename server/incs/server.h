#ifndef SERVER_H_
# define SERVER_H_

# include <stdio.h>
# include <sys/socket.h>
# include <unistd.h>
# include <stdlib.h>
# include <netinet/in.h>
# include <string.h>
# include <pthread.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <signal.h>
# include <fcntl.h>

# include "queue.h"
# include "save.h"
# include "log.h"

# define PORT 4242

extern pthread_mutex_t g_log_mutex;

#endif
