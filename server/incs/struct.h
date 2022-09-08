#ifndef STRUCT_H_
# define STRUCT_H_

# include <pthread.h>

#include "../../agent/incs/packet.h"

struct s_queue_wrapper {
  struct s_packet*  queue;
  pthread_mutex_t   queue_mutex;
};

struct s_recv_param {
  int                     socket;
  struct s_queue_wrapper* qwrapper;
};

#endif
