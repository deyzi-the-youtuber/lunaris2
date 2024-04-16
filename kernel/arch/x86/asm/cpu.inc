%macro DEFINE_ISR 2
global isr_entry_%1
isr_entry_%1:
  cli
%if %2 == 1
  push byte 0
%endif
  push %1
  jmp isr_stage1_handler
%endmacro
