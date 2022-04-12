[bits 64]
global SaveSIMD
global RestoreSIMD
global DumpGeneralRegisters
global GetRegisterDump

section .data

Y0State:  do 0.0, 0.0
Y1State:  do 0.0, 0.0
Y2State:  do 0.0, 0.0
Y3State:  do 0.0, 0.0
Y4State:  do 0.0, 0.0
Y5State:  do 0.0, 0.0
Y6State:  do 0.0, 0.0
Y7State:  do 0.0, 0.0
Y8State:  do 0.0, 0.0
Y9State:  do 0.0, 0.0
Y10State: do 0.0, 0.0
Y11State: do 0.0, 0.0
Y12State: do 0.0, 0.0
Y13State: do 0.0, 0.0
Y14State: do 0.0, 0.0
Y15State: do 0.0, 0.0

RegisterState:
	.rax: dq 0
	.rbx: dq 0
	.rcx: dq 0
	.rdx: dq 0

	.rsi: dq 0
	.rdi: dq 0

	.r8: dq 0
	.r9: dq 0
	.r10: dq 0
	.r11: dq 0
	.r12: dq 0
	.r13: dq 0
	.r14: dq 0
	.r15: dq 0

	.rflags: dq 0

section .text

GetRegisterDump:
	lea rax, [RegisterState]
ret

DumpGeneralRegisters:
	mov [RegisterState.rax],	rax
	mov [RegisterState.rbx],	rbx
	mov [RegisterState.rcx],	rcx
	mov [RegisterState.rdx],	rdx

	mov [RegisterState.rsi],	rsi
	mov [RegisterState.rdi],	rdi

	mov [RegisterState.r8],		r8
	mov [RegisterState.r9],		r9
	mov [RegisterState.r10],	r10
	mov [RegisterState.r11],	r11
	mov [RegisterState.r12],	r12
	mov [RegisterState.r13],	r13
	mov [RegisterState.r14],	r14
	mov [RegisterState.r15],	r15

	; get rflags
	pushfq
	pop rax
	mov [RegisterState.rflags], rax
	
	lea rax, [RegisterState]
ret

SaveSIMD:
	vmovdqu [Y0State],	ymm0
	vmovdqu [Y1State],	ymm1
	vmovdqu [Y2State],	ymm2
	vmovdqu [Y3State],	ymm3
	vmovdqu [Y4State],	ymm4
	vmovdqu [Y5State],	ymm5
	vmovdqu [Y6State],	ymm6
	vmovdqu [Y7State],	ymm7
	vmovdqu [Y8State],	ymm8
	vmovdqu [Y9State],	ymm9
	vmovdqu [Y10State], ymm10
	vmovdqu [Y11State], ymm11
	vmovdqu [Y12State], ymm12
	vmovdqu [Y13State], ymm13
	vmovdqu [Y14State], ymm14
	vmovdqu [Y15State], ymm15
ret

RestoreSIMD:
	vmovdqu ymm0,  [Y0State]
	vmovdqu ymm1,  [Y1State]
	vmovdqu ymm2,  [Y2State]
	vmovdqu ymm3,  [Y3State]
	vmovdqu ymm4,  [Y4State]
	vmovdqu ymm5,  [Y5State]
	vmovdqu ymm6,  [Y6State]
	vmovdqu ymm7,  [Y7State]
	vmovdqu ymm8,  [Y8State]
	vmovdqu ymm9,  [Y9State]
	vmovdqu ymm10, [Y10State]
	vmovdqu ymm11, [Y11State]
	vmovdqu ymm12, [Y12State]
	vmovdqu ymm13, [Y13State]
	vmovdqu ymm14, [Y14State]
	vmovdqu ymm15, [Y15State]
ret