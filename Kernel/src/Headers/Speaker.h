#pragma once
#ifndef H_Speaker
#define H_Speaker
#include <Globals.h>
#include <PIT.h>

namespace Audio
{
	// C0 / C#0 cannot be played
	enum struct Note : uint16
	{
		Null = 0x00,
		None = Null,

		D0 =  (u16)(PIT::BaseFrequency / 18.35),
		Ds0 = (u16)(PIT::BaseFrequency / 19.45),
		Ef0 = Ds0,
		E0 =  (u16)(PIT::BaseFrequency / 20.60),
		F0 =  (u16)(PIT::BaseFrequency / 21.83),
		Fs0 = (u16)(PIT::BaseFrequency / 23.12),
		Gf0 = Fs0,
		G0 =  (u16)(PIT::BaseFrequency / 24.50),
		Gs0 = (u16)(PIT::BaseFrequency / 23.12),
		Af0 = Gs0,
		A0 =  (u16)(PIT::BaseFrequency / 27.50),
		As0 = (u16)(PIT::BaseFrequency / 29.14),
		Bf0 = As0,
		B0 =  (u16)(PIT::BaseFrequency / 30.87),


		C1 = (u16)(PIT::BaseFrequency / 32.70),
		Cs1 = (u16)(PIT::BaseFrequency / 34.65),
		Df1 = Cs1,
		D1 = (u16)(PIT::BaseFrequency / 36.71),
		Ds1 = (u16)(PIT::BaseFrequency / 38.89),
		Ef1 = Ds1,
		E1 = (u16)(PIT::BaseFrequency / 41.20),
		F1 = (u16)(PIT::BaseFrequency / 43.65),
		Fs1 = (u16)(PIT::BaseFrequency / 46.25),
		Gf1 = Fs1,
		G1 = (u16)(PIT::BaseFrequency / 49.00),
		Gs1 = (u16)(PIT::BaseFrequency / 51.91),
		Af1 = Gs1,
		A1 = (u16)(PIT::BaseFrequency / 55.00),
		As1 = (u16)(PIT::BaseFrequency / 58.27),
		Bf1 = As1,
		B1 = (u16)(PIT::BaseFrequency / 61.74),


		C2 = (u16)(PIT::BaseFrequency / 65.41),
		Cs2 = (u16)(PIT::BaseFrequency / 69.30),
		Df2 = Cs2,
		D2 = (u16)(PIT::BaseFrequency / 73.42),
		Ds2 = (u16)(PIT::BaseFrequency / 77.78),
		Ef2 = Ds2,
		E2 = (u16)(PIT::BaseFrequency / 82.41),
		F2 = (u16)(PIT::BaseFrequency / 87.31),
		Fs2 = (u16)(PIT::BaseFrequency / 92.50),
		Gf2 = Fs2,
		G2 = (u16)(PIT::BaseFrequency / 98.00),
		Gs2 = (u16)(PIT::BaseFrequency / 103.83),
		Af2 = Gs2,
		A2 = (u16)(PIT::BaseFrequency / 110.00),
		As2 = (u16)(PIT::BaseFrequency / 116.54),
		Bf2 = As2,
		B2 = (u16)(PIT::BaseFrequency / 123.47),


		C3 = (u16)(PIT::BaseFrequency / 130.81),
		Cs3 = (u16)(PIT::BaseFrequency / 138.59),
		Df3 = Cs3,
		D3 = (u16)(PIT::BaseFrequency / 146.83),
		Ds3 = (u16)(PIT::BaseFrequency / 155.56),
		Ef3 = Ds3,
		E3 = (u16)(PIT::BaseFrequency / 164.81),
		F3 = (u16)(PIT::BaseFrequency / 174.61),
		Fs3 = (u16)(PIT::BaseFrequency / 185.00),
		Gf3 = Fs3,
		G3 = (u16)(PIT::BaseFrequency / 196.00),
		Gs3 = (u16)(PIT::BaseFrequency / 207.65),
		Af3 = Gs3,
		A3 = (u16)(PIT::BaseFrequency / 220.00),
		As3 = (u16)(PIT::BaseFrequency / 233.08),
		Bf3 = As3,
		B3 = (u16)(PIT::BaseFrequency / 246.94),


		C4 = (u16)(PIT::BaseFrequency / 261.63),
		Cs4 = (u16)(PIT::BaseFrequency / 277.18),
		Df4 = Cs4,
		D4 = (u16)(PIT::BaseFrequency / 293.66),
		Ds4 = (u16)(PIT::BaseFrequency / 311.13),
		Ef4 = Ds4,
		E4 = (u16)(PIT::BaseFrequency / 329.63),
		F4 = (u16)(PIT::BaseFrequency / 349.23),
		Fs4 = (u16)(PIT::BaseFrequency / 369.99),
		Gf4 = Fs4,
		G4 = (u16)(PIT::BaseFrequency / 392.00),
		Gs4 = (u16)(PIT::BaseFrequency / 415.30),
		Af4 = Gs4,
		A4 = (u16)(PIT::BaseFrequency / 440.00),
		As4 = (u16)(PIT::BaseFrequency / 466.16),
		Bf4 = As4,
		B4 = (u16)(PIT::BaseFrequency / 493.88),


		C5 = (u16)(PIT::BaseFrequency / 523.25),
		Cs5 = (u16)(PIT::BaseFrequency / 554.37),
		Df5 = Cs5,
		D5 = (u16)(PIT::BaseFrequency / 587.33),
		Ds5 = (u16)(PIT::BaseFrequency / 622.25),
		Ef5 = Ds5,
		E5 = (u16)(PIT::BaseFrequency / 659.25),
		F5 = (u16)(PIT::BaseFrequency / 698.46),
		Fs5 = (u16)(PIT::BaseFrequency / 739.99),
		Gf5 = Fs5,
		G5 = (u16)(PIT::BaseFrequency / 783.99),
		Gs5 = (u16)(PIT::BaseFrequency / 830.61),
		Af5 = Gs5,
		A5 = (u16)(PIT::BaseFrequency / 880.00),
		As5 = (u16)(PIT::BaseFrequency / 932.33),
		Bf5 = As5,
		B5 = (u16)(PIT::BaseFrequency / 987.77),


		C6 = (u16)(PIT::BaseFrequency / 1046.50),
		Cs6 = (u16)(PIT::BaseFrequency / 1108.73),
		Df6 = Cs6,
		D6 = (u16)(PIT::BaseFrequency / 1174.66),
		Ds6 = (u16)(PIT::BaseFrequency / 1244.51),
		Ef6 = Ds6,
		E6 = (u16)(PIT::BaseFrequency / 1318.51),
		F6 = (u16)(PIT::BaseFrequency / 1396.91),
		Fs6 = (u16)(PIT::BaseFrequency / 1479.98),
		Gf6 = Fs6,
		G6 = (u16)(PIT::BaseFrequency / 1567.98),
		Gs6 = (u16)(PIT::BaseFrequency / 1661.22),
		Af6 = Gs6,
		A6 = (u16)(PIT::BaseFrequency / 1760.00),
		As6 = (u16)(PIT::BaseFrequency / 1864.66),
		Bf6 = As6,
		B6 = (u16)(PIT::BaseFrequency / 1975.53),


		C7 = (u16)(PIT::BaseFrequency / 2093.00),
		Cs7 = (u16)(PIT::BaseFrequency / 2217.46),
		Df7 = Cs7,
		D7 = (u16)(PIT::BaseFrequency / 2349.32),
		Ds7 = (u16)(PIT::BaseFrequency / 2489.02),
		Ef7 = Ds7,
		E7 = (u16)(PIT::BaseFrequency / 2637.02),
		F7 = (u16)(PIT::BaseFrequency / 2793.83),
		Fs7 = (u16)(PIT::BaseFrequency / 2959.96),
		Gf7 = Fs7,
		G7 = (u16)(PIT::BaseFrequency / 3135.96),
		Gs7 = (u16)(PIT::BaseFrequency / 3322.44),
		Af7 = Gs7,
		A7 = (u16)(PIT::BaseFrequency / 3520.00),
		As7 = (u16)(PIT::BaseFrequency / 3729.31),
		Bf7 = As7,
		B7 = (u16)(PIT::BaseFrequency / 3951.07),


		C8 = (u16)(PIT::BaseFrequency / 4186.01),
		Cs8 = (u16)(PIT::BaseFrequency / 4434.92),
		Df8 = Cs8,
		D8 = (u16)(PIT::BaseFrequency / 4698.63),
		Ds8 = (u16)(PIT::BaseFrequency / 4978.03),
		Ef8 = Ds8,
		E8 = (u16)(PIT::BaseFrequency / 5274.04),
		F8 = (u16)(PIT::BaseFrequency / 5587.65),
		Fs8 = (u16)(PIT::BaseFrequency / 5919.91),
		Gf8 = Fs8,
		G8 = (u16)(PIT::BaseFrequency / 6271.93),
		Gs8 = (u16)(PIT::BaseFrequency / 6644.88),
		Af8 = Gs8,
		A8 = (u16)(PIT::BaseFrequency / 7040.00),
		As8 = (u16)(PIT::BaseFrequency / 7458.62),
		Bf8 = As8,
		B8 = (u16)(PIT::BaseFrequency / 7902.13),

		Rest = 40,
	};

	struct NoteSpan
	{
		Note Key;
		uint16 DurationMS;
	};

	extern Note Notes[];
	extern NoteSpan NotTetris[];

	void EnableSpeaker();
	void DisableSpeaker();

	void Play(Note note, nint ms);
	void Play(uint16 frequency, nint ms);

	void PlayTrack(NoteSpan* track);

	void Rest(nint ms);

}

#endif