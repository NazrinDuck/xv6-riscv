#include "../kernel/proc.h"
#include "../user/user.h"

#define BIG_NUM 0x13000

int pids[4];

struct pinfo_ex pids_pinfo_ex[4] = {0};

cpuid_t pids_cpu[4] = {CPU0, CPU0, CPU0, CPU0};
cpuid_t pids_cpu_r[4];

int pids_prio_s[4] = {140, 160, 180, 200};
int pids_prio_r[4];

uint64 pids_cnt[4];

int pipe_fds[4][2];

void test_prio() {
  pipe(pipe_fds[0]);
  pids[0] = fork();

  if (pids[0] == 0) {
    // Child 1
    close(pipe_fds[0][0]);

    setpriority(getpid(), pids_prio_s[0]);
    sched_setaffinity(getpid(), pids_cpu[0]);

    for (int i = 0; i < BIG_NUM; ++i) {
      printf("A\r");
    }
    // write(pipe_fds[0][1], "F", 0x1);
    exit(0);
  }
  close(pipe_fds[0][1]);

  pipe(pipe_fds[1]);
  pids[1] = fork();
  if (pids[1] == 0) {
    // Child 2
    close(pipe_fds[1][0]);

    setpriority(getpid(), pids_prio_s[1]);
    sched_setaffinity(getpid(), pids_cpu[1]);

    for (int i = 0; i < BIG_NUM; ++i) {
      printf("B\r");
    }
    // write(pipe_fds[1][1], "F", 0x1);
    exit(0);
  }
  close(pipe_fds[1][1]);

  pipe(pipe_fds[2]);
  pids[2] = fork();
  if (pids[2] == 0) {
    // Child 3
    close(pipe_fds[2][0]);

    setpriority(getpid(), pids_prio_s[2]);
    sched_setaffinity(getpid(), pids_cpu[2]);

    for (int i = 0; i < BIG_NUM; ++i) {
      printf("C\r");
    }
    // write(pipe_fds[2][1], "F", 0x1);
    exit(0);
  }
  close(pipe_fds[2][1]);

  pipe(pipe_fds[3]);
  pids[3] = fork();
  if (pids[3] == 0) {
    // Child 3
    close(pipe_fds[3][0]);

    setpriority(getpid(), pids_prio_s[3]);
    sched_setaffinity(getpid(), pids_cpu[3]);

    for (int i = 0; i < BIG_NUM; ++i) {
      printf("D\r");
    }
    // write(pipe_fds[3][1], "F", 0x1);
    exit(0);
  }
  close(pipe_fds[3][1]);

  return;
}

int main(int argc, char *argv[]) {
  setnbuf(stdout, 0);

  char *banner = "PRIORITY";

  // char buf[0x10];

  if (argc == 2 && strcmp(argv[1], "-b") == 0) {
    banner = "BALANCE";

    for (int i = 0; i < 4; ++i) {
      pids_prio_s[i] = 200;
    }

  } else if (argc == 2 && strcmp(argv[1], "-c") == 0) {
    banner = "VARY CPU";

    for (int i = 0; i < 4; ++i) {
      pids_cpu[i] = CPU(i);
    }

  } else if (argc == 3 && strcmp(argv[1], "-c") == 0 &&
             strcmp(argv[2], "any") == 0) {
    banner = "ANY CPU";

    for (int i = 0; i < 4; ++i) {
      pids_cpu[i] = CPU_ANY;
    }

  } else if (argc == 6 && strcmp(argv[1], "-c") == 0) {
    banner = "GIVEN CPU";

    for (int i = 0; i < 4; ++i) {
      pids_cpu[i] = CPU(argv[i + 2][0] - '0');
    }
  }

  printf("\n");
  printf("[TEST %s] start\n", banner);
  printf("waiting for them\n");
  printf("\n");

  test_prio();

  // pause(280);
  //   After 280 ticks we will get some information

  for (int i = 0; i < 4; ++i) {
    // pids_cnt[i] = sched_getcount(pids[i]);
    pids_prio_r[i] = getpriority(pids[i]);
    pids_cpu_r[i] = sched_getaffinity(pids[i]);
  }

  for (int i = 3; i >= 0; --i) {
    waitid(&pids_pinfo_ex[i], pids[i], P_PID, W_EXTRA);
  }

  for (int i = 0; i < 4; ++i) {
    // read(pipe_fds[i][0], buf, 0x1);
  }

  static char *cpu_names[] = {"CPU0", "CPU1", "CPU2", "CPU3",
                              "CPU4", "CPU5", "CPU6", "CPU7"};
  char *cpu_name;

  printf("\n");
  printf("\n");
  printf("[TEST %s] finished\n", banner);
  printf("PID\tPRIO_R\tPRIO_S\tCPU\tTAT\tWT\tRT\tWT/TAT\n");
  for (int i = 0; i < 4; ++i) {
    if (pids_cpu_r[i] == CPU_ANY) {
      cpu_name = "ANY";
    } else {
      cpu_name = cpu_names[pids_cpu_r[i]];
    }

    printf("%d\t%d\t%d\t%s\t%ld\t%ld\t%ld\t%ld%%\n", pids[i], pids_prio_r[i],
           pids_prio_s[i], cpu_name, pids_pinfo_ex[i].ta_time / 1000,
           pids_pinfo_ex[i].wait_time / 1000,
           (pids_pinfo_ex[i].ta_time - pids_pinfo_ex[i].wait_time) / 1000,
           pids_pinfo_ex[i].wait_time * 100 / pids_pinfo_ex[i].ta_time);
  }
  printf("\n");
  return 0;
}
