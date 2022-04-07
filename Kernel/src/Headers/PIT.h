#ifndef H_PIT
#define H_PIT
#include <Types.h>
#include <Globals.h>

namespace PIT
{

	namespace OperatingMode
	{
		enum
		{
			InterruptTerminal = 0b000,
			OneShot = 0b001,
			RateGenerator = 0b010,
			SquareWave = 0b011,
			SoftStrobe = 0b100,
			HardStrobe = 0b101,
			RateGenerator0 = 0b110,
			SquareWave0 = 0b111,
		};
	}

	struct TimerEntry
	{
		void (*Handler)();
		fp64 Interval;
		fp64 CurrentCount;
		bool Active = false;
	};

	constexpr uint32 BaseFrequency = 1193182;

	void Initialize();
	void Tick();

	void Sleep(fp64 seconds);
	forceinline void SleepMS(nint miliseconds) { Sleep(miliseconds / 1000.0); }

	void SetFrequency(uint32 freq);
	void SetDivisor(uint16 divisor);

	uint32 GetFrequency();
	uint16 GetDivisor();

	byte AddTimer(vptr handler, fp64 intervalS);
	byte AddTimer(vptr handler, uint32 intervalMs);

	void RemoveTimer(byte ID);
	bool RemoveTimer(vptr handler);

	void SetChannel2(const byte mode, const uint16 freq);
}

#endif