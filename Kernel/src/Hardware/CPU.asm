[bits 64]

global GetXCR0
global SetXCR0

GetXCR0:
    push rcx
    push rdx

    xor rcx, rcx
    xgetbv		; Load XCR0 register -> eax

    pop rdx
    pop rcx
ret

; rdi: new value of XCR0
SetXCR0:
	push rax
    push rcx
    push rdx
 
    xor rcx, rcx
	mov rax, rdi
    xsetbv ; eax -> XCR0
 
    pop rdx
    pop rcx
    pop rax
ret