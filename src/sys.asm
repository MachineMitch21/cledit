%ifndef SYS_ASM
%define SYS_ASM

%include "defines.asm"

; Uses sys_write system call 
; param #1 -->  fd to write to
; param #2 -->  str to write to fd
; param #3 -->  size of the str
%macro _WRITE 3
  mov eax, SYS_WRITE
  mov ebx, %1
  mov ecx, %2
  mov edx, %3
%endmacro

; expands _WRITE macro with STDOUT fd
; param #1 -->  str to write
; param #2 -->  size of the str
%macro _WRITE_STDOUT 2
  _WRITE STDOUT, %1, %2
%endmacro

; expands _WRITE macro with STDERR fd
; param #1 --> str to write
; param #2 --> size of the str
%macro _WRITE_STDERR 2
  _WRITE STDERR, %1, %2
%endmacro

; reads from fd
; param #1 -->  fd to read from
; param #2 -->  buffer to read into
; param #3 -->  size of buffer
%macro _READ 3
  mov eax, SYS_READ
  mov ebx, %1
  mov ecx, %2
  mov edx, %3
%endmacro

%endif 