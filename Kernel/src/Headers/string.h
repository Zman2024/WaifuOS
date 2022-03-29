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

	string append(const string&& rvalue);
	string append(const string& rvalue);
	string append(const char* rvalue);

	string prepend(const char* str);

	string& operator=(const string&& value);
	string& operator=(const string& value);
	string& operator=(const char* value);

	inline string operator+(const string&& rvalue) const { return this->append(rvalue); }
	inline string operator+(const string& rvalue) const { return this->append(rvalue); }
	inline string operator+(const char* rvalue) const { return this->append(rvalue); }

	inline operator const char*() { return this->mBuffer; }

	// returns the size of the allocated string buffer
	inline nint GetBufferSize() { return this->mBufferSize; }

	// returns the numbers of characters in this string (excl nul)
	inline nint GetLength() { return this->mBufferSize - 1; }

	~string();

};

inline string operator+(const char* left, const string& right)
{
	return right.prepend(left);
}

#endif