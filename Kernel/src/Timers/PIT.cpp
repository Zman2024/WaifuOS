#include <PIT.h>
#include <IOBus.hpp>

using namespace IO;

namespace PIT
{
	// PIT IO ports
	constexpr byte Data0 = 0x40; // PIT IRQ Channel
	constexpr byte Data1 = 0x41; // Legacy DRAM Refresh
	constexpr byte Data2 = 0x42; // PC Speaker
	constexpr byte Command0 = 0x43;

	uint16 Divisor = 0x00;
	fp64 TimeSinceInit = 0x00;

	TimerEntry Timers[0xFF];
	uint16 CurrentTimers = 0x00;

	void Initialize()
	{
		TimeSinceInit = 0x00;
		Divisor = 0x00;
		CurrentTimers = 0x00;
		SetFrequency(100);
		memset64(Timers, 0, sizeof(Timers));
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

	void SetFrequency(uint32 freq)
	{
		SetDivisor(BaseFrequency / freq);
	}

	void SetDivisor(uint16 divisor)
	{
		Divisor = divisor;
		outb(Data0, (byte)divisor & 0xFF);
		wait();
		outb(Data0, (byte)(divisor >> 8));
		wait();
	}

	uint32 GetFrequency()
	{
		return BaseFrequency / Divisor;
	}

	uint16 GetDivisor()
	{
		return Divisor;
	}

	fp64 GetTimeSinceInit() { return TimeSinceInit; }

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

	void SetChannel2(const byte mode, const uint16 freq)
	{
		outb(Command0, (1 << 7) | (0x3 << 4) | (mode << 1));
		outb(Data2, (byte)freq);
		outb(Data2, (byte)(freq >> 8));
	}
}