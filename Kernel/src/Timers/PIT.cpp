#include <PIT.h>
#include <IOBus.hpp>

namespace PIT
{
	// PIT IO ports
	constexpr byte Data0 = 0x40;
	constexpr byte Data1 = 0x41;
	constexpr byte Data2 = 0x42;
	constexpr byte Command0 = 0x43;

	constexpr uint32 BaseFrequency = 1193182;

	uint16 Divisor = 0x00;
	fp64 TimeSinceInit = 0x00;

	TimerEntry Timers[0xFF];
	uint16 CurrentTimers = 0x00;

	void Initialize()
	{
		TimeSinceInit = 0x00;
		Divisor = 0x00;
		CurrentTimers = 0x00;
		SetDivisor(0x8000);
		memset<uint64>(Timers, 0, sizeof(Timers));
	}

	void ProcessTimers(fp64& deltaTime)
	{
		for (u16 x = 0; x < 0xFF; x++)
		{
			TimerEntry* timer = Timers + x;
			if (timer->Active)
			{
				timer->CurrentCount += deltaTime;
				if (timer->CurrentCount >= timer->Interval)
				{
					timer->CurrentCount = 0x00;
					timer->Handler();
				}
			}
		}
	}

	void Tick()
	{
		fp64 deltaTime = 1.0 / (fp64)GetFrequency();
		TimeSinceInit += deltaTime;
		ProcessTimers(deltaTime);
	}

	void Sleep(fp64 seconds)
	{
		fp64 startTime = TimeSinceInit;
		fp64 deltaTime = 0;

		while (deltaTime < seconds)
		{
			deltaTime = TimeSinceInit - startTime;
			hlt;
		}
	}

	void Sleep(uint64 miliseconds)
	{
		Sleep(miliseconds / 1000.0);
	}

	void SetFrequency(uint32 freq)
	{
		SetDivisor(BaseFrequency / freq);
	}

	void SetDivisor(uint16 divisor)
	{
		Divisor = divisor;
		IO::outb(Data0, (byte)divisor & 0xFF);
		IO::wait();
		IO::outb(Data0, (byte)(divisor >> 8));
		IO::wait();
	}

	uint32 GetFrequency()
	{
		return BaseFrequency / Divisor;
	}

	uint16 GetDivisor()
	{
		return Divisor;
	}

	byte AddTimer(vptr handler, fp64 intervalS)
	{
		if (CurrentTimers >= 0xFF) return 0;
		byte ret = 0;
		for (uint16 x = 0; x < 0xFF; x++)
		{
			if (!Timers[x].Active)
			{
				ret = x;
				break;
			}
		}
		
		TimerEntry* timer = Timers + ret;
		timer->CurrentCount = 0;
		timer->Handler = (void(*)())handler;
		timer->Interval = intervalS;
		timer->Active = true;

		CurrentTimers++;
		return ret;
	}

	byte AddTimer(vptr handler, uint32 intervalMs)
	{
		return AddTimer(handler, intervalMs / 1000.0);
	}

	void RemoveTimer(byte handler)
	{
		auto timer = Timers + handler;
		if (timer->Active)
		{
			// remove it
			timer->Active = false;
			timer->CurrentCount = 0;
			timer->Handler = (void(*)())0;
			timer->Interval = 0;
			CurrentTimers--;
		}
	}

	bool RemoveTimer(vptr handler)
	{
		for (uint16 x = 0; x < 0xFF; x++)
		{
			if ((vptr)(Timers[x].Handler) == handler)
			{
				RemoveTimer(x);
				return true;
			}
		}
		return false;
	}
}