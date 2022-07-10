[bits 64]
global SpinLockAquire
global SpinLockFree
global MutexLock
global MutexUnlock
extern Yield

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

MutexLock:
	lock bts dword [rdi], 0
	jc .yield
ret
  .yield:
	call Yield
	test dword [rdi], 1
	jnz .yield
jmp MutexLock

MutexUnlock:
	mov dword [rdi], 0
ret