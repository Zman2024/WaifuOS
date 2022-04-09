#ifndef H_string
#define H_string
#include <Types.h>

// TODO: optimize addition so we only create a new buffer
// when we exceed the real 16 byte malloc buffer
class string
{
protected:
	char* mBuffer = nullptr;
	uint64 mBufferSize = 0x00;

public:

	string();
	string(uint64 length);
	string(const char* str);
	string(const string& str);
	string(const char* str, nint length);

	forceinline string(const byte* str) : string((const char*)str) { }
	forceinline string(const byte* str, nint length) : string((const char*)str, length) {}

	string append(const string&& rvalue);
	string append(const string& rvalue);
	string append(const char* rvalue);

	string prepend(const char* str);

	string& operator=(const string&& value);
	string& operator=(const string& value);
	string& operator=(const char* value);

	forceinline string operator+(const string&& rvalue) const { return this->append(rvalue); }
	forceinline string operator+(const string& rvalue) const { return this->append(rvalue); }
	forceinline string operator+(const char* rvalue) const { return this->append(rvalue); }

	forceinline operator const char*() { return this->mBuffer; }

	// returns the size of the allocated string buffer
	forceinline nint GetBufferSize() { return this->mBufferSize; }

	// returns the numbers of characters in this string (excl nul)
	forceinline nint GetLength() { return this->mBufferSize - 1; }

	~string();

};

forceinline string operator+(const char* left, const string& right)
{
	return right.prepend(left);
}

// string.asm
global nint strlen(const char* str);
global void strcpy(char* dest, const char* src);
global void strncpy(char* dest, const char* src, nint max);

#endif