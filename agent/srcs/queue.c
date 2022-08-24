#include "queue.h"
#include <stdio.h>

void insert_queue(struct s_packet** head, struct s_packet* new) {
  if (!*head) {
    *head = new;
  } else {
    // while (p->next) {
    //   p = p->next;
    // }
    // p->next = new;
    insert_queue(&(*head)->next, new);
  }
}

struct s_packet* pop_queue() {
  return (0);
}
