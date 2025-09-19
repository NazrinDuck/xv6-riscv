#include "../kernel/proc.h"
#include "./user.h"

int wait(int *xstate) { return waitid(xstate, 0, P_ANY, W_NORMAL); }

int waitpid(int *xstate, pid_t pid) {
  return waitid(xstate, pid, P_PID, W_NORMAL);
}
