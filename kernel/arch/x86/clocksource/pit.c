#include <stdint.h>
#include <arch.h>
#include <arch/x86.h>
#include <sys/io.h>

volatile uint32_t jiffies = 0;

static void __pit_configure_hz(uint16_t hz)
{ 
  int divisor = 1193180 / hz;  
  outb(0x43, 0x36); 
  outb(0x40, divisor & 0xFF);
  outb(0x40, divisor >> 8);
}

void __timer_irq(struct interrupt_frame * regs)
{
  jiffies++;
}

void sleep(uint32_t ms)
{
  uint32_t start_jiffies = jiffies;
  while ((jiffies - start_jiffies) * 10 < ms);
}

void pit_init(void)
{
  __pit_configure_hz(1000);
  interrupt_install(0x20, __timer_irq);
}
