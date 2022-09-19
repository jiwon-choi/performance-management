#ifndef SIG_H_
# define SIG_H_

# include <signal.h>
# include <stdlib.h>

# include "utils.h"

void signal_handler(int sig);
void set_signal(void);

#endif
