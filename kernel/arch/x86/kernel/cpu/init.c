#include <arch.h>
#include <arch/x86.h>
#include <stdint.h>
#include "isr.h"
#include <sys/io.h>

static void __x86_arch_init();
static void __x86_gdt_init();
static void __x86_idt_init();
static void __x86_pic_remap();
extern void cpu_flush_gdt(uintptr_t gdtr);
extern void cpu_flush_idt(uintptr_t idtr);

void arch_init(void)
{
  __x86_arch_init();
}

static void __x86_arch_init()
{
  __x86_gdt_init();
  __x86_idt_init();
}

// gdt

struct segm_descriptor
{
  // x86 CPUs ignore the bases
  uint16_t  limit;
  uint16_t  base_low;
  uint8_t   base_mid;
  uint8_t   access;
  uint8_t   flags;
  uint8_t   base_high;
}__attribute__((packed));

struct gdtr
{
  uint16_t  limit;
  uintptr_t addr;
}__attribute__((packed));

// helping macros

#define DEFINE_SEGM_DESC(base, limit, access, flags) \
  (limit & 0xffff), \
  (base & 0xffff), \
  ((base >> 16) & 0xff), \
  (access), \
  (((limit >> 16) & 0x0f) | (flags & 0xf0)), \
  ((base >> 24) & 0xff)


struct segm_descriptor gdt[] = {
  // null segment descriptor
  DEFINE_SEGM_DESC(0, 0, 0, 0),
  // kernel code segment descriptor (32-bit)
  DEFINE_SEGM_DESC(0, 0xffff, 0x9a, 0xcf),
  // kernel data segment descriptor (32-bit)
  DEFINE_SEGM_DESC(0, 0xffff, 0x92, 0xcf),
  // user code segment descriptor (32-bit) 
  DEFINE_SEGM_DESC(0, 0xffff, 0xfa, 0xcf),
  // user data segment descriptor (32-bit)
  DEFINE_SEGM_DESC(0, 0xffff, 0xf2, 0xcf)
};

struct gdtr _gdtr = {
  .limit  = sizeof(gdt) - 1,
  .addr   = (uintptr_t)&gdt
};

static void __x86_gdt_init()
{
  cpu_flush_gdt((uintptr_t)&_gdtr);
}

// idt

struct idt_vector
{
  uint16_t  base_low;
  uint16_t  segm_sel;
  uint8_t   zero;
  uint8_t   flags;
  uint16_t  base_high;
}__attribute__((packed));

struct idtr 
{
  uint16_t  limit;
  uintptr_t addr;
}__attribute__((packed));

struct idt_vector idt[256];

struct idtr _idtr = {
  .limit = sizeof(idt) - 1,
  .addr = (uintptr_t)&idt
};

// macros dont really work with a structure list
// so ill use functions
static void __x86_define_idt_vector(int vector, uintptr_t base, uint8_t flags)
{
  idt[vector].base_low = base & 0xffff;
  idt[vector].segm_sel = 0x08;
  idt[vector].zero = 0;
  idt[vector].flags = flags | 0x60;
  idt[vector].base_high = (base >> 16) & 0xffff;
}

static uintptr_t isr_entries[43] = {
  (uintptr_t)isr_entry_0,
  (uintptr_t)isr_entry_1,
  (uintptr_t)isr_entry_2,
  (uintptr_t)isr_entry_3,
  (uintptr_t)isr_entry_4,
  (uintptr_t)isr_entry_5,
  (uintptr_t)isr_entry_6,
  (uintptr_t)isr_entry_7,
  (uintptr_t)isr_entry_8,
  (uintptr_t)isr_entry_9,
  (uintptr_t)isr_entry_10,
  (uintptr_t)isr_entry_11,
  (uintptr_t)isr_entry_12,
  (uintptr_t)isr_entry_13,
  (uintptr_t)isr_entry_14,
  (uintptr_t)isr_entry_15,
  (uintptr_t)isr_entry_16,
  (uintptr_t)isr_entry_17,
  (uintptr_t)isr_entry_18,
  (uintptr_t)isr_entry_19,
  (uintptr_t)isr_entry_20,
  (uintptr_t)isr_entry_21,
  (uintptr_t)isr_entry_22,
  (uintptr_t)isr_entry_23,
  (uintptr_t)isr_entry_24,
  (uintptr_t)isr_entry_25,
  (uintptr_t)isr_entry_26,
  (uintptr_t)isr_entry_27,
  (uintptr_t)isr_entry_28,
  (uintptr_t)isr_entry_29,
  (uintptr_t)isr_entry_30,
  (uintptr_t)isr_entry_31,
  (uintptr_t)isr_entry_32,
  (uintptr_t)isr_entry_33,
  (uintptr_t)isr_entry_34,
  (uintptr_t)isr_entry_35,
  (uintptr_t)isr_entry_36,
  (uintptr_t)isr_entry_37,
  (uintptr_t)isr_entry_38,
  (uintptr_t)isr_entry_39,
  (uintptr_t)isr_entry_40,
  (uintptr_t)isr_entry_41,
  (uintptr_t)isr_entry_42
};

static void __x86_idt_init(void)
{
  __x86_pic_remap(0x20, 0x28);
  for(int i = 0; i < 42; i++)
  {
    // all of these are interrupts
    // no traps
    __x86_define_idt_vector(i, isr_entries[i], 0x8e);
  }
  __x86_define_idt_vector(0x80, (uintptr_t)isr_entry_128, 0x8e);
  cpu_flush_idt((uintptr_t)&_idtr);
}

// i8259 (PIC 8259)

#define PIC_MASTER 0x20
#define PIC_SLAVE 0xa0
#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA	0x21
#define PIC_SLAVE_COMMAND	0xa0
#define PIC_SLAVE_DATA	0xa1
// PIC commands
#define PIC_EOI	0x20
#define ICW1_ICW4	0x01
#define ICW1_SINGLE	0x02
#define ICW1_INTERVAL4	0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT	0x10
 
#define ICW4_8086	0x01
#define ICW4_AUTO	0x02
#define ICW4_BUF_SLAVE 0x08
#define ICW4_BUF_MASTER	0x0C
#define ICW4_SFNM	0x10

static void __x86_pic_slave_send_command(uint8_t cmd)
{
  outb(PIC_SLAVE_COMMAND, cmd);
}

static void __x86_pic_master_send_command(uint8_t cmd)
{
  outb(PIC_MASTER_COMMAND, cmd);
}

static void __x86_pic_slave_send_data(uint8_t data)
{
  outb(PIC_SLAVE_DATA, data);
}

static void __x86_pic_master_send_data(uint8_t data)
{
  outb(PIC_MASTER_DATA, data);
}

static void __x86_pic_remap(int offset1, int offset2)
{
	uint8_t a1, a2;
 
	a1 = inb(PIC_MASTER_DATA);                        // save masks
	a2 = inb(PIC_SLAVE_DATA);
 
	__x86_pic_master_send_command(ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	__x86_pic_slave_send_command(ICW1_INIT | ICW1_ICW4);
	io_wait();
	__x86_pic_master_send_data(offset1);                 // ICW2: Master PIC vector offset
	io_wait();
	__x86_pic_slave_send_data(offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	__x86_pic_master_send_data(4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	__x86_pic_slave_send_data(2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	__x86_pic_master_send_data(ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	io_wait();
	__x86_pic_slave_send_data(ICW4_8086);
	io_wait();
 
	__x86_pic_master_send_data(a1);   // restore saved masks.
	__x86_pic_slave_send_data(a2);
}
