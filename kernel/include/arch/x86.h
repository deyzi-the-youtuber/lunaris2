#ifndef __X86_H
#define __X86_H

#include <stdint.h>

#define EFLAGS_INTERRUPTS (1 << 9) // interrupt flag
#define EFLAGS_CPUID (1 << 21) // cpuid supported

void interrupts_enable(void);
void interrupts_disable(void);
int cpu_get_interrupt_flag(void);
uint32_t cpu_get_eflags(void);

#endif
