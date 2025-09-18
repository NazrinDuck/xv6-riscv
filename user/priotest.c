#include "../user/user.h"

int main(void) {

  if (fork() == 0) {
    // Child
    setpriority(getpid(), 40);

    for (int i = 0;; ++i) {
      printf("AAA\n");
    }
  } else {
    setpriority(getpid(), 80);

    for (int i = 0;; ++i) {
      printf("BBB\n");
    }
  };

  return 0;
}
