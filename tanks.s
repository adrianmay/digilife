[BITS 32]
[global madtank]
[global histogram]
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
    %rep 100h-25+12
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

%macro madtank_ok 1
    %rep 0feh
    nop
    %endrep
madtanklabel%1:
    jmp madtanklabel%1
%endmacro

%macro madtank_overflow 1
    %rep 100h-27+17
    nop
    %endrep
	mov al,-128
	dec al
    into
	nop
madtanklabel%1:
    jmp madtanklabel%1
%endmacro

%macro madtank_bounds 1
    %rep 100h-27+13
    nop
    %endrep
	;mov ebx, [bounds]
	mov eax,20
	bound eax, [bounds]
	nop
madtanklabel%1:
    jmp madtanklabel%1
%endmacro

%assign j 85
%macro madtank_123 1
    %rep 0FEh
    db 6Dh
	%assign j j+1
    %endrep
madtanklabel%1:
    jmp madtanklabel%1
%endmacro

madtank:
%assign i 0
%rep 40h
madtank_div0 i
%assign i i+1 
madtank_bounds i
%assign i i+1 
madtank_overflow i
%assign i i+1 
madtank_123 i
%assign i i+1 
%endrep
madtankend:

SECTION .data
bounds:
; dd 5
; dd 10
 
; histogram:
