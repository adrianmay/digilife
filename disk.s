;bum out to real mode for disk operations
;need to save and restore tanks
;use tracks 8,9,10,11 with 4000h each whether floppy or stick

%include "defs.s"

[BITS 16]
[extern go_real]
[extern back_real]
[global real_mode]

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
die:
	jmp die
	mov ax, babystackend
	mov sp, ax	
	mov ax, 0b800h
	mov es, ax
	mov di, 2*(80*24+38)
	mov al, 'R'
	mov [es:di],al
	sti
	call real_test
	jmp real_done
	cmp ax, 1
	jne real_1
	call save_tank
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
	
