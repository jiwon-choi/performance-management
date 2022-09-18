#ifndef STRUCT_H_
# define STRUCT_H_

# include <pthread.h>

# include "packet.h"

struct s_queue_wrapper {
  struct s_packet*  queue;
  pthread_mutex_t   queue_mutex;
};

#endif
