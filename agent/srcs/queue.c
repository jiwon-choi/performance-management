#include "queue.h"

void enqueue(struct s_packet** head, struct s_packet* new) {
  struct s_packet* p = *head;
  if (!*head) {
    *head = new;
    return;
  } else {
    while (p->next) {
      p = p->next;
    }
    p->next = new;
  }
}

struct s_packet* dequeue(struct s_packet** head) {
  if (!head || !*head) {
    return (NULL);
  }
  struct s_packet* pop = *head;
  *head = pop->next;
  return (pop);
}
