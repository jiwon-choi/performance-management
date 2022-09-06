#ifndef SEND_H_
# define SEND_H_

# include <stdlib.h>
# include <unistd.h>

# include "log.h"
# include "struct.h"
# include "queue.h"

void* send_packet(void* vparam);

#endif
