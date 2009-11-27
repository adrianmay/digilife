[BITS 16]	; protected mode

[extern gdt]
[extern gdt_desc]
;[extern kernel_stack_base]
[extern spare_stack_block_1]
[extern spare_stack_block_2]
;[extern printfoo]
[extern printbar]
[extern printn]
[extern interrupt_handler]
[extern keyboard_handler]
[extern main]
[extern hack_kernelcodestart]
[extern hack_kernelcodelimit]
[extern hack_kerneldatastart]
[extern hack_kerneldatalimit]
[extern hack_kernelstackstart]
[extern hack_sparestack1start]
[extern hack_sparestack2start]
[extern hack_kernelstack]
[extern KERNEL_CODE_END]
[extern KERNEL_DATA_BEGIN]
[extern KERNEL_DATA_SIZE]

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
[global go_real]

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
	mov ax, hack_kernelstack
	mov [hack_kernelstackstart], ax
	mov ax, spare_stack_block_1
	mov [hack_sparestack1start], ax
	mov ax, spare_stack_block_2
	mov [hack_sparestack2start], ax

	;mov ax, 0x8000
	;mov [hack_kernelcodestart], ax
	;mov ax, KERNEL_CODE_END
	;mov [hack_kernelcodelimit], ax
	;mov ax, KERNEL_DATA_BEGIN
	;add ax, 0x8000
	;mov [hack_kerneldatastart], ax
	;mov ax, KERNEL_DATA_SIZE
	;mov [hack_kerneldatalimit], ax
	
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
	;sidt [idt_old];
	lidt [idt_ptr];
	sti;
	jmp main;

jump_tank:
	pop ax
	jmp 60h:0

savesp:
	dd 0

;bum out to real mode for disk operations
;need to save and restore tanks
;use tracks 8,9,10,11 with 4000h each whether floppy or stick
	
go_real:
	cli
	pushad
	push ds
	push es
	push fs
	push gs	
	mov [savesp], esp
	mov ax,0x10
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax
	lidt [idt_old]
	mov eax, cr0            
	and eax, 0xfffffffe     
	mov cr0, eax            
	jmp 0h:real_mode       

babystack:
%rep 16
	dd 0
%endrep
babystackend:

real_mode:
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax	
	mov es, ax
	mov ss, ax
	mov ax, babystackend
	mov sp, ax	
	mov ax, 0b800h
	mov es, ax
	mov di, 2*(80*24+38)
	mov al, 'R'
	mov [es:di],al
	call unmap_ints
	;sti
die:
	jmp die
	;cmp ax, 1
	;jne real_1
	;call save_tank
	jmp real_done
real_1:
	cmp ax, 2
	jne real_done
	call load_tank
real_done:
	cli
	mov eax, cr0           
	or eax, 1              
	mov cr0, eax           
	jmp 08h:back_real      
	
	
back_real:	
	mov ax, 0b800h
	mov es, ax
	mov di, 2*(80*24+38)
	mov al, 'S'
	mov [es:di],al
	lidt [idt_ptr]
	mov ax, 0x38
	mov ss, ax
	mov esp, [savesp]
	pop gs
	pop fs
	pop es
	pop ds
	popad
	sti
	ret

real_test:	
	mov ax, 0xb800
	mov es, ax
	mov di, 2*(80*24+38)
	mov al, 'R'
	mov [es:di],al
	ret	


;enter with track number in ch and bx pointing to tank bit 
save_quarter:
	
	mov ax, 0x1000 ;TANKAT
	mov es, ax 
	mov al, 20h ;sectors
	mov cl,1
	mov dh,0
	%ifdef floppy
	mov dl,0
	%else
	mov dl,0x80
	%endif
	mov bx, 0 ;
	mov ah,3
	int 13h
	ret

save_tank:
	mov ch, 8
	mov bx, 0
	call save_quarter
	mov ch,9
	mov bx, 0x4000
	call save_quarter
	mov ch,10
	mov bx, 0x8000
	call save_quarter
	mov ch,11
	mov bx, 0xc000
	call save_quarter
	ret

;enter with track number in ch and bx pointing to tank bit 
load_quarter:
	
	mov ax, 0x1000 ;TANKAT
	mov es, ax 
	mov al, 20h ;sectors
	mov cl,1
	mov dh,0
	%ifdef floppy
	mov dl,0
	%else
	mov dl,0x80
	%endif
	mov bx, 0 ;
	mov ah,2
	int 13h
	ret

load_tank:
	mov ch, 8
	mov bx, 0
	call load_quarter
	mov ch,9
	mov bx, 0x4000
	call load_quarter
	mov ch,10
	mov bx, 0x8000
	call load_quarter
	mov ch,11
	mov bx, 0xc000
	call load_quarter
	ret
	
	
load_tsr:
        ltr     word [ss:esp+4]
        ret	
	
remap_ints:
	mov al, 11h
	out 020h, al ;reset
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

unmap_ints:
	mov al, 11h
	out 020h, al ;reset
	out 0A0h, al
	mov al, 0h
	out 021h, al
	mov al, 8h
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
	push es
	push eax
	push edi
	mov ax, 0x68
	mov es, ax
	mov edi, [ebp+8]
	shl	edi,1
	mov eax, [ebp+12]
	mov [es:di],al
	pop edi
	pop eax
	pop es
	pop ebp
	ret

clearscreen:
	push ebp
	mov ebp,esp
	push eax
	push edx
	push edi
	push es
	mov dx, 0x3d4 ;hide cursor
	mov al, 10
	out dx, al
	inc dx
	mov al, 32
	out dx, al
	mov ax, 0x68
	mov es, ax
	mov eax, 0x0f00
	mov edi, 0
clsloop:
	mov [es:di], ax
	inc edi
	inc edi
	cmp edi, 80*25*2
	jne clsloop;
	pop es
	pop edi
	pop edx
	pop eax
	pop ebp
	ret

nuketank:
	push ebp
	mov ebp, esp
	pushad
	push es
	mov ebx, 0x20
	mov es, bx ;tank
	mov edi, 0 ;TANKPAGES
nukeloop:
	;push edi
	;mov ebx, 0x10
	;mov es, bx ;tank
	mov eax, 0x01020304
	;call rand
	;mov ebx, 0x20
	;mov es, bx ;tank
	;pop edi
	mov [es:edi], eax	
	inc edi
	inc edi
	inc edi
	inc edi
	cmp edi, 0x10000
	je	nukedone
	jmp nukeloop
nukedone:
	pop es
	popad
	pop ebp
	ret

thehistogram: times 256 dw 0  

	
get_histogram:
	push ebp
	mov ebp,esp
	pushad
	push es
	mov esi, thehistogram
	mov eax, 0x20
	mov es, ax ;tank
	mov edi, 0 ;TANKPAGES
histloop:
	mov ebx, 0
	mov	bl, [es:edi]
	shl ebx,1
	inc word [ds:esi+ebx]
	inc	edi
	cmp edi, 0x10000
	jne	histloop
histdone:
	pop es
	popad
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
    pushad
	push ds
	push es
	push fs
	push gs
    mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call interrupt_handler
    mov eax, [ss:esp+48] ; int number
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
	pop gs
	pop fs
	pop es
	pop ds
    popad
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
	;call delay
	sti
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

keyboard_task_loop:
	cli
	push  0
	push  33
    pushad
    push ds
    push es
    push fs
    push gs
    call keyboard_handler
    pop gs
    pop fs
    pop es
    pop ds

    mov al, 20h
    out 20h, al

    popad
    add esp, 4     ; Cleans up the pushed error code and pushed ISR number
	sti
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

idt_entry_ring0 i ;keyboard
;dw 0 
;dw 50h
;db 0
;db 085h
;dw 0
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

idt_old:
	dw 1024
	dd 0

