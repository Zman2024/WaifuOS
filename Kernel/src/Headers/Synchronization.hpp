#pragma once
#ifndef H_Synchronization
#define H_Synchronization
#include <Types.h>

global void SpinLockAquire(uint32* lock);
global void SpinLockFree(uint32* lock);

struct SpinLock
{

	inline SpinLock()
	{
		Lock = 0x00;
	}

	forceinline void Aquire() { SpinLockAquire(&Lock); }
	forceinline void Free() { SpinLockFree(&Lock); }
	forceinline bool GetState() { return this->Lock != 0; } // true = locked, false = free

private:
	volatile uint32 Lock;

};

#endif // !H_Synchronization
