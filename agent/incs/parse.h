#ifndef PARSE_H_
# define PARSE_H_

# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>
# include <dirent.h>
# include <string.h>
# include <pwd.h>
# include "packet.h"

void parse_data(struct s_packet* packet);

#endif
