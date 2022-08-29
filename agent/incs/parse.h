#ifndef PARSE_H_
# define PARSE_H_

# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>
# include <dirent.h>
# include <string.h>
# include <pwd.h>
# include <unistd.h>

# include "packet.h"
# include "queue.h"

void* parse_stat(void* vparam);
void* parse_mem(void* vparam);
void* parse_net(void* vparam);
void* parse_process(void* vparam);

#endif
