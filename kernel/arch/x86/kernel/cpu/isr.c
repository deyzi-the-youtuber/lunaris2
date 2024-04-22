#include <stdint.h>
#include <lunaris/printk.h>
#include <common.h>
#include <arch/x86.h>

#include "isr.h"

static const char * exceptions[32] =
{
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
  void (*handler)(struct interrupt_frame *) = isr_handlers[regs->intr]; \
  handler(regs);

void * isr_handlers[256] = {NULL};

void interrupt_install(uint8_t intr, void (*handler)(struct interrupt_frame *))
{
  if(!handler)
    return;
  if(isr_handlers[intr])
    printk("Overwriting interrupt handler %d\n", intr);
  isr_handlers[intr] = handler;
}

int isr_handle_exception(struct interrupt_frame * regs)
{
  if(isr_handlers[regs->intr])
  {
    CALL_ISR_HANDLER(regs->intr);
    pic_eoi(regs->intr);
    return 0;
  }
  pic_eoi(regs->intr);
  return -1;
}

static void code_dump(uint32_t eip)
{
  uint8_t * ptr = (uint8_t *)(eip - 8);
  printk("Code: ");
  for(int i = 1; i < 16; i++)
  {
    if(i != 7)
      printk("%x ", ptr[i]);
    else 
      printk("<%x> ", ptr[i]);
  }
  printk("\n");
}

void register_dump(struct interrupt_frame * regs)
{
  printk("EIP: 0x%x\n", regs->eip);
  code_dump(regs->eip);
  printk("ESP: 0x%x EFLAGS: 0x%x\n", regs->esp, regs->eflags);
  printk("EAX: 0x%x EBX: 0x%x ECX: 0x%x EDX: 0x%x\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
  printk("ESI: 0x%x EDI: 0x%x EBP: 0x%x\n", regs->esi, regs->edi, regs->ebp);
  printk("CS: 0x%x DS: 0x%x SS: 0x%x\n", regs->cs, regs->ds, regs->ss);
  printk("ERR: 0x%x\n", regs->err);
}

void isr_stage2_handler(struct interrupt_frame * regs)
{
  if(regs->intr < 32)
  {
    printk("Fatal exception: %s\n", exceptions[regs->intr]);
    register_dump(regs);
    if (isr_handle_exception(regs) < 0)
    {
      panic("Unhandled exception 0x%x\n", regs->intr);
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
