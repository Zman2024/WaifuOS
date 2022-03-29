[bits 64]
global strcpy
global strcpys
global strlen

; rdi: const char* dest
; rsi: char* src
strcpy:
	xor rcx, rcx
	.loop:
		test al, al
	jz .end
		mov al, [rsi+rcx]
		mov [rdi+rcx], al
		inc rcx
	jmp .loop

	.end:
ret

; rdi: const char* dest
; rsi: char* src
; rdx: max
strcpys:
	xor rcx, rcx
	.loop:
		test al, al
	jz .end
		cmp rcx, rdx
	jae .end
		mov al, [rsi+rcx]
		mov [rdi+rcx], al
		inc rcx
	jmp .loop

	.end:
ret

; rdi: const char* str
strlen:
	xor rax, rax
	.loop:
		mov edx, [rdi+rax]
		test dl, dl
	jz strlen.end
		inc rax
	jmp strlen.loop
	.end:
ret