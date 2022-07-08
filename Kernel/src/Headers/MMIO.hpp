#pragma once
#ifndef H_MMIO
#define H_MMIO
#include <Types.h>

namespace MMIO
{

	struct Register16
	{
		forceinline Register16() {}
		forceinline Register16(const uint16& value) { this->mValue = value; }

		forceinline operator= (const uint16& value) { this->mValue = value; }
		forceinline operator= (const Register16& value) { this->mValue = value.mValue; }

		forceinline operator uint16() { return this->mValue; }

		forceinline uint16 operator | (const uint16& value) { return this->mValue | value; }
		forceinline uint16 operator & (const uint16& value) { return this->mValue & value; }
		forceinline uint16 operator ^ (const uint16& value) { return this->mValue ^ value; }
		forceinline uint16 operator << (const uint16& value) { return this->mValue << value; }
		forceinline uint16 operator >> (const uint16& value) { return this->mValue << value; }
		forceinline uint16 operator + (const uint16& value) { return this->mValue + value; }
		forceinline uint16 operator - (const uint16& value) { return this->mValue - value; }
		forceinline uint16 operator * (const uint16& value) { return this->mValue * value; }
		forceinline uint16 operator / (const uint16& value) { return this->mValue / value; }

		forceinline uint16 operator |= (const uint16& value) { fast volatile uint16 val = this->mValue; val |= value; this->mValue = val;  return val; }
		forceinline uint16 operator &= (const uint16& value) { fast volatile uint16 val = this->mValue; val &= value; this->mValue = val;  return val; }
		forceinline uint16 operator ^= (const uint16& value) { fast volatile uint16 val = this->mValue; val ^= value; this->mValue = val;  return val; }
		forceinline uint16 operator <<= (const uint16& value) { fast volatile uint16 val = this->mValue; val <<= value; this->mValue = val; return val; }
		forceinline uint16 operator >>= (const uint16& value) { fast volatile uint16 val = this->mValue; val >>= value; this->mValue = val; return val; }
		forceinline uint16 operator += (const uint16& value) { fast volatile uint16 val = this->mValue; val += value; this->mValue = val;  return val; }
		forceinline uint16 operator -= (const uint16& value) { fast volatile uint16 val = this->mValue; val -= value; this->mValue = val;  return val; }
		forceinline uint16 operator *= (const uint16& value) { fast volatile uint16 val = this->mValue; val *= value; this->mValue = val;  return val; }
		forceinline uint16 operator /= (const uint16& value) { fast volatile uint16 val = this->mValue; val /= value; this->mValue = val;  return val; }

		forceinline bool operator == (const uint16& value) { return this->mValue == value; }
		forceinline bool operator != (const uint16& value) { return this->mValue != value; }

	private:
		volatile uint16 mValue;
	};

	struct Register32
	{
		forceinline Register32() {  }
		forceinline Register32(const uint32& value) { this->mValue = value; }

		forceinline operator= (const uint32& value) { this->mValue = value; }
		forceinline operator= (const Register32& value) { this->mValue = value.mValue; }

		forceinline operator uint32() { return this->mValue; }

		forceinline uint32 operator | (const uint32& value)			{ return this->mValue | value; }
		forceinline uint32 operator & (const uint32& value)			{ return this->mValue & value; }
		forceinline uint32 operator ^ (const uint32& value)			{ return this->mValue ^ value; }
		forceinline uint32 operator << (const uint32& value)		{ return this->mValue << value; }
		forceinline uint32 operator >> (const uint32& value)		{ return this->mValue << value; }
		forceinline uint32 operator + (const uint32& value)			{ return this->mValue + value; }
		forceinline uint32 operator - (const uint32& value)			{ return this->mValue - value; }
		forceinline uint32 operator * (const uint32& value)			{ return this->mValue * value; }
		forceinline uint32 operator / (const uint32& value)			{ return this->mValue / value; }

		forceinline uint32 operator |= (const uint32& value)		{ volatile uint32 val = this->mValue; val |= value; this->mValue = val;  return val;	}
		forceinline uint32 operator &= (const uint32& value)		{ volatile uint32 val = this->mValue; val &= value; this->mValue = val;  return val;	}
		forceinline uint32 operator ^= (const uint32& value)		{ volatile uint32 val = this->mValue; val ^= value; this->mValue = val;  return val;	}
		forceinline uint32 operator <<= (const uint32& value)		{ volatile uint32 val = this->mValue; val <<= value; this->mValue = val; return val;	}
		forceinline uint32 operator >>= (const uint32& value)		{ volatile uint32 val = this->mValue; val >>= value; this->mValue = val; return val;	}
		forceinline uint32 operator += (const uint32& value)		{ volatile uint32 val = this->mValue; val += value; this->mValue = val;  return val;	}
		forceinline uint32 operator -= (const uint32& value)		{ volatile uint32 val = this->mValue; val -= value; this->mValue = val;  return val;	}
		forceinline uint32 operator *= (const uint32& value)		{ volatile uint32 val = this->mValue; val *= value; this->mValue = val;  return val;	}
		forceinline uint32 operator /= (const uint32& value)		{ volatile uint32 val = this->mValue; val /= value; this->mValue = val;  return val;	}

		forceinline bool operator == (const uint32& value) { return this->mValue == value; }
		forceinline bool operator != (const uint32& value) { return this->mValue != value; }

	private:
		volatile uint32 mValue;
	};

}

#endif // !H_MMIO
