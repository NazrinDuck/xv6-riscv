#ifndef __TRAP_H
#define __TRAP_H

#define INTERRUPT 0x8000000000000000L

#define S_TIMER_IR (INTERRUPT | 0x5L)
#define S_EXTERNEL_IR (INTERRUPT | 0x9L)

// instruction page fault
#define INS_PAGEFAULT 0xcL
// Load page fault
#define LOAD_PAGEFAULT 0xdL
// Stroe/AMO page fault
#define STORE_PAGEFAULT 0xfL

#endif // !__TRAP_H
