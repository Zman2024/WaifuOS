#ifndef H_PIT
#define H_PIT
#include <Types.h>
#include <Globals.h>

namespace PIT
{
	struct TimerEntry
	{
		void (*Handler)();
		fp64 Interval;
		fp64 CurrentCount;
		bool Active = false;
	};

	void Initialize();
	void Tick();

	void Sleep(fp64 seconds);
	void Sleep(uint64 miliseconds);

	void SetFrequency(uint32 freq);
	void SetDivisor(uint16 divisor);

	uint32 GetFrequency();
	uint16 GetDivisor();

	byte AddTimer(vptr handler, fp64 intervalS);
	byte AddTimer(vptr handler, uint32 intervalMs);
	void RemoveTimer(byte ID);
	bool RemoveTimer(vptr handler);
}

#endif