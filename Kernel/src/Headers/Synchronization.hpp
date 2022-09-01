#pragma once
#ifndef H_Synchronization
#define H_Synchronization
#include <Types.h>

global void SpinLockAquire(uint32* lock);
global void SpinLockFree(uint32* lock);

struct SpinLock
{

	forceinline SpinLock()
	{
		Lock = 0x00;
	}

	forceinline void Aquire() { SpinLockAquire(&Lock); }
	forceinline void Free() { SpinLockFree(&Lock); }
	forceinline bool GetState() { return this->Lock != 0; } // true = locked, false = free

private:
	volatile uint32 Lock;

};


global void MutexLock(uint32* lock);
global void MutexUnlock(uint32* lock);

struct Mutex
{
	forceinline Mutex() { mLock = 0x00; }

	forceinline void Lock() { MutexLock(&mLock); }
	forceinline void Unlock() { MutexUnlock(&mLock); }
	forceinline bool IsLocked() { return this->mLock != 0; } // true = locked, false = free

private:
	volatile uint32 mLock;
};

#endif // !H_Synchronization
