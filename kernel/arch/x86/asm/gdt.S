section .text
  global cpu_flush_gdt

cpu_flush_gdt:
  mov eax, [esp + 4]
  lgdt [eax]

segm_reload:
  jmp 0x08:.reload
.reload:
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  ret
