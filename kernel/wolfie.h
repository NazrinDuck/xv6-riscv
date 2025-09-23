#ifndef __WOLFIE_H
#define __WOLFIE_H

#include "./wolfie.c"

#define WOLFIE_SZ                                                              \
  (sizeof(wolfie_buf) - 1) // used in `sys_wolfie()`, image's size
#define WOLFIE_MIN (WOLFIE_SZ / 10)

#endif // __WOLFIE_H
