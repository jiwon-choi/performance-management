#ifndef QUEUE_H_
# define QUEUE_H_

# include "../../agent/incs/packet.h"

void enqueue(struct s_packet** head, struct s_packet* new_node);
struct s_packet* dequeue(struct s_packet** head);
struct s_packet* peek(struct s_packet** head);

#endif
