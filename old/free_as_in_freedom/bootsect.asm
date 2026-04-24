
%define floppy

;INT 13h AH=02h: Read Sectors From Drive AH=3 = write
;Parameters:
;AH 	02h
;AL 	Sectors To Read Count
;CH 	Track
;CL 	Sector
;DH 	Head
;DL 	Drive
;ES:BX 	Buffer Address Pointer
;Results:
;CF 	Set On Error, Clear If No Error
;AH 	Return Code
;AL 	Actual Sectors Read Count
; disk=0, track=0..79, head=0..1, sector=1..18

[BITS 16]       ; We need 16-bit intructions for Real mode

[ORG 0x7C00]    ; The BIOS loads the boot sector into memory location 0x7C00

	mov ax, cs
	mov ss, ax
	mov ds, ax

	mov ax, 0b800h
	mov es, ax
	mov al, 65
	mov di, 2
	mov [es:di], al
;freeze:
;	jmp freeze
	
%ifdef floppy	

reset_drive:

	mov dl,0 
	mov ah, 0               ; RESET-command
	int 13h                 ; Call interrupt 13h
	or ah, ah               ; Check for error code
	jnz reset_drive         ; Try again if ah != 0

	mov ch, 7               ; Track/Cylinder, gonna count down and copy from read buffer to correct place
; for some strange reason, I can't persuade int13 
; to load into higher places than approx 8000h
track_loop:        
	mov ax, 07E0h      ; right after this boot sector is read buffer. Track 1 onwards gets moved to 8000 onwards
	mov es, ax         ; Destination address
	mov bx, 0          ; Destination address
	mov dl,0           ; drive A
	mov cl, 1               ; Sector 
	mov dh, 0               ; Head 
	mov ah, 2             ; READ SECTOR-command
	mov al, 36               ; Number of sectors to read - all heads*all sectors in a track 
	int 13h                 ; Call interrupt 13h
;	mov al, [bum]		;debugging
;	inc al
;	mov [bum], al
	mov bh, ch; save from rep copy
	cmp ch, 0	
	jz track_done
	mov dh, 0 		;
	mov dl, ch
	mov ax, 0480h	;length of one track (/16 becasue segments)
	mul dx
	mov dx, 07E0h	;base of program
	add ax, dx
	mov es, ax	;destination
	mov di, 0	; for copy
	mov ax, ds 	;save ds
	mov fs, ax	; in fs
	mov ax, 07E0h	;source for copy
	mov ds, ax
	mov si, 0
	mov cx, 4800h	;length of copy
	rep movsb	;do copy
	mov ax, fs	;restore	        
	mov ds, ax	; ds	        
	mov ch, bh      ;restore track count
	dec ch		;next
	jmp track_loop
	
%else

reset_drive:

	mov dl,80h 
	mov ah, 0               ; RESET-command
	int 13h                 ; Call interrupt 13h
	or ah, ah               ; Check for error code
	jnz reset_drive         ; Try again if ah != 0
	
	mov ax, 0b800h
	mov es, ax
	mov al, 66
	mov di, 4
	mov [es:di], al

	mov ax, 07E0h      ; 
	mov es, ax         ; Destination address
	mov bx, 0          ; Destination address
	mov dl, 80h           ; drive C=80, D=81, A=0
	mov cl, 1               ; Sector 
	mov ch, 0
	mov dh, 0               ; Head 
	mov ah, 2             ; READ SECTOR-command
	mov al, 180               ; Number of sectors to read - all heads*all sectors in a track 
	int 13h                 ; Call interrupt 13h
    or ah, ah ; Check for error code
    jnz reset_drive ; Try again if ah != 0

%endif
track_done:
	jmp 0x8000
			


times 508-($-$$) db 0           ; Fill up the file with zeros
length:
	dw 2
    dw 0AA55h                ; Boot sector identifier

