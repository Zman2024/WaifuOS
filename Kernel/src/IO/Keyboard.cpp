#include <Keyboard.h>
#include <IOBus.hpp>

namespace IO
{
	namespace Keyboard
	{
		byte resendCount = 0x00;
		byte cmdResponse = 0x69;
		byte prevCommand = 0x00;
		bool SendingCommand = false;

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

			switch (sc1)
			{
				case (u16)PS2Key1::A:
					gConsole.Write("a");
					break;
			}

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
