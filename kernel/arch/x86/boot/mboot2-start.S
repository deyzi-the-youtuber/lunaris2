section .text
  global _kentry
  extern kernel_main

%define STACK_SIZE 0x400

_kentry:
  mov esp, stack_top
  push 0
  popfd
  ; check if bootloader magic is valid
  cmp eax, 0x36d76289
  jne .halt
  push ebx
  call kernel_main
.halt:
  cli
  hlt

section .bss
stack_bottom: 
  resb STACK_SIZE ; kernel stack
stack_top:
