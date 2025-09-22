#include "../kernel/proc.h"
#include "../user/user.h"

#define BIG_NUM 0x8000

#define PROC_NUM 10

int pids[PROC_NUM];

struct pinfo_ex pids_pinfo_ex[PROC_NUM];

cpuid_t pids_cpu[PROC_NUM];
cpuid_t pids_cpu_r[PROC_NUM];

int pids_prio_s[PROC_NUM];
int pids_prio_r[PROC_NUM];

int pipe_fd[2];

void test_dona() {
  pipe(pipe_fd);

  pids[0] = fork();
  if (pids[0] == 0) {
    close(pipe_fd[0]);
    // Child 0
    setpriority(getpid(), pids_prio_s[0]);
    sched_setaffinity(getpid(), pids_cpu[0]);

    for (int j = 0; j < BIG_NUM; ++j) {
      printf("%c\r", 'A');
    }

    for (int k = 0; k < PROC_NUM - 1; ++k) {
      write(pipe_fd[1], "Finish", 0x6);
    }
    exit(0);
  }

  for (int i = 1; i < PROC_NUM; ++i) {
    pids[i] = fork();
    if (pids[i] == 0) {
      // Child i

      setpriority(getpid(), pids_prio_s[i]);
      sched_setaffinity(getpid(), pids_cpu[i]);

      for (int j = 0; j < BIG_NUM; ++j) {
        printf("%c\r", 'A' + i);
      }
      char buf[0x6];
      read(pipe_fd[0], buf, 0x6);
      exit(0);
    }
  }

  return;
}

int main(int argc, char *argv[]) {
  setnbuf(stdout, 0);

  char *banner = "DONATION on CPU1";

  for (int i = 0; i < PROC_NUM; ++i) {
    pids_prio_s[i] = 60 + 20 * i;
  }

  if (argc == 3 && strcmp(argv[1], "-c") == 0 && strcmp(argv[2], "any") == 0) {
    banner = "DONATION on CPU<ANY>";

    for (int i = 0; i < PROC_NUM; ++i) {
      pids_cpu[i] = CPU_ANY;
    }
  } else {
    for (int i = 0; i < PROC_NUM; ++i) {
      pids_cpu[i] = CPU1;
    }
  }

  printf("\n");
  printf("[TEST %s] start\n", banner);
  printf("waiting for them\n");
  printf("\n");

  setpriority(getpid(), 255);
  test_dona();

  // Wait for setting
  pause(30);

  for (int i = 0; i < PROC_NUM; ++i) {
    pids_prio_r[i] = getpriority(pids[i]);
    pids_cpu_r[i] = sched_getaffinity(pids[i]);
  }

  for (int i = PROC_NUM - 1; i >= 0; --i) {
    waitid(&pids_pinfo_ex[i], pids[i], P_PID, W_EXTRA);
  }

  static char *cpu_names[] = {"CPU0", "CPU1", "CPU2", "CPU3",
                              "CPU4", "CPU5", "CPU6", "CPU7"};
  char *cpu_name;

  printf("\n");
  printf("\n");
  printf("[TEST %s] finished\n", banner);
  printf("PID\tPRIO_R\tPRIO_S\tCPU\tTAT\tWT\tRT\tWT/TAT\n");
  for (int i = 0; i < PROC_NUM; ++i) {
    if (pids_cpu_r[i] == CPU_ANY) {
      cpu_name = "ANY";
    } else {
      cpu_name = cpu_names[pids_cpu_r[i]];
    }

    printf("%d\t%d\t%d\t%s\t%ld\t%ld\t%ld\t%ld%%\n", pids[i], pids_prio_r[i],
           pids_prio_s[i], cpu_name, pids_pinfo_ex[i].ta_time / 1000,
           pids_pinfo_ex[i].wait_time / 1000,
           (pids_pinfo_ex[i].ta_time - pids_pinfo_ex[i].wait_time) / 1000,
           PERCENTAGE(pids_pinfo_ex[i].wait_time, pids_pinfo_ex[i].ta_time));
  }
  printf("\n");
  return 0;
}
