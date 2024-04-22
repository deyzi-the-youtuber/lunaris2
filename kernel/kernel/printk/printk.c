#include <sys/types.h>
#include <stdarg.h>
#include <lunaris/video.h>
#include <common.h>
#include <lunaris/sync.h>

DEFINE_SPINLOCK(s)

ssize_t printk(const char fmt[], ...)
{
  spinlock_acquire(&s);
  va_list arg;
  va_start(arg, fmt);
  ssize_t r = vprintf(fmt, arg);
  va_end(arg);
  spinlock_release(&s);
  return r;
}
