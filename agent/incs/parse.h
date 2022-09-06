#ifndef PARSE_H_
# define PARSE_H_

# include <dirent.h>
# include <fcntl.h>
# include <pwd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

# include "struct.h"
# include "queue.h"

void* parse_stat(void* vparam);
void* parse_mem(void* vparam);
void* parse_net(void* vparam);
void* parse_process(void* vparam);

#endif
