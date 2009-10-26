[BITS 32]
[global madtank]
[global histogram]
ALIGN 1
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
    %rep 100h-25+11 
    nop
    %endrep
    mov ax, 0x10 
    mov ds, ax ;crash
    mov ax, 0x20 
    mov ds, ax ;crash
	
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
    nop
	nop
    nop
	nop
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

%macro madtank_jmp 1
    %rep 0fch
    nop
    %endrep
madtanklabel%1:
    jmp 10h:5000h
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

%macro madtank_int39 1
    %rep 100h-5
    nop
    %endrep
	int 36
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

%assign j 249
%macro madtank_123 1
    %rep 079h
    dw j
	%assign j j+1
    %endrep
madtanklabel%1:
    jmp madtanklabel%1
%endmacro

madtank:
%assign i 0
%rep 40h
madtank_ok i
%assign i i+1 
madtank_ok i
%assign i i+1 
madtank_ok i
%assign i i+1 
madtank_ok i
%assign i i+1 
%endrep
madtankend:

SECTION .data
bounds:
; dd 5
; dd 10
 
