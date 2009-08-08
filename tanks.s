[BITS 32]
[global madtank]
ALIGN 8
SECTION .text
%macro madtank_bit 1
    %rep 100h-26+12
    nop
    %endrep
    mov al, 42
    mov edx, %1
    shl edx, 1
    mov [gs:edx], al
madtanklabel%1:
    jmp madtanklabel%1
%endmacro

%macro madtank_gp 1
    %rep 100h-25+12
    nop
    %endrep
    mov ax, 0x10 
    mov ds, ax ;crash
    shl edx, 1
    mov [gs:edx], al
madtanklabel%1:
    jmp madtanklabel%1
%endmacro

%macro madtank_div0 1
    %rep 100h-25+12-26
    nop
    %endrep
    mov bl,0
    div bl ;crash
    mov ds, ax 
    shl edx, 1
    mov [gs:edx], al
madtanklabel%1:
    jmp madtanklabel%1
%endmacro

%macro madtank_int1 1
    %rep 100h-27+12
    nop
    %endrep
    int 1
    mov ax, 0x10 
    mov ds, ax ;crash
    shl edx, 1
    mov [gs:edx], al
madtanklabel%1:
    jmp madtanklabel%1
%endmacro

madtank:
madtank_div0 0
madtank_int1 1
madtank_gp 2
%assign i 3
%rep 4dh
madtank_bit i
%assign i i+1 
%endrep
madtankend:

