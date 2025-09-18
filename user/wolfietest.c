#include "../kernel/types.h"

#include "../kernel/stat.h"
#include "../user/user.h"

#include "../kernel/wolfie.h"

char buffer[WOLFIE_SZ];

int test_ok() {
  int ret = wolfie(buffer, WOLFIE_SZ);
  switch (ret) {
  case -1:
    fprintf(stderr, "wolfie: fail to copy\n");
    return -1;
  case -2:
    fprintf(stderr, "wolfie: size too small\n");
    return -1;
  default:
    printf("wolfie return %d\n", ret);
    break;
  };

  printf("%s\n", buffer);

  if (ret == WOLFIE_SZ) {
    return 0;
  } else {
    return -1;
  }
}

int test_small() {
  int ret = wolfie(buffer, 0x100);
  switch (ret) {
  case -1:
    fprintf(stderr, "wolfie: fail to copy\n");
    break;
  case -2:
    fprintf(stderr, "wolfie: size too small\n");
    break;
  default:
    printf("wolfie return %d\n", ret);
    break;
  };

  return ret;
}

int test_part() {
  uint size = WOLFIE_SZ / 2;
  int ret = wolfie(buffer, size);
  switch (ret) {
  case -1:
    fprintf(stderr, "wolfie: fail to copy\n");
    return -1;
  case -2:
    fprintf(stderr, "wolfie: size too small\n");
    return -1;
  default:
    printf("wolfie return %d\n", ret);
    break;
  };

  printf("%s\n", buffer);

  if (ret == size) {
    return 0;
  } else {
    return -1;
  }
}

int main(int argc, char *argv[]) {

  printf("[1] test_ok START\n");
  if (test_ok() == 0) {
    printf("[1] test_ok OK\n");
  } else {
    printf("[1] test_ok FAIL\n");
  }

  memset(buffer, 0, WOLFIE_SZ);

  printf("[2] test_small START\n");
  if (test_small() == -2) {
    printf("[2] test_small OK\n");
  } else {
    printf("[2] test_small FAIL\n");
  }

  memset(buffer, 0, WOLFIE_SZ);

  printf("[3] test_part START\n");
  if (test_part() == 0) {
    printf("[3] test_part OK\n");
  } else {
    printf("[3] test_part FAIL\n");
  }

  return 0;
}
