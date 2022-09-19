#ifndef SOCKET_H_
# define SOCKET_H_

# include <arpa/inet.h>
# include <netinet/in.h>
# include <signal.h>
# include <stdlib.h>
# include <string.h>
# include <sys/socket.h>
# include <unistd.h>

# include "utils.h"
# include "queue.h"
# include "struct.h"

# define PORT 4242

void tcp_reconnect();
int tcp_connect();
void* send_packet(void* vparam);

#endif
