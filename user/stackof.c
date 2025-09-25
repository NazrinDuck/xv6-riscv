#include "./user.h"

int overflow(int n) {
  uint64 stack_val = 0;
  printf("%d: %p\n", n, &stack_val);

  if (n > 0x0fffffff) {
    return 0;
  }
  return overflow(n + 1);
}

int main() {
  printf("func: %p\n", overflow);
  printf("main: %p\n", main);
  overflow(0x0);
  return 0;
}
