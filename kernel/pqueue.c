#include "defs.h"
#include "proc.h"

int push_queue(struct pqueue *pq, struct proc *proc) {
  struct proc *pcurr = 0;
  int i, j;

  int8 prio = proc->prio;

  for (i = pq->head; i != pq->tail; i = (i + 1) % NPROC) {
    pcurr = pq->queue[i];

    if (!pcurr) {
      break;
    }

    if (prio > pcurr->prio) {
      struct proc *pprev = pcurr;
      // Move them back
      for (j = i; j != pq->tail; j = (j + 1) % NPROC) {
        pcurr = pq->queue[j];

        pq->queue[(j + 1) % NPROC] = pprev;

        pprev = pcurr;
      }

      goto END;
    }
  }

END:
  // Insert chosen process
  pq->queue[i] = proc;

  // Grow the queue
  if ((pq->tail + 1) % NPROC != pq->head) {
    pq->tail = (pq->tail + 1) % NPROC;
  } else {
    // Queue is full, fail to push
    return -1;
  }

  return 0;
}

struct proc *pop_queue(struct pqueue *pq) {
  struct proc *ret = 0;

  if (pq->head == pq->tail) {
    // Queue is empty, fail to pop
    return 0;
  }

  ret = pq->queue[pq->head];

  pq->head = (pq->head + 1) % NPROC;

  return ret;
}

void show_queue(struct pqueue *pq) {
  int i;
  printf("[head]: %lu\n", pq->head);
  printf("[tail]: %lu\n", pq->tail);

  for (i = pq->head; i != pq->tail; i = (i + 1) % NPROC) {
    printf("[%d]: %p\n", i, pq->queue[i]);
  }

  return;
}

int is_empty(struct pqueue *pq) { return pq->head == pq->tail; }
