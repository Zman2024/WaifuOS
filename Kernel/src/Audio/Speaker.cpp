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
	NoteSpan NotHoshizora[] = {
		/*
		// Pickup before first measure
		{Note::C5, 200},
		{Note::D5, 200},
		// m1
		{Note::E5, 1600},
		// m2
		{Note::F5, 400},
		{Note::E5, 400},
		{Note::F5, 400},
		{Note::A5, 400},
		// m3
		{Note::G5, 800},
		{Note::F5, 800},
		// m4
		{Note::E5, 700}, {Note::Rest, 100},
		{Note::E5, 400},
		{Note::D5, 400},
		// m5
		{Note::C5, 800},
		{Note::E5, 800},
		// m6
		{Note::D5, 1200},
		{Note::A4, 400},
		// m7
		{Note::B4, 1200},
		{Note::C5, 400},
		// m8
		{Note::D5, 1200},
		// end
		*/
		{Note::Null, 0},



	};
	NoteSpan NotTetris[] = {
		/* Uncomment for (not)tetris, I fear DMCA like JDH


		// Main Melody
		{ Note::E4, 400 },
		
		{ Note::B3, 200 },
		{ Note::C4, 200 },
		
		{ Note::D4, 400 },
		
		{ Note::C4, 200 },
		{ Note::B3, 200 },
		
		{ Note::A3, 390 }, { Note::Rest, 10 },
		
		{ Note::A3, 200 },
		{ Note::C4, 200 },
		
		{ Note::E4, 400 },
		
		{ Note::D4, 200 },
		{ Note::C4, 200 },
		
		{ Note::B3, 590 }, { Note::Rest, 10 },
		
		{ Note::C4, 200 },
		{ Note::D4, 400 },
		{ Note::E4, 400 },
		{ Note::C4, 400 },
		
		{ Note::A3, 390 }, { Note::Rest, 10 },
		{ Note::A3, 400 }, { Note::Rest, 600 },

		{ Note::D4, 400 },
		{ Note::F4, 200 },
		{ Note::A4, 400 },
		{ Note::G4, 200 },
		{ Note::F4, 200 },
		{ Note::E4, 400 }, { Note::Rest, 200 },

		{ Note::C4, 200 },
		{ Note::E4, 400 },
		{ Note::D4, 200 },
		{ Note::C4, 200 },
		{ Note::B3, 390 }, { Note::Rest, 10 },
		{ Note::B3, 200 },

		{ Note::C4, 200 },
		{ Note::D4, 400 },
		{ Note::E4, 400 },
		{ Note::C4, 400 },
		{ Note::A3, 390 }, { Note::Rest, 10 },
		{ Note::A3, 400 }, // { Note::Rest, 10 },

		{ Note::Rest, 400 },
		
		// Main Melody
		{ Note::E4, 400 },

		{ Note::B3, 200 },
		{ Note::C4, 200 },

		{ Note::D4, 400 },

		{ Note::C4, 200 },
		{ Note::B3, 200 },

		{ Note::A3, 390 }, { Note::Rest, 10 },

		{ Note::A3, 200 },
		{ Note::C4, 200 },

		{ Note::E4, 400 },

		{ Note::D4, 200 },
		{ Note::C4, 200 },

		{ Note::B3, 590 }, { Note::Rest, 10 },

		{ Note::C4, 200 },
		{ Note::D4, 400 },
		{ Note::E4, 400 },
		{ Note::C4, 400 },

		{ Note::A3, 390 }, { Note::Rest, 10 },
		{ Note::A3, 400 }, { Note::Rest, 600 },

		{ Note::D4, 400 },
		{ Note::F4, 200 },
		{ Note::A4, 400 },
		{ Note::G4, 200 },
		{ Note::F4, 200 },
		{ Note::E4, 400 }, { Note::Rest, 200 },

		{ Note::C4, 200 },
		{ Note::E4, 400 },
		{ Note::D4, 200 },
		{ Note::C4, 200 },
		{ Note::B3, 390 }, { Note::Rest, 10 },
		{ Note::B3, 200 },

		{ Note::C4, 200 },
		{ Note::D4, 400 },
		{ Note::E4, 400 },
		{ Note::C4, 400 },
		{ Note::A3, 390 }, { Note::Rest, 10 },
		{ Note::A3, 400 },

		{ Note::Rest, 400 },

		// Slower ending part
		{ Note::E4, 800 },
		{ Note::C4, 800 },

		{ Note::D4, 800 },
		{ Note::B3, 800 },

		{ Note::C4, 800 },
		{ Note::A3, 800 },

		{ Note::Gs3, 800 },
		{ Note::B3, 600 }, { Note::Rest, 200 },

		{ Note::E4, 800 },
		{ Note::C4, 800 },

		{ Note::D4, 800 },
		{ Note::B3, 800 },

		{ Note::C4, 400 },
		{ Note::E4, 400 },
		{ Note::A4, 800 },
		{ Note::Gs4, 800 },
		
	//	*/


		// end note
		{ Note::Rest, 800 },
		{ Note::Null, 0 },

	};

	void EnableSpeaker()
	{
		PIT::SetChannel2(PIT::OperatingMode::SquareWave, (u16)Note::Rest);
		byte temp = inb(SpeakerPort);
		outb(SpeakerPort, temp | SpeakerBitsEnable);
		wait();
	}

	void DisableSpeaker()
	{
		byte temp = inb(SpeakerPort);
		outb(SpeakerPort, temp & (~SpeakerBitsEnable));
		wait();
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
		SleepMS(durationMs);
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
		SleepMS(durationMs);
	}

	void PlayTrack(NoteSpan* track)
	{
		EnableSpeaker();
		while (track->Key != Note::Null)
		{
			Play(track->Key, track->DurationMS);
			track++;
		}
		DisableSpeaker();
	}

	void Rest(nint ms)
	{
		using namespace PIT;
		SetChannel2(OperatingMode::SquareWave, (u16)Note::Rest);
		SleepMS(ms);
	}

}