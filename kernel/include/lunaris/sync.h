#ifndef __SYNC_H
#define __SYNC_H

#include <stdint.h>
#include <stdatomic.h>

struct mutex
{
  uint8_t locked : 1;
};

typedef atomic_flag spinlock;

#define DEFINE_SPINLOCK(name) static spinlock name = ATOMIC_FLAG_INIT;

void spinlock_acquire(spinlock * lock);
void spinlock_release(spinlock * lock);

#endif
