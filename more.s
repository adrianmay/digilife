[BITS 32]	; protected mode
[global put_handler]	; called by an extern function
[global idt]
[global idt_ptr]
[global enable_A20]
[global start_interrupts]
[extern printfoo]
align 8
 
; Plan:
;   Use %rep and %assign macros to declare idt pointing to different functions. 
;   They're all the same size so don't need a pointer array
;   They push the int number (more macros) and jump to the standard handler which
;       does cleanup stuff according to int number, e.g. telling irq controllers if its a hardware int, 
;               trying to continue,etc
;       and tries a table of c functions

%macro isr_frontline_pushdummy 1 
isr_head_%1:
	cli
	push byte 0
	push byte %1
	jmp isr_common
%endmacro

%macro isr_frontline_nopushdummy 1 
isr_head_%1:
	cli
	push byte %1
	jmp isr_common
%endmacro

isr_common:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp   ; Push us the stack
    push eax
    mov eax, _fault_handler
    call eax       ; A special call, preserves the 'eip' register
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    mov ax, [sp+36] ; int number
    ; acknowledge whichever PICs
    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!


%macro idt_entry 1
	dw isr_head_%1
	dw 08h
	dw 0x8e00
	dw 0
%endmacro

enable_A20:
        cli

        call    a20wait
        mov     al,0xAD
        out     0x64,al

        call    a20wait
        mov     al,0xD0
        out     0x64,al

        call    a20wait2
        in      al,0x60
        push    eax

        call    a20wait
        mov     al,0xD1
        out     0x64,al

        call    a20wait
        pop     eax
        or      al,2
        out     0x60,al

        call    a20wait
        mov     al,0xAE
        out     0x64,al

        call    a20wait
        sti
        ret

a20wait:
        in      al,0x64
        test    al,2
        jnz     a20wait
        ret


a20wait2:
        in      al,0x64
        test    al,1
        jz      a20wait2
        ret

idt:
%assign i 0
%rep 256
idt_entry i
%assign i i+1
%endrep
idt_end: 

idt_ptr:
	dw idt_end - idt - 1; IDT limit
	dd idt	; start of IDT

common_isr:
	call printfoo;
	; jmp common_isr
	iretd;

start_interrupts:
	mov ecx,(idt_end - idt) >> 3	; number of descriptors
	mov edi,idt	; idt location
	mov esi,common_isr	; common handler to esi
fill_idt:
	mov eax,esi
	mov [edi],ax	; low offset
	shr eax,16
	mov [edi+6],ax	; high offset
	add edi,8	; descriptor length
	loop fill_idt

	lidt [idt_ptr];
	sti;
	ret;

put_handler:
	push ebx	; save
	push edx	; registers
	push eax	; that will 
	push ebp	; be changed

	mov ebp, esp
	mov ebx, [ss:ebp + 20]	; interrupt number
	mov eax, [ss:ebp + 24]	; interrupt handler
	mov edx, idt	; idt location

	; change address of handler
	shl ebx, 3	; need to multiply by 8
	mov [edx + ebx], ax	; low offset
	shr eax, 16
	mov [edx + ebx + 6], ax	; high offset

	; change flags
	mov ax, [ss:ebp + 28]	; gate flags
	mov [edx + ebx + 4], ax
	
	pop ebp	; restore 
	pop eax	; registers 
	pop edx	; that were 
	pop ebx	; changed	
	
	ret	; return to caller


