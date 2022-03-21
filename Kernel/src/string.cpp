#include <string.h>
#include <cstr.h>
#include <Memory.h>
#include <MemoryUtils.hpp>

vptr calloc(int x)
{

}

void free(vptr ptr)
{

}

string::string()
{
	mBuffer = (char*)calloc(0x10);
	mLength = 0x10;
}

string::string(const char* str)
{
	mLength = cstr::Length(str) + 1;
	mBuffer = (char*)calloc(mLength);
	memcpy(str, mBuffer, mLength);
}

// eg:
// string s = string("hello");
// s = string("world"); <--- here
string & string::operator=(string && value)
{
	auto right = &(value);
	if (right != this)
	{
		// destroy this
		this->~string();

		// copy rvalue to this
		memcpy(right, (vptr)this, sizeof(string));

		// stop destruction of rvalue
		memset(right, 0x00, sizeof(string));
	}

	return (*this);
}

// eg:
// string s = string("hello");
// string s1 = string("world");
// s = s1; <--- here
string& string::operator=(const string & value)
{
	auto right = &(value);
	if (right != this)
	{
		// destroy this
		this->~string();

		// set this to be equal to right
		this->mBuffer = calloc(value.mLength);
		this->mLength = value.mLength;

		memcpy(value.mBuffer, this->mBuffer, value.mLength);
	}

	return (*this);
}

string& string::operator=(const char* value)
{
	if (value != this->mBuffer)
	{
		// destroy this
		this->~string();

		// set this to be equal to right
		this->mLength = cstr::Length(value) + 1;
		this->mBuffer = calloc(this->mLength);

		memcpy(value, this->mBuffer, this->mLength);
	}

	return (*this);
}

string::~string()
{
	if (mBuffer) free(mBuffer);
	mBuffer = nullptr;
	mLength = 0x00;
}
