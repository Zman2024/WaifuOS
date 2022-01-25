#include <cstr.h>
#include <MemoryUtils.hpp>

namespace cstr
{
	char hexBuffer[32];
	char strBuffer[32];
	char revBuffer[256];

	uint64 Length(const char* str)
	{
		uint64 ln = 0;
		while (str[ln]) ln++;
		return ln;
	}

	bool strcmp(const char* str0, const char* str1)
	{
		u64 len = Length(str0);
		if (len != Length(str1)) return false;

		return strcmp(str0, str1, len);
	}

	bool strcmp(const char* str0, const char* str1, u64 len)
	{
		for (u64 x = 0; x < len; x++)
			if (str0[x] != str1[x]) return false;

		return true;
	}

	void Reverse(char* str, uint64 length)
	{
		if (!length) length = Length(str);
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

	int64 _ToIntBase10(const char* str)
	{
		int64 value = 0;
		bool isNegative = str[0] == '-';
		const char* buffer = str;

		if (isNegative) buffer += 1;

		u64 length = Length(buffer);

		for (u64 x = 0; x < length; x++)
		{
			value *= 10;
			value += (buffer[x] - 0x30);
		}

		if (isNegative) value *= -1;

		return value;
	}

	int64 _ToIntBase16(const char* str)
	{
		int64 value = 0;
		const char* buffer = str;

		if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
		{
			buffer = str + 2;
		}

		u64 length = Length(buffer);

		for (u64 x = 0; x < length; x++)
		{
			value *= 0x10;
			value += (buffer[x] > 0x39 ? buffer[x] - 0x37 : buffer[x] - 0x30);
		}

		return value;
	}

	int64 ToInt(const char* str, bool hex)
	{
		if (hex) return _ToIntBase16(str);
		return _ToIntBase10(str);
	}

}
