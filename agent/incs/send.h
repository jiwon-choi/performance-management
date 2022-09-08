#ifndef SEND_H_
# define SEND_H_

# include <signal.h>
# include <stdlib.h>
# include <unistd.h>

# include "connect.h"
# include "log.h"
# include "queue.h"
# include "struct.h"

void* send_packet(void* vparam);

#endif
