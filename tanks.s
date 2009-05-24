[BITS 32]
ALIGN 8
SECTION .text
%macro madtank_bit 1
    %rep 1000
    nop
    %endrep
    mov ax, 10h
    mov es, ax
    mov al, 046h
    mov edx, %1
    shl edx, 1
    add edx, 0b8000h
    mov [es:edx], al
madtanklabel%1:
    jmp madtanklabel%1
%endmacro


madtank:
%assign i 0
%rep 20
madtank_bit i
%assign i i+1 
%endrep
madtankend:

