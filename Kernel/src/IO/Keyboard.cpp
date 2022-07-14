#include <Keyboard.h>
#include <IOBus.hpp>
#include <Interrupts.h>

namespace IO
{
	namespace Keyboard
	{
		byte resendCount = 0x00;
		byte cmdResponse = 0x00;
		byte prevCommand = 0x00;
		bool SendingCommand = false;

		bool LShift = false;
		bool RShift = false;

		bool LControl = false;
		bool RControl = false;

		bool LAlt = false;
		bool RAlt = false;

		bool CapsLock = false;
		bool NumLock = false;

		#define SHIFT (LShift | RShift)
		#define CTRL (LControl | RControl)
		#define ALT (LAlt | RAlt)

		bool KeyState[0x80];

		char Key1Table[] = {

			0x00, 0x00, '1', '2', '3', '4', '5', '6',
			'7', '8', '9', '0', '-', '=', '\b', '\t',
			'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
			'o', 'p', '[', ']', '\n', 0x00, 'a', 's',
			'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
			'\'', '`', 0x00, '\\', 'z', 'x', 'c', 'v',
			'b', 'n', 'm', ',', '.', '/', '\0', '*',
			0x00, ' ', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, '7',
			'8', '9', '-', '4', '5', '6', '+', '1',
			'2', '3', '0', '.', 0x00, 0x00, 0x00, 0x00, 
			0x00,
		};

		char ShiftKey1Table[] = {

			0x00, 0x00, '!', '@', '#', '$', '%', '^',
			'&', '*', '(', ')', '_', '+', '\b', 0x00,
			'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
			'O', 'P', '{', '}', '\n', 0x00, 'A', 'sS',
			'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
			'"', '~', 0x00, '|', 'Z', 'X', 'C', 'V',
			'B', 'N', 'M', '<', '>', '?', 0x00, '*',
			0x00, ' ', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, '7',
			'8', '9', '-', '4', '5', '6', '+', '1',
			'2', '3', '0', '.', 0x00, 0x00, 0x00, 0x00,
			0x00,
		};

		char CapsKey1Table[] = {

			0x00, 0x00, '1', '2', '3', '4', '5', '6',
			'7', '8', '9', '0', '-', '=', '\b', '\t',
			'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
			'O', 'P', '[', ']', '\n', 0x00, 'A', 'sS',
			'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',
			'\'', '`', 0x00, '\\', 'Z', 'X', 'C', 'V',
			'B', 'N', 'M', ',', '.', '/', '\0', '*',
			0x00, ' ', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, '7',
			'8', '9', '-', '4', '5', '6', '+', '1',
			'2', '3', '0', '.', 0x00, 0x00, 0x00, 0x00,
			0x00,
		};

		char CapsShiftKey1Table[] = {

			0x00, 0x00, '!', '@', '#', '$', '%', '^',
			'&', '*', '(', ')', '_', '+', '\b', 0x00,
			'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
			'o', 'p', '{', '}', '\n', 0x00, 'a', 's',
			'd', 'f', 'g', 'h', 'j', 'k', 'l', ':',
			'"', '~', 0x00, '|', 'z', 'x', 'c', 'v',
			'b', 'n', 'm','<', '>', '?', 0x00, '*',
			0x00, ' ', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, '7',
			'8', '9', '-', '4', '5', '6', '+', '1',
			'2', '3', '0', '.', 0x00, 0x00, 0x00, 0x00,
			0x00,
		};

		bool GetKeyState(byte Key)
		{
			Key &= 0x7F;
			return KeyState[Key];
		}

		void SetKeyState(byte Key, bool state)
		{
			Key &= 0x7F;
			KeyState[Key] = state;
		}

		bool Resend()
		{
			debug("Resending keyboard command...");
			if (resendCount++ > 2)
			{
				resendCount = 0x00;
				return false;
			}

			// Resend previous command
			outb(KBPort, prevCommand);
		}

		void KeyUpEvent(byte sc)
		{
			SetKeyState(sc, false);

			switch (sc)
			{
				case PS2Key1::LShift:
					LShift = false;
					return;

				case PS2Key1::RShift:
					RShift = false;
					return;

				case PS2Key1::LControl:
					LControl = false;
					return;

				case PS2Key1::LAlt:
					LAlt = false;
					return;
			}
		}

		void KeyDownEvent(byte sc)
		{
			SetKeyState(sc, true);

			switch (sc)
			{
				case PS2Key1::LShift:
					LShift = true;
					return;

				case PS2Key1::RShift:
					RShift = true;
					return;

				case PS2Key1::LControl:
					LControl = true;
					return;

				case PS2Key1::LAlt:
					LAlt = true;
					return;

				case PS2Key1::Capslock:
					CapsLock =! CapsLock;
					return;

				case PS2Key1::Backspace:
					gConsole.WriteChar('\b');
					gConsole.WriteChar(' ');
					gConsole.WriteChar('\b');
					return;

				case PS2Key1::Escape:
					gConsole.Clear();
					return;

				case PS2Key1::F12:
					intcall(Interrupts::Interrupt::MachineCheck);
					return;

			}

			if (!CTRL & !ALT)
			{
				if ((SHIFT & !CapsLock)) gConsole.WriteChar(ShiftKey1Table[sc]);
				else if (CapsLock & !SHIFT) gConsole.WriteChar(CapsKey1Table[sc]);
				else if (CapsLock & SHIFT) gConsole.WriteChar(CapsShiftKey1Table[sc]);
				else gConsole.WriteChar(Key1Table[sc]);
			}
		}

		void KeyEvent()
		{
			byte sc1 = inb(KBPort), sc2 = 0x00, sc3 = 0x00, sc4 = 0x00;
			if (SendingCommand)
			{
				switch (sc1)
				{
					case Response::ERR0:
					case Response::ERR1:
						error("THE KEYBOARD SENT AN ERROR!");
						cmdResponse = Response::ERR1;
						return;

					case Response::TESTFAIL1:
					{
						sc2 = inb(KBPort);
						switch (sc2)
						{
						case Response::TESTFAIL2:
							error("KEYBOARD SELF TEST FAILED!");
							cmdResponse = sc2;
							return;
						}
						cmdResponse = sc1;
						return;
					}

					case Response::ECHO:
						debug("Keyboard Echoed");
						cmdResponse = sc1;
						return;

					case Response::RESEND:
						cmdResponse = sc1;
						return;

					case Response::ACK:
						resendCount = 0x00;
						debug("Keyboard ACK");
						cmdResponse = sc1;
						return;

					case Response::TESTPASS:
						debug("Keyboard self test passed");
						cmdResponse = sc1;
						return;
				}
				cmdResponse = sc1;
				return;
			}

			// msb means keyup for ScanSet1
			if (sc1 & 1 << 7) KeyUpEvent(sc1 & 0b01111111);
			else KeyDownEvent(sc1);

		}

		byte SendCommand(Command cmd, byte* data, byte datacount)
		{
			int16 index = 0;
			cmdResponse = 0x00;

			prevCommand = (byte)cmd;
			SendingCommand = true;
			outb(KBPort, (byte)cmd);

			// wait for the keyboard to respond
			while (index > datacount)
			{
				while (!cmdResponse)
				{
					hlt;
				}

				switch (cmdResponse)
				{
					case Response::ERR0:
					case Response::ERR1:
						SendingCommand = false;
						return Response::ERR1;


					case Response::TESTFAIL1:
					case Response::TESTFAIL2:
						SendingCommand = false;
						return Response::TESTFAIL2;

					case Response::ECHO:
						SendingCommand = false;
						return Response::ECHO;

					case Response::RESEND:
					{
						if (!Resend())
						{
							cmdResponse = 0x00;
							prevCommand = 0x00;
							SendingCommand = false;
							return Response::ERR1;
						}
					}

					case Response::ACK:
						break;

					case Response::TESTPASS:
						SendingCommand = false;
						return Response::TESTPASS;

				}

				cmdResponse = 0x00;
				resendCount = 0x00;
				prevCommand = data[index];
				outb(KBPort, data[index]);
				index++;
			}

			while (!cmdResponse)
			{
				hlt;
			}

			SendingCommand = false;
			return cmdResponse;
		}

	}
}
