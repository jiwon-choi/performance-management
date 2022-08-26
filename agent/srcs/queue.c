#include "queue.h"
#include <stdio.h>

void insert_queue(struct s_packet** head, struct s_packet* new) {
  if (!*head) {
    *head = new;
    return;
  }
  insert_queue(&(*head)->next, new);
}

struct s_packet* pop_queue(struct s_packet** head) {
  if (!head || !*head) {
    return (NULL);
  }
  struct s_packet* pop = *head;
  *head = pop->next;
  return (pop);
}
