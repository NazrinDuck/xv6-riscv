#define NPROC 64       // maximum number of processes
#define NCPU 8         // maximum number of CPUs
#define NOFILE 16      // open files per process
#define NFILE 100      // open files per system
#define NCHAN (64)     // maximum number of channels
#define NINODE 50      // maximum number of active i-nodes
#define NDEV 10        // maximum major device number
#define ROOTDEV 1      // device number of file system root disk
#define MAXARG 32      // max exec arguments
#define MAXOPBLOCKS 10 // max # of blocks any FS op writes
#define MAX_PIDS (65536)
#define LOGBLOCKS (MAXOPBLOCKS * 3) // max data blocks in on-disk log
#define NBUF (MAXOPBLOCKS * 3)      // size of disk block cache
#define FSSIZE 2000                 // size of file system in blocks
#define MAXPATH 128                 // maximum file path name
#define USERSTACK 1                 // user stack pages

// maximum numbers of process queue
#define MAX_QUEUE (NPROC)

// If a process waiting time divided turn-around time more than
// this value, we will incresse it's priority
#define AGING_RATIO (95)
#define AGING_WAIT_TIME (100000 * 1000)

// Macros about CPUs
#define CPU_ANY (-100)

#define CPU(x) ((cpuid_t)(x > (NCPU - 1) ? (NCPU - 1) : x))

#define CPU0 ((cpuid_t)0)
#define CPU1 ((cpuid_t)1)
#define CPU2 ((cpuid_t)2)
#define CPU3 ((cpuid_t)3)
#define CPU4 ((cpuid_t)4)
#define CPU5 ((cpuid_t)5)
#define CPU6 ((cpuid_t)6)
#define CPU7 ((cpuid_t)7)
