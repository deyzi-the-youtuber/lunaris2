#ifndef __MUTEX_H
#define __MUTEX_H

struct mutex
{
  uint8_t locked : 1;
};

#endif
