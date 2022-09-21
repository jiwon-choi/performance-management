#ifndef SAVE_H_
# define SAVE_H_

# include <mysql.h>
# include <stdio.h>
# include <string.h>
# include <sys/stat.h>
# include <unistd.h>

# include "struct.h"
# include "packet.h"
# include "utils.h"

void save_udp(struct s_udp_begin* begin, struct s_udp_end* end, MYSQL* db);
void save_stat(struct s_packet* packet, MYSQL* db);
void save_mem(struct s_packet* packet, MYSQL* db);
void save_net(struct s_packet* packet, MYSQL* db);
void save_process(struct s_packet* packet, MYSQL* db);

#endif
