#include "sig.h"

void signal_handler(int sig) {
  switch (sig) {
  case SIGPIPE:
    write_log("Signal SIGPIPE");
    tcp_reconnect();
    return;
  case SIGINT:
    write_log("Signal SIGINT");
    break;
  case SIGQUIT:
    write_log("Signal SIGQUIT");
    break;
  case SIGILL:
    write_log("Signal SIGILL");
    break;
  case SIGABRT:
    write_log("Signal SIGABRT");
    break;
  case SIGBUS:
    write_log("Signal SIGBUS");
    break;
  case SIGSEGV:
    write_log("Signal SIGSEGV");
    break;
  case SIGTERM:
    write_log("Signal SIGTERM");
    break;
  default:
    write_log("Signal another");
  }
  exit(EXIT_FAILURE);
}

void set_signal() {
  signal(SIGINT, signal_handler);
  signal(SIGQUIT, signal_handler);
  signal(SIGILL, signal_handler);
  signal(SIGABRT, signal_handler);
  signal(SIGIOT, signal_handler);
  signal(SIGBUS, signal_handler);
  signal(SIGSEGV, signal_handler);
  signal(SIGTERM, signal_handler);
  signal(SIGPIPE, signal_handler);
}
