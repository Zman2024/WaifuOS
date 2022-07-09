[bits 64]

; notes:
; parameter passing:
;	rdi 1st
;	rsi 2nd
;	rdx 3rd
;	rcx 4th
;	r8 5th
;	r9 6th
; temp regs:
;	r10
;	r11
; all other regs need to be saved if they are used

%define p0 rdi
%define p1 rsi
%define p2 rdx
%define p3 rdx

global EnableAVX
global TestAVX
global GetRDRAND
global CheckXSAVE
global EnableXSAVE
global memset64
global LoadTSS

CPUID.ECX.XSAVE equ 1 << 26
CR4.OSXSAVE equ 1 << 18

EnableAVX:
	push rax
	push rcx
	push rdx
	
	xor rcx, rcx
	xgetbv ;Load XCR0 register
	or rax, 0b0111 ;Set AVX, SSE, X87 bits
	xsetbv ;Save back to XCR0

	pop rdx
	pop rcx
	pop rax
ret

CheckXSAVE:
	push rbx
	push rcx
	push rdx

	  mov eax, 1
	  cpuid
	  xor rax, rax
	  test ecx, CPUID.ECX.XSAVE
	  setnz al

	pop rdx
	pop rcx
	pop rbx
ret


EnableXSAVE:
	mov rax, cr4
	or rax, CR4.OSXSAVE
	mov cr4, rax
ret


TestAVX:
	vpxor ymm0, ymm0, ymm0
ret

; Scheduling.h
; rdi: tss offset in GDT
LoadTSS:
	mov rax, rdi
	and rax, 0xFFFF
	ltr ax
ret

GetRDRAND:
	rdrand rax
ret

; p0 / rdi: void* destination
; p1 / rsi: uint64 value
; p2 / rdx: uint64 nBytes
memset64:
	mov rax, p1
	mov rcx, p2
	shr rcx, 3    ; divide by 8
	and p2, 0b111 ; the remainder
	rep stosq 	; store value into buffer

	test p2, p2
	.byteLoop:
	jz .end
		stosb
		ror rax, 8
		dec p2
	jmp .byteLoop
	.end:
ret
