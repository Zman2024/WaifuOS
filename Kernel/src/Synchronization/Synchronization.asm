[bits 64]
global SpinLockAquire
global SpinLockFree
global SemaphoreSignal
global SemaphoreWait

SpinLockAquire:
	lock bts dword [rdi], 0
	jc .spin
ret
  .spin:
	pause
	test dword [rdi], 1
	jnz .spin
jmp SpinLockAquire

SpinLockFree:
	mov dword [rdi], 0
ret

SemaphoreWait:
ret

SemaphoreSignal:
ret