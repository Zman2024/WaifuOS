[bits 64]

global LoadGDT

; rdi = first param
LoadGDT: 
	lgdt [rdi]
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; Complex return so we set cs
	pop rdi
	mov rax, 0x08
	push rax
	push rdi
retfq