[BITS 16]       ; We need 16-bit intructions for Real mode

[ORG 0x7C00]    ; The BIOS loads the boot sector into memory location 0x7C00

	mov ax, cs
	mov ss, ax
	mov ds, ax
    
reset_drive:
        mov ah, 0               ; RESET-command
        int 13h                 ; Call interrupt 13h
        or ah, ah               ; Check for error code
        jnz reset_drive         ; Try again if ah != 0

        mov ax, 0
        mov es, ax
        mov bx, 0x8000          ; Destination address = 0000:1000

        mov ah, 02h             ; READ SECTOR-command
        mov al, [length]             ; Number of sectors to read 
        mov ch, 0               ; Cylinder = 0
        mov cl, 02h             ; Sector = 2
        mov dh, 0               ; Head = 0
        int 13h                 ; Call interrupt 13h
        or ah, ah               ; Check for error code
        jnz reset_drive         ; Try again if ah != 0
	
	mov ax, 0xb800
	mov es, ax
	mov ax, 80
	mov di, ax
	mov al, [length]
	add al, 0x20
	mov [es:di], al

stophere:
	jmp 0x8000

fowia:
    pop bx
    push bx
    ret
	
	



times 508-($-$$) db 0           ; Fill up the file with zeros
length:
	dw 2
        dw 0AA55h                ; Boot sector identifyer

