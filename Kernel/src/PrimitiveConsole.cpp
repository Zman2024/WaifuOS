#include <PrimitiveConsole.h>
#include <Memory.h>

PrimitiveConsole::PrimitiveConsole() {  }

PrimitiveConsole::PrimitiveConsole(FrameBuffer * fb, Font * font)
{
	if ((!fb) | (!font)) return;
	mFrameBuffer = fb;
	mFont = font;
	this->mWidth = fb->PixelsPerScanline / (font->Header->charsize>>1);
	this->mHeight = (fb->Height / font->Header->charsize);

	HasDrawnCursor = false;
	CursorEnabled = false;

	mCursorPosition = { 0, 0 };
	mPreviousRenderedPosition = { 0, 0 };
	
	SetForegroundColor(Color::White);
	SetBackgroundColor(Color::VeryDarkGray);
}

void PrimitiveConsole::WriteChar(char chr)
{
	WriteChar(chr, mForegroundColor);
}

inline void PrimitiveConsole::WriteChar(char chr, Color32 color)
{
	switch (chr)
	{
		case '\n':
			NewLine();
			return;

		case '\b':
			Backspace(1, false);
			return;

		case '\0':
			return;

		case '\t':
			Write("    ", color);
			return;

	}

	sPoint originalPos = mCursorPosition;

	// Increase cursor position by 1
	byte chrSz = mFont->Header->charsize;
	byte chrSz2 = mFont->Header->charsize >> 1;
	bool shouldScroll = false;

	mCursorPosition.X += 1;
	if (mCursorPosition.X >= mWidth)
	{
		mCursorPosition.X = 0;
		mCursorPosition.Y += 1;

		if (mCursorPosition.Y > this->mHeight - 1)
		{
			mCursorPosition.Y = this->mHeight - 1;
			shouldScroll = true;
		}
	}

	UpdateCursor();
	
	PutChar(chr, color, mBackgroundColor, originalPos.X, originalPos.Y);
	if (shouldScroll) ScrollDown();
}

void PrimitiveConsole::PutChar(char chr, Color32 fColor, Color32 bColor, uint xOff, uint yOff)
{
	char* fontPtr = (char*)mFont->GlyphBuffer + (chr * mFont->Header->charsize);
	byte chrSz = mFont->Header->charsize;
	byte chrSz2 = mFont->Header->charsize >> 1;

	sPoint unscaled = ConvertScaledToUnscaled(xOff, yOff);

	for (uint64 y = unscaled.Y; y < unscaled.Y + chrSz; y++)
	{
		for (uint64 x = unscaled.X; x < unscaled.X + chrSz2; x++)
		{
			WritePixel(x, y, bColor);
			if (((*fontPtr) & (0b10000000 >> (x - unscaled.X))) > 0)
			{
				WritePixel(x, y, fColor);
			}
		}
		fontPtr++;
	}

}

void PrimitiveConsole::Write(const char * str)
{
	Write(str, mForegroundColor);
}

inline void PrimitiveConsole::Write(const char * str, Color32 color)
{
	for (uint64 index = 0; str[index]; index++)
		WriteChar(str[index], color);
}

void PrimitiveConsole::WriteLine(const char * str)
{
	WriteLine(str, mForegroundColor);
}

inline void PrimitiveConsole::WriteLine(const char * str, Color32 color)
{
	Write(str, color);
	NewLine();
}

void PrimitiveConsole::WriteLine()
{
	NewLine();
}

void PrimitiveConsole::NewLine(uint lines)
{
	mCursorPosition.X = 0;
	mCursorPosition.Y += lines;
	
	bool wasEnabled = CursorEnabled;
	if (CursorEnabled) DisableCursor();

	if (mCursorPosition.Y > this->mHeight - 1)
	{
		ScrollDown(mCursorPosition.Y - (this->mHeight - 1));
		mCursorPosition.Y = this->mHeight - 1;
	}

	if (wasEnabled) EnableCursor();
}

void PrimitiveConsole::ScrollDown()
{
	fast uint64 fbBase = (u64)mFrameBuffer->BaseAddress;
	fast uint64 height = mFrameBuffer->Height;
	fast uint64 bytesPerScanline = u64(mFrameBuffer->PixelsPerScanline) << 2;

	byte charsize = mFont->Header->charsize;

	for (fast u64 vScanline = 0; vScanline < height - charsize; vScanline++)
	{
		fast uint64 vScanBase = fbBase + (bytesPerScanline * vScanline);
		fast uint64 vScanBaseAhead = fbBase + (bytesPerScanline * (vScanline + charsize));

		memcpy((vptr)vScanBase, vScanBaseAhead, bytesPerScanline);
	}

	fast uint64 speedup = u64(mBackgroundColor) | (u64(mBackgroundColor) << 32);

	for (fast u64 vScanline = height - charsize; vScanline < height; vScanline++)
	{
		fast uint64 vScanBase = fbBase + (bytesPerScanline * vScanline);
		memset64((vptr)vScanBase, speedup, bytesPerScanline);
	}

}

void PrimitiveConsole::ScrollDown(uint lines)
{
	for (uint x = 0; x < lines; x++)
	{
		ScrollDown();
	}
}

void PrimitiveConsole::Backspace(int nBackspace, bool RemoveChar)
{
	byte mFontSz = mFont->Header->charsize;
	byte mFontSz2 = mFontSz >> 1;

	for (int x = 0; x < nBackspace; x++)
	{
		if (mCursorPosition.X <= 0)
		{
			mCursorPosition.X = mWidth;
			mCursorPosition.Y -= 1;
			if (mCursorPosition.Y < 0)
			{
				mCursorPosition.Y = 0;
				mCursorPosition.X = 1;
			}
		}


		mCursorPosition.X -= 1;
		if (mCursorPosition.X < 0)
		{
			mCursorPosition.X = mWidth;
			mCursorPosition.Y -= 1;
			if (mCursorPosition.Y < 0)
			{
				mCursorPosition.Y = 0;
				mCursorPosition.X = 0;
			}
		}

		if (RemoveChar)
		{
			PutChar(' ', mForegroundColor, mBackgroundColor, mCursorPosition.X, mCursorPosition.Y);
		}

	}
	UpdateCursor();
}

void PrimitiveConsole::Clear(Color32 color)
{
	fast uint64 fbBase = (u64)mFrameBuffer->BaseAddress;
	fast uint64 height = mFrameBuffer->Height;
	fast uint64 bytesPerScanline = u64(mFrameBuffer->PixelsPerScanline) << 2;
	
	fast uint64 speedup = u64(color) | (u64(color) << 32);

	for (fast u64 vScanline = 0; vScanline < height; vScanline++)
	{
		fast uint64 vScanBase = fbBase + (bytesPerScanline * vScanline);
		memset64((vptr)vScanBase, speedup, bytesPerScanline);
	}

	mCursorPosition = sPoint(0, 0);
}

#pragma region Getters & Setters

// This sets them scaled, so char X, line Y rather then pixel X, pixel Y
void PrimitiveConsole::SetCursorPosition(int64 X, int64 Y)
{
	mCursorPosition.X = X;
	mCursorPosition.Y = Y;
}

sPoint PrimitiveConsole::GetCursorPosition()
{
	return mCursorPosition;
}

void PrimitiveConsole::SetBackgroundColor(Color32 color)
{
	mBackgroundColor = color;
}

void PrimitiveConsole::SetForegroundColor(Color32 color)
{
	mForegroundColor = color;
}

Color32 PrimitiveConsole::GetForegroundColor()
{
	return mForegroundColor;
}

Color32 PrimitiveConsole::GetBackgroundColor()
{
	return mBackgroundColor;
}

void PrimitiveConsole::SetFont(Font * font)
{
	mFont = font;
}

Font PrimitiveConsole::GetFont()
{
	return *mFont;
}

#pragma endregion

void PrimitiveConsole::UpdateCursor()
{
	if (!CursorEnabled) return;

	sPoint pos = ConvertScaledToUnscaled(mCursorPosition.X, mCursorPosition.Y);

	byte yCharSize = mFont->Header->charsize;
	byte xCharSize = yCharSize >> 1;

	if (HasDrawnCursor)
	{
		sPoint oldPos = ConvertScaledToUnscaled(mPreviousRenderedPosition.X, mPreviousRenderedPosition.Y);
		for (uint64 y = 0; y < yCharSize; y++)
		{
			for (uint64 x = 0; x < xCharSize; x++)
			{
				byte bite = ((y * 8) + x) / 8;
				if (mCursorBuffer[bite] & (0b10000000 >> (x % 8)))	// Only replace the pixels we overwrote
				{
					WritePixel(oldPos.X + x, oldPos.Y + y, mPreviousCursorBuffer[x + (y * 8)]);
				}
			}
		}
	}

	mPreviousRenderedPosition = mCursorPosition;

	// Hardcoded numbers = cringe, but my cursor is hardcoded so whatever
	for (uint64 y = 0; y < 16; y++)
	{
		for (uint64 x = 0; x < 8; x++)
		{
			// Save the old pixels
			mPreviousCursorBuffer[x + (y * 8)] = ReadPixel(pos.X + x, pos.Y + y);
			byte bite = ((y * 8) + x) / 8;
			if (mCursorBuffer[bite] & (0b10000000 >> (x % 8)))
			{
				WritePixel(pos.X + x, pos.Y + y, mForegroundColor);
			}
		}
	}

	HasDrawnCursor = true;

}

void PrimitiveConsole::EnableCursor()
{
	CursorEnabled = true;
	HasDrawnCursor = false;
	UpdateCursor();
}

void PrimitiveConsole::DisableCursor()
{
	sPoint pos = ConvertScaledToUnscaled(mCursorPosition.X, mCursorPosition.Y);

	byte yCharSize = mFont->Header->charsize;
	byte xCharSize = yCharSize >> 1;

	if (HasDrawnCursor)
	{
		sPoint oldPos = ConvertScaledToUnscaled(mPreviousRenderedPosition.X, mPreviousRenderedPosition.Y);
		for (uint64 y = 0; y < yCharSize; y++)
		{
			for (uint64 x = 0; x < xCharSize; x++)
			{
				byte bite = ((y * 8) + x) / 8;
				if (mCursorBuffer[bite] & (0b10000000 >> (x % 8)))	// Only replace the pixels we overwrote
				{
					WritePixel(oldPos.X + x, oldPos.Y + y, mPreviousCursorBuffer[x + (y * 8)]);
				}
			}
		}
	}

	CursorEnabled = false;
	HasDrawnCursor = false;
}

void PrimitiveConsole::MoveCursor(Direction dir)
{
	switch (dir)
	{
		case Direction::Up:
			if (mCursorPosition.Y > 0) mCursorPosition.Y--;
			else
			{
				mCursorPosition.Y = 0;
				mCursorPosition.X = 0;
			}
			break;

		case Direction::Down:
			if (mCursorPosition.Y < this->mHeight - 1) mCursorPosition.Y++;
			else mCursorPosition.Y = this->mHeight - 1;
			break;

		case Direction::Left:
			if (mCursorPosition.X > 0) mCursorPosition.X--;
			else if(mCursorPosition.Y > 0)
			{
				mCursorPosition.Y--;
				mCursorPosition.X = mWidth - 1;
			}
			else
			{
				mCursorPosition.X = 0;
				mCursorPosition.Y = 0;
			}
			break;

		case Direction::Right:
			if (mCursorPosition.X < mWidth-1) mCursorPosition.X++;
			else if (mCursorPosition.Y < this->mHeight - 1)
			{
				mCursorPosition.Y++;
				mCursorPosition.X = 0;
			}
			else
			{
				mCursorPosition.X = mWidth - 1;
				mCursorPosition.Y = this->mHeight - 1;
			}
			break;
	}
}

sPoint PrimitiveConsole::ConvertScaledToUnscaled(int64 X, int64 Y)
{
	return sPoint(X * (mFont->Header->charsize >> 1), Y * mFont->Header->charsize);
}

sPoint PrimitiveConsole::ConvertUncaledToScaled(int64 X, int64 Y)
{
	return sPoint(X / (mFont->Header->charsize >> 1), Y / mFont->Header->charsize);
}

void PrimitiveConsole::WritePixel(int64 posX, int64 posY, Color32 color)
{
	if ((posX >= mFrameBuffer->PixelsPerScanline) | (posX < 0)) return;
	if ((posY >= mFrameBuffer->Height) | (posY < 0)) return;

	Color32* buff = (Color32*)mFrameBuffer->BaseAddress + posX + (posY * mFrameBuffer->PixelsPerScanline);
	*buff = color;
}

Color32 PrimitiveConsole::ReadPixel(int64 posX, int64 posY)
{
	if ((posX >= mFrameBuffer->PixelsPerScanline) | (posX < 0)) return Color::Black;
	if ((posY >= mFrameBuffer->Height) | (posY < 0)) return Color::Black;
	Color32* ptr = (Color32*)mFrameBuffer->BaseAddress;
	return ptr [posX + (posY * mFrameBuffer->PixelsPerScanline)];

}
