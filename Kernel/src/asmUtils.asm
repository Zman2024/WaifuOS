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

global memset64
global memcpy
global GetRDRAND
global LoadTSS
global repmovsb

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
	push rcx
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
	pop rcx
ret

; p0 / rdi: void* destination
; p1 / rsi: void* src
; p2 / rdx: nint n
memcpy:
	cld
	mov rcx, p2
	shr rcx, 3	; divide by 8
	and p2, 0b111 ; remainder
	rep movsq	; mega fast boi
	
	; the rest
	mov rcx, p2
	rep movsb
ret

; p0 / rdi: void* destination
; p1 / rsi: void* src
; p2 / rdx: nint n
repmovsb:
	cld
	mov rcx, p2
	rep movsb	; mega fast boi
ret

