#ifndef __X86_H
#define __X86_H

#include <stdint.h>

#define EFLAGS_INTERRUPTS (1 << 9) // interrupt flag
#define EFLAGS_CPUID (1 << 21) // cpuid supported

struct interrupt_frame
{
  uint32_t ds;
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; 
  uint32_t intr, err;
  uint32_t eip, cs, eflags, useresp, ss;
};

void interrupt_install(uint8_t intr, void (*handler)(struct interrupt_frame *));
void interrupts_enable(void);
void interrupts_disable(void);
int cpu_get_interrupt_flag(void);
uint32_t cpu_get_eflags(void);

void pic_eoi(uint8_t irq);

#endif
