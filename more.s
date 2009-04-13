[BITS 32]	; protected mode
[global start]
[global put_handler]	; called by an extern function
[global idt]
[global idt_ptr]
[global crash]
[global isr_nothing]
[global start_interrupts]
[extern printfoo]
[extern printbar]
[extern printn]
[extern interrupt_handler]
[extern main]
align 8
 
; Need a guzzilion of these just because Intel don't tell us the interrupt number...
start:
	lidt [idt_ptr];
	sti;
	jmp main;
        
%macro isr_frontline_pushdummy 1 
isr_head_%1:
	cli
	push  0
	push  %1
	jmp isr_common
%endmacro

%macro isr_frontline_nopushdummy 1 
isr_head_%1:
	cli
	push  %1
	jmp isr_common
%endmacro

isr_nothing:
    iret
    
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
    mov eax, esp   ; Push the stack
    push eax
    mov eax, interrupt_handler
    call eax       ; A special call, preserves the 'eip' register
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    mov eax, [ss:esp+32] ; int number
    ; acknowledge whichever PICs: 32-39 inclusive, just master (0x20,0x20), 40-47 also slave A0, 20
    cmp eax, 32
    jl no_more_acks
    cmp eax,48
    jnl no_more_acks
    cmp al, 40
    jl ack_master
    mov al, 020h
    out 0a0h, al
ack_master:
    mov al, 20h
    out 20h, al
no_more_acks:    
    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!


; These functions are what the IDT entries point at...
%assign i 0

%rep 8
isr_frontline_nopushdummy i
%assign i i+1
%endrep

isr_frontline_pushdummy i 
%assign i i+1
isr_frontline_nopushdummy i
%assign i i+1

%rep 5
isr_frontline_pushdummy i
%assign i i+1
%endrep

isr_frontline_nopushdummy i 
%assign i i+1
isr_frontline_nopushdummy i 
%assign i i+1
isr_frontline_pushdummy i 
%assign i i+1
isr_frontline_nopushdummy i
%assign i i+1
isr_frontline_nopushdummy i
%assign i i+1

; hardware interrputs to end
%rep 236
isr_frontline_pushdummy i
%assign i i+1
%endrep

; The IDT...

%macro idt_entry 1
	dw isr_head_%1
	dw 08h
	dw 0x8e00
	dw 0
%endmacro

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


; obsolete...
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

crash:
;        mov eax, [0xffffffff]
;	int 3
	ret
	mov ax, 8
	mov  ds, ax
	mov byte [ds:20], 9
        ret
