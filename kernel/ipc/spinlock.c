#include <stdatomic.h>
#include <lunaris/sync.h>
 
// basically mutexes but without relying on a process scheduler

void spinlock_acquire(spinlock * lock)
{
  while( atomic_flag_test_and_set_explicit(lock, memory_order_acquire))
  {
    __builtin_ia32_pause();
  }
}
 
void spinlock_release(spinlock * lock)
{
  atomic_flag_clear_explicit(lock, memory_order_release);
}
