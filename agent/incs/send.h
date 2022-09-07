#ifndef SEND_H_
# define SEND_H_

# include <stdlib.h>
# include <unistd.h>

# include "connect.h"
# include "log.h"
# include "struct.h"
# include "queue.h"

extern int g_socket;

void* send_packet(void* vparam);

#endif
