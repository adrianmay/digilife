[BITS 32]	; protected mode
[global put_handler]	; called by an extern function
[global idt]
[global idt_ptr]
[global enable_A20]
[global start_interrupts]
[extern printfoo]
align 8

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
%rep 256
	dw 0	; offset 15:0
	dw 08h	; selector
	dw 0x8E00	; present, ring 0, '386 interrupt gate
	dw 0	; offset 31:16
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


