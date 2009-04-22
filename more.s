[BITS 16]	; protected mode
[global start]
[global put_handler]	; called by an extern function
[global idt]
[global gdt]
[global idt_ptr]
[global crash]
[global isr_nothing]
[global start_interrupts]
[global jump_tank]
[extern printfoo]
[extern printbar]
[extern printn]
[extern interrupt_handler]
[extern setup_gdt]
[extern main]
[extern tss_kernel]
[extern tss_tank]
[extern KERNEL_CODE_END]
[extern TANK_START]
[extern TANK_END]
[extern TANK_SIZE]

; disk=0, track=0..79, head=0..1, sector=1..18

ALIGN 8

SECTION .text

start:

	cli                     ; Disable interrupts, we want to be alone
        xor ax, ax
        mov ds, ax              ; Set DS-register to 0 - used by lgdt

        lgdt [gdt_desc]         ; Load the GDT descriptor

        mov eax, cr0            ; Copy the contents of CR0 into EAX
        or eax, 1               ; Set bit 0
        mov cr0, eax            ; Copy the contents of EAX into CR0

        jmp 08h:clear_pipe      ; Jump to code segment, offset clear_pipe
				
[BITS 32]                       ; We now need 32-bit instructions
clear_pipe:
        mov ax, 10h             ; Save data segment identifyer
        mov ds, ax              ; Move a valid data segment into the data segment register
        mov es, ax              ; Move a valid data segment into the data segment register
        mov fs, ax              ; Move a valid data segment into the data segment register
        mov gs, ax              ; Move a valid data segment into the data segment register
        mov ss, ax              ; Move a valid data segment into the stack segment register
        mov esp, 090000h        ; Move the stack pointer to 090000h


	call enable_A20
	call remap_ints
	lidt [idt_ptr];
	sti;
	jmp main;

jump_tank:
	pop ax
	jmp 30h:0

remap_ints:
	mov al, 11h
	out 020h, al
	out 0A0h, al
	mov al, 20h
	out 021h, al
	mov al, 28h
	out 0A1h, al
	mov al, 4
	out 021h, al
	mov al, 2
	out 0A1h, al
	mov al, 1
	out 021h, al
	out 0A1h, al
	mov al, 0
	out 021h, al
	out 0A1h, al	
	
	;100Hz clock
	mov al, 34h
	out 0x43, al
	mov al, 9Ch
	out 0x40, al
	mov al, 2eh
	out 0x40, al
	ret


enable_A20:
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

put_screen:
	

; Need a guzzilion of these just because Intel don't tell us the interrupt number...

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
db "Frontline interrupt handlers are here:"

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

SECTION .data

; The IDT...

%macro idt_entry 1
	dw isr_head_%1
	dw 08h
	dw 0x8e00
	dw 0
%endmacro

db "IDT starts here:"

idt:
%assign i 0
%rep 256
idt_entry i
%assign i i+1
%endrep
idt_end: 
db ":IDT ended there:"

idt_ptr:
	dw idt_end - idt - 1; IDT limit
	dd idt	; start of IDT

db "GDT starts here:"
gdt:                    ; Address for the GDT

gdt_null:               ; Null Segment
        dd 0
        dd 0

gdt_kernel_code:               ; Code segment, read/execute, nonconforming
        dw TANK_SIZE ;       ; limit 15:0
        dw 0            ; base 15:0
        db 0            ; base 23:16
        db 10011010b    ; present, dpl*2, sys/code, type*4
        db 11000001b    ; gran, 16/32, 0, avail, limit 19:16
        db 0            ; base 31:24

gdt_kernel_data:               ; Data segment, read/write, expand down
        dw 0h        ; limit 15:0 16MB = 1000000h Bytes
        dw 0            ; base 15:0
        db 0            ; base 23:16
        db 10010010b    ; present, dpl*2, sys/code, type*4
        db 11001100b    ; gran, 16/32, 0, avail, limit 19:16 just beyond the screen
        db 0            ; base 31:24

gdt_tank_code:               ; Data segment, read/write, expand down
	dw 0000h           ; limit 15:0
	dw 0            ; base 15:0
	db 0h          ; base 23:16 from just after screen
	db 10010010b    ; present, dpl*2, sys/code, type*4
	db 01000000b    ; gran, 16/32, 0, avail, limit 19:16 just beyond the screen
	db 0            ; base 31:24

gdt_tank_data:               ; Data segment, read/write, expand down
	dw 1000h           ; limit 15:0
	dw 0            ; base 15:0
	db 0Ch            ; base 23:16 from just after screen
	db 10010010b    ; present, dpl*2, sys/code, type*4
	db 11000000b    ; gran, 16/32, 0, avail, limit 19:16 just beyond the screen
	db 0            ; base 31:24
		    
gdt_kernel_tss:
	dw 104            ; limit 15:0 
	dw tss_kernel     ; base 15:0
	db 0            ; base 23:16 from just after screen
	db 10001001b    ; present, dpl*2, sys/code, type*4
	db 00000000b    ; gran, 16/32, 0, avail, limit 19:16 
	db 0            ; base 31:24

gdt_tank_tss:
	dw 104            ; limit 15:0 
	dw tss_tank     ; base 15:0
	db 0            ; base 23:16 from just after screen
	db 10001001b    ; present, dpl*2, sys/code, type*4
	db 00000000b    ; gran, 16/32, 0, avail, limit 19:16 
	db 0            ; base 31:24

gdt_end:                ; Used to calculate the size of the GDT
db ":GDT ended there"

gdt_desc:                       ; The GDT descriptor
        dw gdt_end - gdt - 1    ; Limit (size)
        dd gdt                  ; Address of the GDT

