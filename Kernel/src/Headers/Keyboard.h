#pragma once
#ifndef H_Keyboard
#define H_Keyboard
#include <Globals.h>

namespace IO
{
	namespace Keyboard
	{
		constexpr uint16 KBPort = 0x60;

		extern char Key1Table[];

		enum struct Command : byte
		{
			SetLED = 0xED,
			Echo = 0xEE,

			GetSetScancodeSet = 0xF0,
			Identify = 0xF2,
			SetRepeatRateDelay = 0xF3,

			EnableScan = 0xF4,
			DisableScan = 0xF5,

			SetDefault = 0xF6,

			SetAllMaticAuto = 0xF7,
			SetAllMakeRelease = 0xF8,
			SetAllMake = 0xF9,
			SetAllMaticAutoMakeRelease = 0xFA,

			SetKeyMaticAuto = 0xFB,
			SetKeyMakeRelease = 0xFC,
			SetKeyMake = 0xFD,

			Resend = 0xFE,
			Reset = 0xFF
		};

		namespace Response
		{
			constexpr byte ERR0 = 0x00;			// Key detection error or internal buffer overrun
			constexpr byte ERR1 = 0xFF;			// Key detection error or internal buffer overrun

			constexpr byte TESTPASS = 0xAA;		// Self test passed (sent after "0xFF (reset)" command or keyboard power up)
			constexpr byte ECHO = 0xEE;			// Response to "0xEE (echo)" command
			constexpr byte ACK = 0xFA;			// Command acknowledged (ACK)
			constexpr byte RESEND = 0xFE;		// Resend (keyboard wants controller to repeat last command it sent)

			constexpr byte TESTFAIL1 = 0xFC;	// Self test failed (sent after "0xFF (reset)" command or keyboard power up)
			constexpr byte TESTFAIL2 = 0xFD;	// Self test failed (sent after "0xFF (reset)" command or keyboard power up)
		}

		namespace PS2Key1
		{
			enum : uint16
			{
				Null = 0x00,

				Escape = 0x01,

				One = 0x02,
				Two = 0x03,
				Three = 0x04,
				Four = 0x05,
				Five = 0x06,
				Six = 0x07,
				Seven = 0x08,
				Eight = 0x09,
				Nine = 0x0A,
				Zero = 0x0B,

				Minus = 0x0C,
				Equals = 0x0D,
				Backspace = 0x0E,

				Tab = 0x0F,
				Q = 0x10,
				W = 0x11,
				E = 0x12,
				R = 0x13,
				T = 0x14,
				Y = 0x15,
				U = 0x16,
				I = 0x17,
				O = 0x18,
				P = 0x19,
				SquareBracketLeft = 0x1A,
				SquareBracketRight = 0x1B,

				Enter = 0x1C,
				LControl = 0x1D,
				A = 0x1E,
				S = 0x1F,
				D = 0x20,
				F = 0x21,
				G = 0x22,
				H = 0x23,
				J = 0x24,
				K = 0x25,
				L = 0x26,
				Semicolon = 0x27,
				SingleQuote = 0x28,
				BackTick = 0x29,

				LShift = 0x2A,
				BackSlash = 0x2B,
				Z = 0x2C,
				X = 0x2D,
				C = 0x2E,
				V = 0x2F,
				B = 0x30,
				N = 0x31,
				M = 0x32,
				Comma = 0x33,
				Period = 0x34,
				ForwardSlash = 0x35,
				RShift = 0x36,
				KPStar = 0x37,

				LAlt = 0x38,
				Space = 0x39,
				Capslock = 0x3A,
				F1 = 0x3B,
				F2 = 0x3C,
				F3 = 0x3D,
				F4 = 0x3E,
				F5 = 0x3F,
				F6 = 0x40,
				F7 = 0x41,
				F8 = 0x42,
				F9 = 0x43,
				F10 = 0x44,

				NumLock = 0x45,
				ScrollLock = 0x46,
				KP7 = 0x47,
				KP8 = 0x48,
				KP9 = 0x49,
				KPMinus = 0x4A,
				KP4 = 0x4B,
				KP5 = 0x4C,
				KP6 = 0x4D,
				KPPlus = 0x4E,
				KP1 = 0x4F,
				KP2 = 0x50,
				KP3 = 0x51,
				KP0 = 0x52,
				KPDot = 0x53,

				F11 = 0x57,
				F12 = 0x58,

			};
		}

		namespace PS2Key2
		{
			enum : uint16
			{
				Null = 0x00,

				Escape = 0x76,

				One = 0x16,
				Two = 0x1E,
				Three = 0x26,
				Four = 0x25,
				Five = 0x2E,
				Six = 0x36,
				Seven = 0x3D,
				Eight = 0x3E,
				Nine = 0x46,
				Zero = 0x45,

				Minus = 0x4E,
				Equals = 0x55,
				Backspace = 0x66,

				Tab = 0x0D,
				Q = 0x15,
				W = 0x1D,
				E = 0x24,
				R = 0x2D,
				T = 0x2C,
				Y = 0x35,
				U = 0x3C,
				I = 0x43,
				O = 0x44,
				P = 0x4D,
				SquareBracketLeft = 0x54,
				SquareBracketRight = 0x5B,

				Enter = 0x5A,
				LControl = 0x14,
				A = 0x1C,
				S = 0x1B,
				D = 0x23,
				F = 0x2B,
				G = 0x34,
				H = 0x33,
				J = 0x3B,
				K = 0x42,
				L = 0x4B,
				Semicolon = 0x4C,
				SingleQuote = 0x52,
				BackTick = 0x0E,

				LShift = 0x12,
				BackSlash = 0x5D,
				Z = 0x1A,
				X = 0x22,
				C = 0x21,
				V = 0x2A,
				B = 0x32,
				N = 0x31,
				M = 0x3A,
				Comma = 0x41,
				Period = 0x49,
				ForwardSlash = 0x4A,
				RShift = 0x59,
				KPStar = 0x7C,

				LAlt = 0x11,
				Space = 0x29,
				Capslock = 0x58,
				F1 = 0x05,
				F2 = 0x06,
				F3 = 0x04,
				F4 = 0x0C,
				F5 = 0x03,
				F6 = 0x0B,
				F7 = 0x83,
				F8 = 0x0A,
				F9 = 0x01,
				F10 = 0x09,

				NumLock = 0x77,
				ScrollLock = 0x7E,
				KP7 = 0x6C,
				KP8 = 0x75,
				KP9 = 0x7D,
				KPMinus = 0x7B,
				KP4 = 0x6B,
				KP5 = 0x73,
				KP6 = 0x74,
				KPPlus = 0x79,
				KP1 = 0x69,
				KP2 = 0x72,
				KP3 = 0x7A,
				KP0 = 0x70,
				KPDot = 0x71,

				F11 = 0x78,
				F12 = 0x07

			};
		}

		void KeyEvent();

		bool GetKeyState(byte Key);

		byte SendCommand(Command cmd, byte* data, byte datacount);

	}
}

#endif // !H_Keyboard