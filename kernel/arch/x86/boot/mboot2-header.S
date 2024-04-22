section .text
  %include "include/asm/config.inc"

multiboot_header:
  align 8
  ; Header magic
  dd 0xe85250d6
  ; ISA: i386
  dd 0x0
  ; Header length.
  dd multiboot_header_end - multiboot_header
  ; Header checksum
  dd -(0xe85250d6 + 0x0 + (multiboot_header_end - multiboot_header))
%if CONFIG_FB == 1
multiboot_fb_tag_start:
  dw 5
  dw 1
  dd multiboot_fb_tag_end - multiboot_fb_tag_start
  dd 800
  dd 600
  dd 32
multiboot_fb_tag_end:
%endif
multiboot_header_tag_end:
  dw 0
  dw 0
  dd 0
multiboot_header_end:
