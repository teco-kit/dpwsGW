#ifndef __TIMEVAL_H__
#define __TIMEVAL_H__ 1
#include <stdint.h>
struct timeval
{
  uint32_t tv_sec;
  uint32_t tv_usec;
};
#endif
