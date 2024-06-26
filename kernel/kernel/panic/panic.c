#include <lunaris/printk.h>
#include <arch/x86.h>
#include <common.h>
#include <stdarg.h>

static void die()
{
  interrupts_disable();
  asm volatile("hlt");
}

void panic(const char fmt[], ...)
{
  va_list ap;
  va_start(ap, fmt);
  vprintf(strcat("panic: ", fmt), ap);
  va_end(ap);
  die();
  __builtin_unreachable();
}
