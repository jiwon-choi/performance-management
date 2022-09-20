#ifndef STRUCT_H_
# define STRUCT_H_

# include <mysql.h>
# include <pthread.h>

# include "packet.h"

struct s_queue_wrapper {
  struct s_packet*  queue;
  pthread_mutex_t   queue_mutex;
};

struct s_recv_param {
  int                     socket;
  struct s_queue_wrapper* qwrapper;
};

struct s_worker_param {
	MYSQL* db;
	struct s_queue_wrapper* qwrapper;
};

#endif
