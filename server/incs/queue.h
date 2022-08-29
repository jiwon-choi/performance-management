#ifndef QUEUE_H_
# define QUEUE_H_

# include "../../agent/incs/packet.h"

void insert_queue(struct s_packet** head, struct s_packet* new_node);
struct s_packet* pop_queue(struct s_packet** head);

#endif
