[bits 64]

global EnableAVX
global TestAVX
global GetRDRAND
global CheckXSAVE
global EnableXSAVE

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

GetRDRAND:
	rdrand rax
ret