#include "./types.h"
#include "riscv.h"

time_t get_cycle() { return r_time(); }
