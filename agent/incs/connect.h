#ifndef CONNECT_H_
# define CONNECT_H_

# include <arpa/inet.h>
# include <netinet/in.h>
# include <stdlib.h>
# include <string.h>
# include <sys/socket.h>

# include "log.h"

# define PORT 4242

void tcp_reconnect();
int tcp_connect();

#endif
