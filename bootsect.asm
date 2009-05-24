; disk=0, track=0..79, head=0..1, sector=1..18

[BITS 16]       ; We need 16-bit intructions for Real mode

[ORG 0x7C00]    ; The BIOS loads the boot sector into memory location 0x7C00

	mov ax, cs
	mov ss, ax
	mov ds, ax

reset_drive:

        mov dl,0 
        mov ah, 0               ; RESET-command
        int 13h                 ; Call interrupt 13h
        or ah, ah               ; Check for error code
        jnz reset_drive         ; Try again if ah != 0

        mov ch, 3               ; Track/Cylinder 
; for some strange reason, I can't persuade int13 
; to load into higher places than approx 8000h
track_loop:        
        mov ax, 07E0h
        mov es, ax
        mov bx, 0          ; Destination address = 0000:1000
        mov dl,0 
        mov cl, 1               ; Sector 
        mov dh, 0               ; Head 
        mov ah, 2             ; READ SECTOR-command
        mov al, 36               ; Number of sectors to read 
        int 13h                 ; Call interrupt 13h
		mov al, [bum]
		inc al
		mov [bum], al
		mov bh, ch; save from rep copy
        cmp ch, 0
        jz track_done
        mov dh, 0
        mov dl, ch
        mov ax, 0480h
        mul dx
        mov dx, 07E0h
        add ax, dx
		mov es, ax
		mov di, 0
		mov ax, ds ;save ds
		mov fs, ax
		mov ax, 07E0h
		mov ds, ax
		mov si, 0
		mov cx, 4800h
		rep movsb
		mov ax, fs	; restore	        
		mov ds, ax	; restore	        
		mov ch, bh ; restore
		dec ch
		jmp track_loop
track_done:
		
		mov ax, 0xb800
		mov es, ax
		mov di, 80
		mov ah, 0fh
		mov al, [bum]
		mov [es:di], ax
		
		mov ax, 10c0h
		mov ds, ax
		mov si, 0
		mov di, 800
		mov cx, 50
		rep movsb
stophere:
	jmp 0x8000
    
reset_drive_1:
        mov dl,0 
        mov ah, 0               ; RESET-command
        int 13h                 ; Call interrupt 13h
        or ah, ah               ; Check for error code
        jnz reset_drive         ; Try again if ah != 0

        mov ax, 0800h
        mov es, ax
        mov bx, 0          ; Destination address = 0000:1000

        mov ah, 02h             ; READ SECTOR-command
        mov al, [length]              ; Number of sectors to read 
        mov ch, 0               ; Cylinder = 0
        mov cl, 02h             ; Sector = 2
        mov dh, 0               ; Head = 0
        int 13h                 ; Call interrupt 13h
        or ah, ah               ; Check for error code
        jnz reset_drive         ; Try again if ah != 0

		;jmp stophere
			

fowia:
    pop bx
    push bx
    ret
	
bum:	
db '@'


times 508-($-$$) db 0           ; Fill up the file with zeros
length:
	dw 2
        dw 0AA55h                ; Boot sector identifyer

