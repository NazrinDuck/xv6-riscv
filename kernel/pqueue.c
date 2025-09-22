#include "defs.h"
#include "proc.h"
#include "types.h"

void acquire_plock(plock_t *prio) {
  // TODO: l'Optimizer
  while (__atomic_exchange_n(prio, 1, __ATOMIC_ACQUIRE))
    ;
}

void release_plock(plock_t *prio) {
  __atomic_store_n(prio, 0, __ATOMIC_RELEASE);
}

int holding_plock(plock_t *prio) { return *prio == 1; }

static int priority_of(struct proc *proc) {
  return proc->priority.prio + proc->priority.nice;
}

int push_queue(struct pqueue *pq, struct proc *proc) {

  struct proc *pcurr = 0;
  int i, j;

  int priority = priority_of(proc);

  for (i = pq->head; i != pq->tail; i = (i + 1) % MAX_QUEUE) {
    pcurr = pq->queue[i];

    if (!pcurr) {
      break;
    }

    if (priority > priority_of(pcurr)) {
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

struct proc *pop_front_queue(struct pqueue *pq) {
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

struct proc *pop_back_queue(struct pqueue *pq) {
  struct proc *ret = 0;

  if (pq->head == pq->tail) {
    // Queue is empty, fail to pop
    return 0;
  }

  ret = pq->queue[pq->tail - 1];

  pq->queue[pq->tail - 1] = 0;

  pq->tail = (pq->tail - 1) % MAX_QUEUE;

  return ret;
}

struct proc *last_queue(struct pqueue *pq) {
  if (pq->head == pq->tail) {
    // Queue is empty, fail to return the last one
    return 0;
  }

  return pq->queue[(pq->tail - 1) % MAX_QUEUE];
}

int is_empty(struct pqueue *pq) { return pq->head == pq->tail; }

int length_queue(struct pqueue *pq) {
  return (pq->tail - pq->head) % MAX_QUEUE;
}
