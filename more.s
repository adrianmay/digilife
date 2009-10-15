[BITS 16]	; protected mode

[extern gdt]
[extern gdt_desc]
[extern kernel_stack_base]
[extern spare_stack_block]
[extern printfoo]
[extern printbar]
[extern printn]
[extern interrupt_handler]
[extern keyboard_handler]
[extern main]
[extern hack_from]
[extern hack_to]
[extern hack_too]
[extern delay]
[extern rand]
[global thehistogram]

[global start]
[global idt]
[global idt_ptr]
[global crash]
[global load_tsr]
[global jump_tank]
[global keyboard_task_loop]
[global pokescreen]
[global clearscreen]
[global get_histogram]
[global nuketank]


ALIGN 8

SECTION .text

start:

	cli                     ; Disable interrupts, we want to be alone
	mov ax, 0b800h
	mov es, ax
	mov al, 68
	mov di, 8
	mov [es:di], al
	
	mov ax, 0
	mov ds, ax
        mov ax, hack_from
        mov [hack_to], ax
        mov ax, spare_stack_block
        mov [hack_too], ax
        lgdt [gdt_desc]         ; Load the GDT descriptor

        mov eax, cr0            ; Copy the contents of CR0 into EAX
        or eax, 1               ; Set bit 0
        mov cr0, eax            ; Copy the contents of EAX into CR0

        jmp 08h:clear_pipe      ; Jump to code segment, offset clear_pipe
				
[BITS 32]                       ; We now need 32-bit instructions
clear_pipe:
        mov ax, 10h             
        mov ds, ax              
        mov es, ax              
        mov fs, ax              
        mov ax, 68h             ; screen
        mov gs, ax              
        mov ax, 38h		; stack
        mov ss, ax              
        mov esp, 1024   ; 
        

	call enable_A20
	call remap_ints
	lidt [idt_ptr];
	sti;
	jmp main;

jump_tank:
	pop ax
	jmp 60h:0

load_tsr:
        ltr     word [ss:esp+4]
        ret
	
	
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

crash:
;        mov eax, [0xffffffff]
	int 32
	ret
	mov ax, 8
	mov  ds, ax
	mov byte [ds:20], 9
        ret


pokescreen: ;(where, what)
	push ebp
	mov ebp,esp
	push fs
	push eax
	push edi
	mov ax, 0x68
	mov fs, ax
	mov edi, [ebp+8]
	shl	edi,1
	mov eax, [ebp+12]
	mov [fs:di],al
	pop edi
	pop eax
	pop fs
	pop ebp
	ret

clearscreen:
	pusha
	mov dx, 0x3d4 ;hide cursor
	mov al, 10
	out dx, al
	inc dx
	mov al, 32
	out dx, al
	mov ax, 0x68
	mov es, ax
	mov cx, 80*25
	mov ax, 0x0f00
	mov di, 0
	rep stosw
	popa
	ret

nuketank:
	push ebp
	mov ebp, esp
	pusha
	mov ebx, 0x20
	mov gs, bx ;tank
	mov esi, 0 ;TANKPAGES
nukeloop:
	push esi
	mov eax, 0x10203040
	mov ebx, 0x10
	mov gs, bx ;tank
	call rand
	pop esi
	mov ebx, 0x20
	mov gs, bx ;tank
	mov [gs:esi], eax	
	inc esi
	inc esi
	inc esi
	inc esi
	cmp esi, 0x10000
	je	nukedone
	jmp nukeloop
nukedone:
	popa
	pop ebp
	ret

thehistogram: times 256 dw 0  

get_histogram_no:
	push ebp
	mov ebp,esp
	pusha
	mov ax, ds
	mov es, ax
	mov ax, 7
	mov cx, 0xff	
	mov di, thehistogram
	rep stosw
	popa
	pop ebp
	ret

	
get_histogram:
	push ebp
	mov ebp,esp
	pusha
	
	mov ax, ds
	mov es, ax
	
	mov edi, thehistogram
	mov esi, 0
	mov eax, 0x20
	mov gs, ax ;tank
	mov esi, 0 ;TANKPAGES
histloop:
	mov eax, 0
	mov	al, [gs:esi]
	shl eax,1
	mov ebx, eax
	inc word [ebx+edi]
;	mov ax, [ebx+edi]
;	inc ax
;	mov [ebx+edi], ax
	inc	esi
	cmp esi, 0x10000
	jne	histloop
histdone:
	popa
	pop ebp
	ret
	
dumptank:
	; drop 128*TANKPAGES sectors from 5200h on disk = 51st sector

	
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
    
isr_common:
    pusha
    mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov ax, 0x20   ; Load the Kernel Data Segment descriptor!
    mov fs, ax
    mov ax, 0x68   ; Screen
    mov gs, ax
    call interrupt_handler
    mov ax, 0x20   ; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ax, 0x68   ; Screen
    mov gs, ax
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
	;call delay
	sti
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

keyboard_task_loop:
	cli
	;call printbar
	push  0
	push  33
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
    call keyboard_handler
    pop gs
    pop fs
    pop es
    pop ds

    mov al, 20h
    out 20h, al

    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!
    jmp keyboard_task_loop



; These functions are what the IDT entries point at...
db "Frontline interrupt handlers are here:"

%assign i 0

%rep 8
isr_frontline_pushdummy i
%assign i i+1
%endrep

isr_frontline_nopushdummy i 
%assign i i+1
isr_frontline_pushdummy i
%assign i i+1

%rep 5
isr_frontline_nopushdummy i
%assign i i+1
%endrep

isr_frontline_pushdummy i 
%assign i i+1
isr_frontline_pushdummy i 
%assign i i+1
isr_frontline_nopushdummy i 
%assign i i+1
isr_frontline_pushdummy i
%assign i i+1
isr_frontline_pushdummy i
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
	db 0
	db 0eeh
	dw 0
%endmacro

%macro idt_entry_ring0 1
	dw isr_head_%1
	dw 08h
	db 0
	db 08eh
	dw 0
%endmacro

db "IDT starts here:"

idt:
%assign i 0
%rep 20
idt_entry_ring0 i
%assign i i+1 
%endrep
%rep 12
idt_entry_ring0 i
%assign i i+1 
%endrep

idt_entry_ring0 i ;timer
%assign i i+1

;idt_entry_ring0 i ;keyboard
dw 0 
dw 50h
db 0
db 085h
dw 0
%assign i i+1

%rep 222
idt_entry_ring0 i
%assign i i+1
%endrep

idt_end: 
db ":IDT ended there:"

idt_ptr:
	dw idt_end - idt - 1; IDT limit
	dd idt	; start of IDT


