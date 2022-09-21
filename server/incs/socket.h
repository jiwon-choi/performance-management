#ifndef SOCKET_H_
# define SOCKET_H_

# include <netinet/in.h>
# include <pthread.h>
# include <string.h>
# include <sys/socket.h>

# include "utils.h"
# include "queue.h"
# include "save.h"
# include "struct.h"

# define PORT 4242

void tcp_connection(int* server_fd, struct sockaddr_in* address);
void* udp_connection(void* db);
void* recv_packet(void* vparam);

#endif
