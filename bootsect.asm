[BITS 16]       ; We need 16-bit intructions for Real mode

[ORG 0x7C00]    ; The BIOS loads the boot sector into memory location 0x7C00


reset_drive:
        mov ah, 0               ; RESET-command
        int 13h                 ; Call interrupt 13h
        or ah, ah               ; Check for error code
        jnz reset_drive         ; Try again if ah != 0

        mov ax, 0
        mov es, ax
        mov bx, 0x1000          ; Destination address = 0000:1000

        mov ah, 02h             ; READ SECTOR-command
        mov al, 24             ; Number of sectors to read 
        mov ch, 0               ; Cylinder = 0
        mov cl, 02h             ; Sector = 2
        mov dh, 0               ; Head = 0
        int 13h                 ; Call interrupt 13h
        or ah, ah               ; Check for error code
        jnz reset_drive         ; Try again if ah != 0

        cli                     ; Disable interrupts, we want to be alone

	;remap hardware interrupts from 32
	
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

;enable_A20:
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

        jmp 08h:01000h          ; Jump to section 08h (code), offset 01000h

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

gdt:                    ; Address for the GDT

gdt_null:               ; Null Segment
        dd 0
        dd 0

gdt_code:               ; Code segment, read/execute, nonconforming
        dw 08000h       ; limit 15:0
        dw 0            ; base 15:0
        db 0            ; base 23:16
        db 10011010b    ; present, dpl*2, sys/code, type*4
        db 01000000b    ; gran, 16/32, 0, avail, limit 19:16
        db 0            ; base 31:24

gdt_data:               ; Data segment, read/write, expand down
        dw 0h        ; limit 15:0
        dw 0            ; base 15:0
        db 0            ; base 23:16
        db 10010010b    ; present, dpl*2, sys/code, type*4
        db 01001100b    ; gran, 16/32, 0, avail, limit 19:16
        db 0            ; base 31:24

gdt_end:                ; Used to calculate the size of the GDT



gdt_desc:                       ; The GDT descriptor
        dw gdt_end - gdt - 1    ; Limit (size)
        dd gdt                  ; Address of the GDT




times 510-($-$$) db 0           ; Fill up the file with zeros

        dw 0AA55h                ; Boot sector identifyer
