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

void* parse_stat(void* queue);
void* parse_mem(void* queue);
void* parse_net(void* queue);
// void* parse_process();

#endif
