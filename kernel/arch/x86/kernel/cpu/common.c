#include <arch.h>
#include <arch/x86.h>
#include <stdint.h>
#include <common.h>
#include <sys/io.h>

uint32_t cpu_get_eflags(void)
{
  uint32_t eflags;
  asm volatile("pushf");
  asm volatile("pop %0" : "=r"(eflags));
  return eflags;
}

void interrupts_enable(void)
{
  asm volatile("sti");
}

void interrupts_disable(void)
{
  asm volatile("cli");
}

int cpu_get_interrupt_flag(void)
{
  return cpu_get_eflags() & EFLAGS_INTERRUPTS;
}

void outb(uint16_t port, uint8_t data)
{
  asm volatile ("outb %0, %1" :: "a"(data), "Nd"(port) : "memory");
}

uint8_t inb(uint16_t port)
{
  uint8_t data;
  asm volatile("inb %1, %0" : "=a"(data) : "Nd"(port) : "memory");
  return data;
}

void outw(uint16_t port, uint16_t data)
{
  asm volatile ("outw %0, %1" :: "a"(data), "Nd"(port) : "memory");
}

uint16_t inw(uint16_t port)
{
  uint16_t data;
  asm volatile("inw %1, %0" : "=a"(data) : "Nd"(port) : "memory");
  return data;
}

void outl(uint16_t port, uint32_t data)
{
  asm volatile ("outl %0, %1" :: "a"(data), "Nd"(port) : "memory");
}

uint32_t inl(uint16_t port)
{
  uint32_t data;
  asm volatile("inl %1, %0" : "=a"(data) : "Nd"(port) : "memory");
  return data;
}

void io_wait(void)
{
  outb(0x80, 0);
}

void pic_eoi(uint8_t irq)
{
	if(irq >= 8)
		outb(0xa0, 0x20);
	outb(0x20, 0x20);
}
