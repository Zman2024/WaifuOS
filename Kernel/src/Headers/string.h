#ifndef H_string
#define H_string
#include <Types.h>

class string
{
protected:
	char* mBuffer = nullptr;
	uint64 mLength = 0x00;

public:

	string();
	string(const char* str);

	string& operator = (string&& value) noexcept;

	string& operator = (const string& value) noexcept;

	string& operator=(const char* value) noexcept;

	inline operator const char*() { return this->mBuffer; }

	~string();

};

#endif