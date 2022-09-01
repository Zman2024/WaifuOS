#pragma once
#ifndef H_List
#define H_List
#include <Types.h>
#include <Memory.h>

//i need to fix this it's buggy

template <typename T>
class List
{
public:

	inline List<T>()
	{
		mArray = new T[5];
		mArraySize = 5;
		mCount = 0;
	}

	inline List<T>(const List<T>& value)
	{
		this->mArray = value.mArray;
		this->mArraySize = value.mArraySize;
		this->mCount = value.mCount;
		
		auto rwvalue = (List<T>*)&value;

		rwvalue->mArray = nullptr;
		rwvalue->mArraySize = 0;
		rwvalue->mCount = 0;
	}

	inline List<T>& operator=(const List<T>&& value)
	{
		auto right = (void*)&(value);
		if (right != this)
		{
			// destroy this
			this->~List();

			// copy rvalue to this
			this->mArray = value.mArray;
			this->mArraySize = value.mArraySize;
			this->mCount = value.mCount;

			// stop destruction of rvalue
			memset64(right, 0x00, sizeof(List<T>));
		}

		return (*this);
	}

	inline List<T>& operator=(const List<T>& value)
	{
		auto right = &(value);
		if (right != this)
		{
			// destroy this
			this->~List();

			// set this to be equal to right
			this->mArray = new T[value.mArraySize];
			this->mArraySize = value.mArraySize;
			this->mCount = value.mCount;

			memcpy(this->mArray, value.mArray, value.mCount * sizeof(T));
		}

		return (*this);
	}

	inline List<T>(nint initialCount)
	{
		mArray = new T[initialCount];
		mCount = initialCount;
		mArraySize = initialCount;
	}

	forceinline nint GetCount()
	{
		return mCount;
	}

	inline void Add(const T& item)
	{
		if (mCount + 1 > mArraySize)
		{
			T* newArray = new T[mArraySize + 5];
			for (nint x = 0; x < mCount; x++) // kinda memcpy but oh well
			{
				newArray[x] = mArray[x];
			}
			delete mArray;
			mArray = newArray;
			mArraySize = mArraySize + 5;
		}

		mArray[mCount++] = item;
	}

	inline void Remove(const T& item)
	{
		for (nint x = 0; x < mCount; x++)
		{
			if (item == mArray[x])
			{
				RemoveAt(x);
				break;
			}
		}
	}

	inline void RemoveAt(nint index)
	{
		if (index + 1 > mCount) return;
		if (index == 0 && mCount == 1)
		{
			memset64(mArray, 0x00, sizeof(T));
			mCount = 0;
			return;
		}

		for (nint x = index + 1; x < mCount; x++)
		{
			mArray[x - 1] = mArray[x];
		}

		mCount--;
	}

	inline T& Get(nint index)
	{
		if (index + 1 > mCount) // #PF if bruh
		{
			// cause nullptr pagefult
			error("INDEX WAS OUTSIDE BOUNDS OF ARRAY! index: %0, mCount: %1", index, mCount);
			*(byte*)nullptr;
		}
		return mArray[index];
	}

	inline T& operator[] (nint index)
	{
		return Get(index);
	}

	inline ~List()
	{
		if (!mArray || !this) return;
		delete mArray;
	}

protected:
	T* mArray = nullptr;
	nint mArraySize = 0;
	nint mCount = 0;

};

#endif // !H_List