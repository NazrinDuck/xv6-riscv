#include "types.h"

#include "defs.h"
#include "memlayout.h"
#include "param.h"

#include "proc.h"
#include "vm.h"

extern struct proc proc[];

uint64 sys_exit(void) {
  int n;
  argint(0, &n);
  kexit(n);
  __builtin_unreachable();
  return 0; // not reached
}

uint64 sys_getpid(void) { return myproc()->pid; }

uint64 sys_fork(void) { return kfork(); }

/*
uint64 sys_wait(void) {
  uint64 p;
  argaddr(0, &p);
  return kwait(p, 0, P_ANY, W_NORMAL);
}
*/

uint64 sys_waitid(void) {
  uint64 addr;
  pid_t wpid;
  enum wait_mode mode;
  enum wait_option option;

  argaddr(0, &addr);
  argint(1, &wpid);
  argint(2, (int *)&mode);
  argint(3, (int *)&option);

  return kwait(addr, wpid, mode, option);
}

uint64 sys_sbrk(void) {
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64 sys_pause(void) {
  int n;
  tick_t ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64 sys_nanosleep(void) {
  time_t n;
  arguint64(0, &n);

  // TODO: Venez l'implemer
  panic("unimplement");
  return 0;
}

uint64 sys_kill(void) {
  pid_t pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64 sys_uptime(void) {
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// uint8 sys_getpriority(int)
//
// required arg `pid`
//
// return the priority of given pid's process
//
// return -1 if process not found
uint64 sys_getpriority(void) {
  struct proc *p;
  pid_t pid;

  argint(0, &pid);

  if (pid < 0 || pid > MAX_PIDS) {
    return -1;
  }

  for (p = proc; p < &proc[NPROC]; p++) {
    if (p->pid == pid) {
      return p->priority.prio;
    }
  }

  return -1;
}

// int sys_setpriority(int, int8)
//
// required arg `pid` and `value`
//
// set the priority of given pid's process to `value`
//
// return 0 if success
//
// return -1 if process not found
uint64 sys_setpriority(void) {
  struct proc *p;
  pid_t pid;
  uint8 value;

  argint(0, &pid);
  arguint8(1, &value);

  if (pid < 0 || pid > MAX_PIDS) {
    return -1;
  }

  for (p = proc; p < &proc[NPROC]; p++) {
    if (p->pid == pid) {
      if (!holding_plock(&p->priority.lock) && !p->priority.is_swap) {
        p->priority.prio = value;
      } else {
        return -2;
      }
      return 0;
    }
  }

  return -1;
}

// cpuid_t sched_getaffinity(pid_t)
//
// required arg `pid`
//
// get the CPU affinity of given pid's process
//
// return cpuid if success
//
// return -1 if process not found
uint64 sys_sched_getaffinity(void) {
  struct proc *p;
  pid_t pid;

  argint(0, &pid);

  if (pid < 0 || pid > MAX_PIDS) {
    return -1;
  }

  for (p = proc; p < &proc[NPROC]; p++) {
    if (p->pid == pid) {
      return p->pcpu_info.affinity;
    }
  }

  return -1;
}

// int sched_setaffinity(pid_t, cpuid_t)
//
// required arg `pid` and `cpuid`
//
// set the CPU affinity of given pid's process to `cpuid`
//
// return 0 if success
//
// return -1 if process not found
uint64 sys_sched_setaffinity(void) {
  struct proc *p;
  pid_t pid;
  cpuid_t cpuid;

  argint(0, &pid);
  argint(1, &cpuid);

  if (pid < 0 || pid > MAX_PIDS || cpuid < 0 || cpuid > NCPU) {
    return -1;
  }

  for (p = proc; p < &proc[NPROC]; p++) {
    if (p->pid == pid) {
      p->pcpu_info.affinity = cpuid;
      return 0;
    }
  }

  return -1;
}

/*
// uint64 sched_gettickinfo(pid_t)
//
// required arg `pid` and `struct *tick_info`
//
// return 0 if success
//
// return -1 if process not found
uint64 sys_gettickinfo(void) {
  struct proc *p;
  struct proc *curr = myproc();

  pid_t pid;
  uint64 tick_info_addr;

  argint(0, &pid);
  argaddr(1, &tick_info_addr);

  if (pid < 0 || pid > MAX_PIDS || tick_info_addr == 0) {
    return -1;
  }

  for (p = proc; p < &proc[NPROC]; p++) {
    if (p->pid == pid) {
      return copyout(curr->pagetable, tick_info_addr, (void *)&p->time_info,
                     sizeof(struct time_info));
    }
  }

  return -1;
}
*/
