#include <Speaker.h>
#include <IOBus.hpp>

using namespace IO;

namespace Audio
{
	constexpr uint16 SpeakerPort = 0x61;
	constexpr byte SpeakerBitsEnable = 0b11;

	Note Notes[] = {

		Note::D0,
		Note::Ds0,
		Note::E0,
		Note::F0,
		Note::Fs0,
		Note::G0,
		Note::Gs0,
		Note::A0,
		Note::As0,
		Note::B0,

		Note::C1,
		Note::Cs1,
		Note::D1,
		Note::Ds1,
		Note::E1,
		Note::F1,
		Note::Fs1,
		Note::G1,
		Note::Gs1,
		Note::A1,
		Note::As1,
		Note::B1,

		Note::C2,
		Note::Cs2,
		Note::D2,
		Note::Ds2,
		Note::E2,
		Note::F2,
		Note::Fs2,
		Note::G2,
		Note::Gs2,
		Note::A2,
		Note::As2,
		Note::B2,

		Note::C3,
		Note::Cs3,
		Note::D3,
		Note::Ds3,
		Note::E3,
		Note::F3,
		Note::Fs3,
		Note::G3,
		Note::Gs3,
		Note::A3,
		Note::As3,
		Note::B3,

		Note::C4,
		Note::Cs4,
		Note::D4,
		Note::Ds4,
		Note::E4,
		Note::F4,
		Note::Fs4,
		Note::G4,
		Note::Gs4,
		Note::A4,
		Note::As4,
		Note::B4,

		Note::C5,
		Note::Cs5,
		Note::D5,
		Note::Ds5,
		Note::E5,
		Note::F5,
		Note::Fs5,
		Note::G5,
		Note::Gs5,
		Note::A5,
		Note::As5,
		Note::B5,

		Note::C6,
		Note::Cs6,
		Note::D6,
		Note::Ds6,
		Note::E6,
		Note::F6,
		Note::Fs6,
		Note::G6,
		Note::Gs6,
		Note::A6,
		Note::As6,
		Note::B6,

		Note::C7,
		Note::Cs7,
		Note::D7,
		Note::Ds7,
		Note::E7,
		Note::F7,
		Note::Fs7,
		Note::G7,
		Note::Gs7,
		Note::A7,
		Note::As7,
		Note::B7,

		Note::C8,
		Note::Cs8,
		Note::D8,
		Note::Ds8,
		Note::E8,
		Note::F8,
		Note::Fs8,
		Note::G8,
		Note::Gs8,
		Note::A8,
		Note::As8,
		Note::B8,

	};

	Note Tetris[] = {



	};

	void EnableSpeaker()
	{
		byte temp = inb(SpeakerPort);
		outb(SpeakerPort, temp | SpeakerBitsEnable);
	}

	void DisableSpeaker()
	{
		byte temp = inb(SpeakerPort);
		outb(SpeakerPort, temp & (~SpeakerBitsEnable));
	}

	void Play(Note note)
	{
		using namespace PIT;
		SetChannel2(OperatingMode::SquareWave, (u16)note);
	}

	void Play(Note note, nint durationMs)
	{
		using namespace PIT;
		SetChannel2(OperatingMode::SquareWave, (u16)note);
		Sleepms(durationMs);
	}

	void Play(uint16 frequency)
	{
		using namespace PIT;
		SetChannel2(OperatingMode::SquareWave, (u16)(PIT::BaseFrequency / frequency));
	}

	void Play(uint16 frequency, nint durationMs)
	{
		using namespace PIT;
		SetChannel2(OperatingMode::SquareWave, (u16)(PIT::BaseFrequency / frequency));
		Sleepms(durationMs);
	}

	void Rest(nint ms)
	{
		using namespace PIT;
		SetChannel2(OperatingMode::SquareWave, (u16)Note::Unhearable);
		Sleepms(ms);
	}

}