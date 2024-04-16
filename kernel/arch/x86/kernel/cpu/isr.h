#ifndef __ISR_H
#define __ISR_H

extern void isr_entry_0();
extern void isr_entry_1();
extern void isr_entry_2();
extern void isr_entry_3();
extern void isr_entry_4();
extern void isr_entry_5();
extern void isr_entry_6();
extern void isr_entry_7();
extern void isr_entry_8();
extern void isr_entry_9();
extern void isr_entry_10();
extern void isr_entry_11();
extern void isr_entry_12();
extern void isr_entry_13();
extern void isr_entry_14();
extern void isr_entry_15();
extern void isr_entry_16();
extern void isr_entry_17();
extern void isr_entry_18();
extern void isr_entry_19();
extern void isr_entry_20();
extern void isr_entry_21();
extern void isr_entry_22();
extern void isr_entry_23();
extern void isr_entry_24();
extern void isr_entry_25();
extern void isr_entry_26();
extern void isr_entry_27();
extern void isr_entry_28();
extern void isr_entry_29();
extern void isr_entry_30();
extern void isr_entry_31();
extern void isr_entry_32();
extern void isr_entry_33();
extern void isr_entry_34();
extern void isr_entry_35();
extern void isr_entry_36();
extern void isr_entry_37();
extern void isr_entry_38();
extern void isr_entry_39();
extern void isr_entry_40();
extern void isr_entry_41();
extern void isr_entry_42();
extern void isr_entry_128();

struct interrupt_frame
{
  uint32_t ds;
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; 
  uint32_t intr, err;
  uint32_t eip, cs, eflags, useresp, ss;
};

#endif
