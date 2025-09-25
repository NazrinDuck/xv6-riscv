#include "./user.h"

int main() {
  printf("main: %p\n", main);
  printf("[TEST Null Pointer]\n");
  uint64 *ptr = 0;
  printf("Ptr: %p\n", ptr);
  uint64 val = *ptr;
  printf("Val: %ld\n", val);
  return 0;
}
