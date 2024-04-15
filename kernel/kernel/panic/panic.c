#include <lunaris/printk.h>
#include <common.h>
#include <stdarg.h>

static void die()
{
  asm volatile("cli");
  asm volatile("hlt");
}

void panic(const char fmt[], ...)
{
  va_list ap;
  va_start(ap, fmt);
  vsprintf(strcat("Kernel panic: ", fmt), ap);
  va_end(ap);
  die();
  __builtin_unreachable();
}
