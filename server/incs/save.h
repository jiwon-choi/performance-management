#ifndef SAVE_H_
# define SAVE_H_

# include <stdio.h>
# include <string.h>
# include <sys/stat.h>
# include <unistd.h>

# include "packet.h"

void save_udp(struct s_udp_begin* begin, struct s_udp_end* end);
void save_stat(struct s_packet* packet);
void save_mem(struct s_packet* packet);
void save_net(struct s_packet* packet);
void save_process(struct s_packet* packet);

#endif
