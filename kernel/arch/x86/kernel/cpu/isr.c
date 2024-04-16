#include <stdint.h>
#include <lunaris/printk.h>
#include <common.h>
#include "isr.h"
#include <arch/x86.h>

static const char * exceptions[32] = {
  "Divide Error",
  "Debug",
  "NMI Interrupt",
  "Breakpoint",
  "Overflow",
  "BOUND Range Exceeded",
  "Invalid Opcode",
  "Device Not Available",
  "Double Fault",
  "Coprocessor Segment Overrun",
  "Invalid TSS",
  "Segment Not Present",
  "Stack Segment Fault", 
  "General Protection Fault", 
  "Page Fault",
  "Reserved",
  "Floating-Point Error",
  "Alignment Check",
  "Machine Check",
  "SIMD Floating-Point Exception",
  "Virtualization Exception",
  "Control Protection Exception",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved"
};

#define CALL_ISR_HANDLER(interrupt) \
  void (*handler)(struct interrupt_frame * regs) = isr_handlers[regs->intr]; \
  handler(regs);

void * isr_handlers[256] = {NULL};

int isr_handle_exception(struct interrupt_frame * regs)
{
  if(isr_handlers[regs->intr])
  {
    CALL_ISR_HANDLER(regs->intr);
    return 0;
  }
  pic_eoi(regs->intr);
  return -1;
}

void isr_stage2_handler(struct interrupt_frame * regs)
{
  if(regs->intr < 32)
  {
    printk("Fatal exception: %s\n", exceptions[regs->intr]);
    if (isr_handle_exception(regs) < 0)
    {
      printk("Unhandled exception 0x%x\n", regs->intr);
      interrupts_disable();
      for(;;);
    }
  }
  else
  {
    if(isr_handlers[regs->intr])
    {
      CALL_ISR_HANDLER(regs->intr);
    }
    pic_eoi(regs->intr);
  }
}
