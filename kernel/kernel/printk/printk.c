#include <sys/types.h>
#include <stdarg.h>
#include <lunaris/video.h>
#include <common.h>

static void __kmsg_append()
{

}

ssize_t printk(const char fmt[], ...)
{
  va_list arg;
  va_start(arg, fmt);
  ssize_t r = vsprintf(fmt, arg);
  va_end(arg);
  return r;
}
