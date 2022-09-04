#ifndef SAVE_H_
# define SAVE_H_

void save_stat(struct s_packet* packet);
void save_mem(struct s_packet* packet);
void save_net(struct s_packet* packet);
void save_process(struct s_packet* packet);

#endif
