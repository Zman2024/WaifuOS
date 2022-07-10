[bits 64]
global SaveAllRegisters
global RestoreAllRegisters
global GetRegisterDump

section .data

RegisterState:
	.rax: dq 0
	.rbx: dq 0
	.rcx: dq 0
	.rdx: dq 0

	.rsi: dq 0
	.rdi: dq 0

	.rbp: dq 0

	.r8: dq 0
	.r9: dq 0
	.r10: dq 0
	.r11: dq 0
	.r12: dq 0
	.r13: dq 0
	.r14: dq 0
	.r15: dq 0

	.ds: dq 0
	.es: dq 0
	.fs: dq 0
	.gs: dq 0

	.mm0: dq 0
	.mm1: dq 0
	.mm2: dq 0
	.mm3: dq 0
	.mm4: dq 0
	.mm5: dq 0
	.mm6: dq 0
	.mm7: dq 0

	.ymm0:  do 0.0, 0.0
	.ymm1:  do 0.0, 0.0
	.ymm2:  do 0.0, 0.0
	.ymm3:  do 0.0, 0.0
	.ymm4:  do 0.0, 0.0
	.ymm5:  do 0.0, 0.0
	.ymm6:  do 0.0, 0.0
	.ymm7:  do 0.0, 0.0
	.ymm8:  do 0.0, 0.0
	.ymm9:  do 0.0, 0.0
	.ymm10: do 0.0, 0.0
	.ymm11: do 0.0, 0.0
	.ymm12: do 0.0, 0.0
	.ymm13: do 0.0, 0.0
	.ymm14: do 0.0, 0.0
	.ymm15: do 0.0, 0.0

section .text

GetRegisterDump:
	lea rax, [RegisterState]
ret

SaveAllRegisters:
	mov [RegisterState.rax],	rax
	mov [RegisterState.rbx],	rbx
	mov [RegisterState.rcx],	rcx
	mov [RegisterState.rdx],	rdx

	mov [RegisterState.rsi],	rsi
	mov [RegisterState.rdi],	rdi

	mov [RegisterState.rbp],	rbp

	mov [RegisterState.r8],		r8
	mov [RegisterState.r9],		r9
	mov [RegisterState.r10],	r10
	mov [RegisterState.r11],	r11
	mov [RegisterState.r12],	r12
	mov [RegisterState.r13],	r13
	mov [RegisterState.r14],	r14
	mov [RegisterState.r15],	r15

	; segment registers (cs and ss saved by CPU)
	mov rax, ds
	mov [RegisterState.ds], rax
	mov rax, es
	mov [RegisterState.es], rax
	mov rax, fs
	mov [RegisterState.fs], rax
	mov rax, gs
	mov [RegisterState.gs], rax
	
	movq [RegisterState.mm0], mm0
	movq [RegisterState.mm1], mm1
	movq [RegisterState.mm2], mm2
	movq [RegisterState.mm3], mm3
	movq [RegisterState.mm4], mm4
	movq [RegisterState.mm5], mm5
	movq [RegisterState.mm6], mm6
	movq [RegisterState.mm7], mm7

	vmovdqu [RegisterState.ymm0],	ymm0
	vmovdqu [RegisterState.ymm1],	ymm1
	vmovdqu [RegisterState.ymm2],	ymm2
	vmovdqu [RegisterState.ymm3],	ymm3
	vmovdqu [RegisterState.ymm4],	ymm4
	vmovdqu [RegisterState.ymm5],	ymm5
	vmovdqu [RegisterState.ymm6],	ymm6
	vmovdqu [RegisterState.ymm7],	ymm7
	vmovdqu [RegisterState.ymm8],	ymm8
	vmovdqu [RegisterState.ymm9],	ymm9
	vmovdqu [RegisterState.ymm10], ymm10
	vmovdqu [RegisterState.ymm11], ymm11
	vmovdqu [RegisterState.ymm12], ymm12
	vmovdqu [RegisterState.ymm13], ymm13
	vmovdqu [RegisterState.ymm14], ymm14
	vmovdqu [RegisterState.ymm15], ymm15
ret

RestoreAllRegisters:
	
	; segment registers (cs and ss restored by CPU (iretq))
	mov rax, [RegisterState.ds]
	mov ds, rax
	mov rax, [RegisterState.es]
	mov es, rax
	mov rax, [RegisterState.fs]
	mov fs, rax
	mov rax, [RegisterState.gs]
	mov gs, rax

	mov rax,	[RegisterState.rax]
	mov rbx,	[RegisterState.rbx]
	mov rcx,	[RegisterState.rcx]
	mov rdx,	[RegisterState.rdx]

	mov rsi,	[RegisterState.rsi]
	mov rdi,	[RegisterState.rdi]

	mov rbp,	[RegisterState.rbp]

	mov r8,		[RegisterState.r8]
	mov r9,		[RegisterState.r9]
	mov r10,	[RegisterState.r10]
	mov r11,	[RegisterState.r11]
	mov r12,	[RegisterState.r12]
	mov r13,	[RegisterState.r13]
	mov r14,	[RegisterState.r14]
	mov r15,	[RegisterState.r15]

	movq mm0, [RegisterState.mm0]
	movq mm1, [RegisterState.mm1]
	movq mm2, [RegisterState.mm2]
	movq mm3, [RegisterState.mm3]
	movq mm4, [RegisterState.mm4]
	movq mm5, [RegisterState.mm5]
	movq mm6, [RegisterState.mm6]
	movq mm7, [RegisterState.mm7]

	vmovdqu ymm0,  [RegisterState.ymm0]
	vmovdqu ymm1,  [RegisterState.ymm1]
	vmovdqu ymm2,  [RegisterState.ymm2]
	vmovdqu ymm3,  [RegisterState.ymm3]
	vmovdqu ymm4,  [RegisterState.ymm4]
	vmovdqu ymm5,  [RegisterState.ymm5]
	vmovdqu ymm6,  [RegisterState.ymm6]
	vmovdqu ymm7,  [RegisterState.ymm7]
	vmovdqu ymm8,  [RegisterState.ymm8]
	vmovdqu ymm9,  [RegisterState.ymm9]
	vmovdqu ymm10, [RegisterState.ymm10]
	vmovdqu ymm11, [RegisterState.ymm11]
	vmovdqu ymm12, [RegisterState.ymm12]
	vmovdqu ymm13, [RegisterState.ymm13]
	vmovdqu ymm14, [RegisterState.ymm14]
	vmovdqu ymm15, [RegisterState.ymm15]

ret