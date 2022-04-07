#include <string.h>
#include <cstr.h>
#include <Memory.h>
#include <Globals.h>

string::string()
{
	mBuffer = nullptr;
	mBufferSize = 0x00;
}

string::string(uint64 length)
{
	length++;
	mBuffer = (char*)calloc(length);
	mBufferSize = length;
}

string::string(const string& value)
{
	auto right = &(value);
	if (right != this)
	{
		// set this to be equal to right
		this->mBuffer = calloc(value.mBufferSize);
		this->mBufferSize = value.mBufferSize;

		memcpy(this->mBuffer, value.mBuffer, value.mBufferSize);
	}
}

string::string(const char* str)
{
	mBufferSize = strlen(str) + 1;
	mBuffer = (char*)calloc(mBufferSize);
	memcpy(mBuffer, str, mBufferSize);
}

string::string(const char* str, nint length)
{
	mBufferSize = length + 1;
	mBuffer = (char*)calloc(mBufferSize);
	memcpy(mBuffer, str, mBufferSize);
	mBuffer[length] = 0x00;
}

string::~string()
{
	if (mBuffer) free(mBuffer);
	mBuffer = nullptr;
	mBufferSize = 0x00;
}

string string::append(const string&& rvalue)
{
	nint newBufferSize = this->GetLength() + rvalue.mBufferSize;

	// allocate new character buffer
	auto newbuffer = new char[newBufferSize];

	// copy all text (without nul char)
	memcpy(newbuffer, this->mBuffer, this->GetLength());

	// copy all text from rvalue's buffer to our newbuffer + this->Length
	memcpy(newbuffer + this->GetLength(), rvalue.mBuffer, rvalue.mBufferSize);

	// destroy rvalue string cause otherwise memleaks
	rvalue.~string();

	auto str = string();
	str.mBuffer = newbuffer;
	str.mBufferSize = newBufferSize;
	return str;
}

string string::append(const string& rvalue)
{
	nint newBufferSize = this->GetLength() + rvalue.mBufferSize;

	// allocate new character buffer
	auto newbuffer = new char[newBufferSize];

	// copy all text (without nul char)
	memcpy(newbuffer, this->mBuffer, this->GetLength());

	// copy all text from rvalue's buffer to our newbuffer + this->Length
	memcpy(newbuffer + this->GetLength(), rvalue.mBuffer, rvalue.mBufferSize);

	auto str = string();
	str.mBuffer = newbuffer;
	str.mBufferSize = newBufferSize;
	return str;
}

string string::append(const char* rvalue)
{
	auto rvalLength = strlen(rvalue);

	nint newBufferSize = this->mBufferSize + rvalLength;

	// allocate new character buffer
	auto newbuffer = new char[newBufferSize];

	// copy all text (without nul char)
	memcpy(newbuffer, this->mBuffer, this->GetLength());

	// copy all text from rvalue's buffer to our newbuffer + this->Length
	memcpy(newbuffer + this->GetLength(), rvalue, rvalLength + 1);

	auto str = string();
	str.mBuffer = newbuffer;
	str.mBufferSize = newBufferSize;

	return str;
}

string string::prepend(const char* lvalue)
{
	auto lvalLength = strlen(lvalue);

	nint newBufferSize = nint(this->mBufferSize + lvalLength);

	// allocate new character buffer
	auto newbuffer = new char[newBufferSize];

	// copy all text (without nul char)
	memcpy(newbuffer, lvalue, lvalLength);

	// copy all text from rvalue's buffer to our newbuffer + this->Length
	memcpy(newbuffer + lvalLength, this->mBuffer, this->mBufferSize);

	auto str = string();
	str.mBuffer = newbuffer;
	str.mBufferSize = newBufferSize;

	return str;
}

// eg:
// string s = string("hello");
// s = string("world"); <--- here
string& string::operator=(const string&& value)
{
	auto right = (void*)&(value);
	if (right != this)
	{
		// destroy this
		this->~string();

		// copy rvalue to this
		memcpy((vptr)this, right, sizeof(string));

		// stop destruction of rvalue
		memset64(right, 0x00, sizeof(string));
	}

	return (*this);
}

// eg:
// string s = string("hello");
// string s1 = string("world");
// s = s1; <--- here
string& string::operator=(const string& value)
{
	auto right = &(value);
	if (right != this)
	{
		// destroy this
		this->~string();

		// set this to be equal to right
		this->mBuffer = calloc(value.mBufferSize);
		this->mBufferSize = value.mBufferSize;

		memcpy(this->mBuffer, value.mBuffer, value.mBufferSize);
	}

	return (*this);
}

// eg:
// string myString;
// myString = "Hello World!";
string& string::operator=(const char* value)
{
	if (value != this->mBuffer)
	{
		// destroy this
		this->~string();

		// set this to be equal to right
		this->mBufferSize = strlen(value) + 1;
		this->mBuffer = calloc(this->mBufferSize);

		memcpy(this->mBuffer, value, this->mBufferSize);
	}

	return (*this);
}
