#include <cstr.h>
#include <MemoryUtils.hpp>

namespace cstr
{
	char hexBuffer[32];
	char strBuffer[32];
	char revBuffer[256];

	uint64 len(const char* str)
	{
		uint64 ln = 0;
		while (str[ln]) ln++;
		return ln;
	}

	void Reverse(char* str, uint64 length)
	{
		if (!length) length = len(str);
		if (length > 255) return;

		byte index = 0;
		byte reverseIndex = length;

		while (reverseIndex)
		{
			revBuffer[--reverseIndex] = str[index++];
		}

		memcpy(revBuffer, str, length);

	}

	inline char* _ToStr(int64 num)
	{
		// Zero out the buffer
		memset<uint64>(strBuffer, 0, 32);

		char* buffer = strBuffer;
		if (num == 0)
		{
			buffer[0] = '0';
			return strBuffer;
		}
		if (num < 0)
		{
			strBuffer[0] = '-';
			buffer = strBuffer + 1;
			num *= -1;
		}

		byte index = 0;
		while (num != 0)
		{
			buffer[index++] = (num % 10) + 0x30;
			num /= 10;
		}

		Reverse(buffer);
		return strBuffer;
	}

	inline char* _ToHex(uint64 num)
	{
		memset<uint64>(hexBuffer, 0, 32);
		hexBuffer[0] = '0';
		hexBuffer[1] = 'x';
		char* buffer = hexBuffer + 2;

		if (num == 0)
		{
			buffer[0] = '0';
			buffer[1] = '0';
			return hexBuffer;
		}
		if (num < 0x10)
		{
			buffer[1] = '0';
		}

		byte index = 0;
		// the lazy way to do this but oh well
		while (num != 0)
		{
			byte val = (num % 0x10);
			buffer[index++] = (val > 9) ? ((val) + 0x37) : val + 0x30;
			num /= 0x10;
		}

		Reverse(buffer);
		return hexBuffer;
	}

	char* ToString(int64 num, bool hex)
	{
		if (hex) return _ToHex(num);
		return _ToStr(num);
	}

	int64 ToInt(const char* str, bool hex)
	{
		return 0;
	}

}
