[BITS 32]
[global madtank]
ALIGN 8
SECTION .text
%macro madtank_bit 1
    %rep 1000-26
    nop
    %endrep
;    mov ax, 0x10 
;    mov ds, ax ;crash
    mov al, 42
    mov edx, %1
    shl edx, 1
    add edx, 0b8000h
    sub edx, 0d000h
    mov [es:edx], al
madtanklabel%1:
    jmp madtanklabel%1
%endmacro


madtank:
%assign i 0
%rep 80
madtank_bit i
%assign i i+1 
%endrep
madtankend:

