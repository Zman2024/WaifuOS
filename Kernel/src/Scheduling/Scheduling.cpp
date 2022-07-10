#include <Scheduling.h>
#include <Memory.h>
#include <PageFrameAllocator.h>
#include <CPU.h>
#include <PIT.h>
#include <Synchronization.hpp>

attribute((aligned(PAGE_SIZE))) TaskStateSegment gTSS;
byte IOPB[33];

using namespace Interrupts;

namespace Scheduler
{
	bool Running = false;
	bool deletingThread = false;
	List<Thread*>* Threads = nullptr;
	nint nextThreadID = 1;
	nint currentThreadIndex = 0;
	nint threadsCreatedSinceBoot = 0;

	SpinLock taskEndSpinlock = SpinLock();

	void Start()
	{
		cli;
		Threads = new List<Thread*>(20);
		Threads->Add(new Thread(0x00));
		Running = true;
		deletingThread = false;
		currentThreadIndex = 0;
		nextThreadID = 1;
		threadsCreatedSinceBoot = 1;

		Yield();

		// Increase retarded robbin task switch frequency
		PIT::SetDivisor(100);
	}

	void Yield()
	{
		if (!Running || !Threads) return;
		sti;
		intcall(Interrupt::PIT);
	}

	void TaskSwitch(RegisterState* registers, InterruptFrame* frame)
	{
		if (!Running || !Threads) return;
		cli;

		auto currentThread = Threads->Get(currentThreadIndex);
		if (!currentThread) intcall(Interrupt::MachineCheck); // idk
		if (deletingThread)
		{
			*registers = currentThread->Registers;
			*frame = currentThread->InterruptFrame;
			currentThread->Running = true;
			deletingThread = false;
			taskEndSpinlock.Free();
			return;
		}

		currentThread->Registers = *registers;
		currentThread->InterruptFrame = *frame;
		currentThread->Running = false;

		if (++currentThreadIndex >= Threads->GetCount())
			currentThreadIndex = 0;

		currentThread = Threads->Get(currentThreadIndex);
		
		*registers = currentThread->Registers;
		*frame = currentThread->InterruptFrame;
		currentThread->Running = true;
	}

	nint CreateThread(void(*main)(nint))
	{
		cli;
		auto thread = new Thread(nextThreadID++);
		
		// 32kb stack to start with
		thread->StackBaseAddress = malloc(0x8000);

		// well... 32kb - 4b
		uint32* threadStack = (uint32*)(((byte*)thread->StackBaseAddress) + PAGE_SIZE - 0x4);
		
		// because return address goes here
		threadStack[0] = (uint32)TaskEnded;

		// Create thread
		thread->Running = false;
		thread->InterruptFrame.rflags = FLAGS::RSV0 | FLAGS::IF;
		thread->InterruptFrame.rsp = (nint)(threadStack);
		thread->InterruptFrame.rip = (nint)(vptr)main;
		thread->Registers.rbp = (nint)(threadStack + 1);
		thread->InterruptFrame.cs = 0x08;
		thread->InterruptFrame.ss = 0x10;
		thread->Registers.rdi = thread->ProcessID;
		Threads->Add(thread);

		++threadsCreatedSinceBoot;

		sti;
		return thread->ProcessID;
	}

	nint GetThreadCount()
	{
		if (!Threads) return 0;
		return Threads->GetCount();
	}

	nint GetThreadsCreated()
	{
		return threadsCreatedSinceBoot;
	}

	void TaskEnded()
	{
		if (!Running || !Threads) intcall(Interrupt::MachineCheck);
		cli;
		taskEndSpinlock.Aquire();

		auto currentThread = Threads->Get(currentThreadIndex);
		if (!currentThread) intcall(Interrupt::MachineCheck); // idk

		Threads->RemoveAt(currentThreadIndex--);
		delete currentThread;
		free(currentThread->StackBaseAddress);
		deletingThread = true;

		intcall(Interrupt::PIT);

		// shouldn't get here
		intcall(Interrupt::MachineCheck);
	}

}