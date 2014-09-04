#ifndef BITS_TIMES_H
#define BITS_TIMES_H

#include <bits/types.h>

struct tms {
  clock_t tms_utime;
  clock_t tms_stime;
  clock_t tms_cutime;
  clock_t tms_cstime;
};

#endif /* BITS_TIMES_H */
