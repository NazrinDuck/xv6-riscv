#include "defs.h"
#include "proc.h"

int push_queue(struct pqueue *pq, struct proc *proc) {
  struct proc *pcurr = 0;
  int i, j;

  int8 prio = proc->prio;

  for (i = pq->head; i != pq->tail; i = (i + 1) % MAX_QUEUE) {
    pcurr = pq->queue[i];

    if (!pcurr) {
      break;
    }

    if (prio > pcurr->prio) {
      struct proc *pprev = pcurr;
      // Move them back
      for (j = i; j != pq->tail; j = (j + 1) % MAX_QUEUE) {
        pcurr = pq->queue[(j + 1) % MAX_QUEUE];

        pq->queue[(j + 1) % MAX_QUEUE] = pprev;

        pprev = pcurr;
      }

      goto END;
    }
  }

END:
  //  Insert chosen process
  pq->queue[i] = proc;

  // Grow the queue
  if ((pq->tail + 1) % MAX_QUEUE != pq->head) {
    pq->tail = (pq->tail + 1) % MAX_QUEUE;
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

  pq->queue[pq->head] = 0;

  pq->head = (pq->head + 1) % MAX_QUEUE;

  return ret;
}

int is_empty(struct pqueue *pq) { return pq->head == pq->tail; }
