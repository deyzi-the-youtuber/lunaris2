section .text
  global cpu_flush_idt

cpu_flush_idt:
  mov eax, [esp + 4]
  lidt [eax]
  sti
  ret
