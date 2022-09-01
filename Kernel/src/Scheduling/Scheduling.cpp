#include <Scheduling.h>
#include <Memory.h>
#include <PageFrameAllocator.h>
#include <PageTableManager.h>
#include <CPU.h>
#include <PIT.h>
#include <Synchronization.hpp>

attribute((aligned(PAGE_SIZE))) TaskStateSegment gTSS;
byte IOPB[33];

using namespace Interrupts;

namespace Scheduler
{
	bool Running = false;
	int64 YieldedTime = 0;

	nint NextThreadID = 0;

	List<Thread*> ReadyQueue;
	Thread* CurrentThread = nullptr;

	forceinline nint GetTimeQuantum()
	{
		nint count = ReadyQueue.GetCount() + 1;
		return 200 / count;
	}

	void hTaskYield(nint _, InterruptFrame * frame);

	void hStartScheduler(nint _, InterruptFrame * frame)
	{
		RegisterInterrupt((vptr)hTaskYield, Interrupt::TaskYield);

		CurrentThread = new Thread(NextThreadID++);
		CurrentThread->CurrentTimeQuantum = GetTimeQuantum();
		CurrentThread->StackBaseAddress = nullptr;
		CurrentThread->InterruptFrame = *frame;
		CurrentThread->Registers = *GetRegisterDump();
		CurrentThread->ThreadID = 0x00;
		CurrentThread->SetName("Kernel");

		RegisterInterrupt((vptr)hPitScheduler, Interrupt::PIT);
	}

	void Start()
	{
		if (Running) return;
		asm("pushfq");
		{
			cli;
			debug("Starting Scheduler...");

			Running = false;
			ReadyQueue = List<Thread*>();

			RegisterInterrupt((vptr)hStartScheduler, Interrupt::TaskYield);

			// start stuff (i dont wanna manually iretq in asm)
			Yield();

			PIT::SetFrequency(200);
			Running = true;
			debug("Started Scheduler!");
		}
		asm("popfq");
	}

	void TimerInterrupt(RegisterState* registers, InterruptFrame* frame)
	{
		if (!Running || ReadyQueue.GetCount() <= 0) return;

		if (!CurrentThread)
		{
			TaskSwitchOnEnd(registers, frame);
			return;
		}

		nint timeMs = (1.0 / PIT::GetFrequency()) * 1000;
		if (!timeMs) timeMs = 1;
		CurrentThread->CurrentTimeQuantum -= timeMs;

		if (CurrentThread->CurrentTimeQuantum <= 0)
			TaskSwitch(registers, frame);

	}

	void hTaskYield(nint _, InterruptFrame * frame)
	{
		if (!Running || !CurrentThread) return;
		YieldedTime += CurrentThread->CurrentTimeQuantum;
		CurrentThread->CurrentTimeQuantum = 0x00;
		TaskSwitch(GetRegisterDump(), frame);
	}

	void TaskSwitchOnEnd(RegisterState* registers, InterruptFrame* frame)
	{
		CurrentThread = ReadyQueue.Get(0);
		ReadyQueue.RemoveAt(0);

		frame[0] = CurrentThread->InterruptFrame;
		registers[0] = CurrentThread->Registers;
		CurrentThread->CurrentTimeQuantum = GetTimeQuantum();
	}

	void TaskSwitch(RegisterState* registers, InterruptFrame* frame)
	{
		CurrentThread->InterruptFrame = *frame;
		CurrentThread->Registers = *registers;
		CurrentThread->CurrentTimeQuantum = 0x00;

		ReadyQueue.Add(CurrentThread);
		CurrentThread = ReadyQueue.Get(0);
		ReadyQueue.RemoveAt(0);

		frame[0] = CurrentThread->InterruptFrame;
		registers[0] = CurrentThread->Registers;
		CurrentThread->CurrentTimeQuantum = GetTimeQuantum();
	}

	nint CreateThread(void(*entry0)(nint), const char* title)
	{
		if (!Running) return;

		uint32* stack = (u32*)calloc(PAGE_SIZE * 4);

		Thread* newThread = new Thread(NextThreadID++);
		newThread->CurrentTimeQuantum = GetTimeQuantum();
		newThread->StackBaseAddress = stack;
		newThread->SetName(title);

		stack += PAGE_SIZE - 3;
		stack[0] = (u32)ThreadExit;

		newThread->InterruptFrame.cs = 0x08;
		newThread->InterruptFrame.ss = 0x10;
		newThread->InterruptFrame.rflags = FLAGS::IF | FLAGS::RSV0;
		newThread->InterruptFrame.rip = (nint)entry0;
		newThread->InterruptFrame.rsp = (nint)stack;

		newThread->Registers.rbp = nint(stack + 1);
		newThread->Registers.ds = 0x10;
		newThread->Registers.es = 0x10;
		newThread->Registers.fs = 0x10;
		newThread->Registers.gs = 0x10;
		newThread->Registers.rdi = newThread->ThreadID;

		ReadyQueue.Add(newThread);

	}

	nint GetThreadCount()
	{
		return ReadyQueue.GetCount() + 1;
	}

	nint GetThreadsCreated()
	{
		return NextThreadID;
	}

	Thread* GetCurrentThread()
	{
		return CurrentThread;
	}

	void ThreadExit()
	{
		cli;
		free(CurrentThread->StackBaseAddress);
		delete CurrentThread;
		CurrentThread = nullptr;
		sti;

		while (true) hlt;
	}

}

global void Yield() { intcall(Interrupts::Interrupt::TaskYield); }
