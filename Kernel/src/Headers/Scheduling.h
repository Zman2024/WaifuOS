#pragma once
#ifndef H_Scheduling
#define H_Scheduling
#include <Types.h>
#include <BitMap.h>
#include <Interrupts.h>
#include <List.h>

struct TaskStateSegment
{
	uint32 rsv0 = 0x00;
	
	uint64 rsp0 = 0x00;
	uint64 rsp1 = 0x00;
	uint64 rsp2 = 0x00;

	uint64 rsv1 = 0x00;

	uint64 ist_rsp1 = 0x00;
	uint64 ist_rsp2 = 0x00;
	uint64 ist_rsp3 = 0x00;
	uint64 ist_rsp4 = 0x00;
	uint64 ist_rsp5 = 0x00;
	uint64 ist_rsp6 = 0x00;
	uint64 ist_rsp7 = 0x00;

	uint64 rsv2 = 0x00;
	
	uint16 rsv3 = 0x00;
	uint16 IOMapBase = sizeof(TaskStateSegment); // we put the IOPB right after the TSS

};

global TaskStateSegment gTSS;
global void LoadTSS(nint entryOffset); // defined in asmUtils.asm

struct Thread
{
	inline Thread() { }

	forceinline Thread(nint id)
	{
		ThreadID = id;
		Name[0] = 0x00;
	}

	forceinline void SetName(const char* name)
	{
		if (!name)
		{
			strncpy(Name, cstr::format("Unnamed Thread #%0", ThreadID), sizeof(Name));
			return;
		}
		strncpy(Name, name, sizeof(Name));
	}

	nint ThreadID;

	// the time in ms this process has left
	// until the next task switch
	int64 CurrentTimeQuantum; 

	vptr StackBaseAddress = nullptr;
	Interrupts::RegisterState Registers;
	Interrupts::InterruptFrame InterruptFrame;

	char Name[64];

};

// Retarded Robbin 2.0 scheduler
namespace Scheduler
{
	void Start();
	global inline void Yield(){ intcall(Interrupts::Interrupt::TaskYield); }
	void TimerInterrupt(Interrupts::RegisterState* registers, Interrupts::InterruptFrame* frame);
	void TaskSwitchOnEnd(Interrupts::RegisterState* registers, Interrupts::InterruptFrame* frame);
	void TaskSwitch(Interrupts::RegisterState* registers, Interrupts::InterruptFrame* frame);
	nint CreateThread(void(*entry0)(nint), const char* title = nullptr);
	nint GetThreadCount();
	nint GetThreadsCreated();
	Thread* GetCurrentThread();
	void ThreadExit();
}

#endif // !H_Scheduling