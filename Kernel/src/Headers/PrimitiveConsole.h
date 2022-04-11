#pragma once
#ifndef H_PrimitiveConsole
#define H_PrimitiveConsole
#include <Types.h>
#include <Structs.h>
#include <Colors.h>
#include <Direction.h>

class PrimitiveConsole
{
public:
	PrimitiveConsole();
	PrimitiveConsole(FrameBuffer* fb, Font* font);

	void PutChar(char chr, Color32 fColor, Color32 bColor, uint xOff, uint yOff);

	void WriteChar(char chr);
	void WriteChar(char chr, Color32 color);

	void Write(const char* str);
	void Write(const char* str, Color32 color);
	void WriteLine(const char* str);
	void WriteLine(const char* str, Color32 color);
	void WriteLine();

	void NewLine(uint lines = 1);
	void ScrollDown();
	void ScrollDown(uint lines);
	void Backspace(int nBackspace = 1, bool RemoveChar = true);

	void Clear(Color32 color);
	forceinline void Clear() { Clear(mBackgroundColor); }

	void SetCursorPosition(int64 X, int64 Y);
	sPoint GetCursorPosition();

	void SetBackgroundColor(Color32 color);
	void SetForegroundColor(Color32 color);

	Color32 GetForegroundColor();
	Color32 GetBackgroundColor();

	void SetFont(Font* font);
	Font GetFont();

	void UpdateCursor();
	void MoveCursor(Direction dir);

	void EnableCursor();
	void DisableCursor();

	forceinline bool GetCursorEnabled() { return CursorEnabled; }

protected:
	void WritePixel(int64 posX, int64 posY, Color32 color);
	Color32 ReadPixel(int64 posX, int64 posY);

	sPoint ConvertScaledToUnscaled(int64 X, int64 Y);
	sPoint ConvertUncaledToScaled(int64 X, int64 Y);

	FrameBuffer* mFrameBuffer;
	Font* mFont;
	int64 mWidth;
	int64 mHeight;
	sPoint mCursorPosition = { 0, 0 };
	sPoint mPreviousRenderedPosition = { 0, 0 };
	bool HasDrawnCursor = false;
	bool CursorEnabled = false;
	Color32 mForegroundColor = Color::White;
	Color32 mBackgroundColor = Color::DarkGray;

	byte mCursorBuffer[16] =
	{
		0b00000000, //0b00000000,
		0b00000000, //0b00000000,
		0b00000000, //0b00000000,
		0b00000000, //0b00000000,
		0b00000000, //0b00000000,
		0b00000000, //0b00000000,
		0b00000000, //0b00000000,
		0b00000000, //0b00000000,
		0b00000000, //0b00000000,
		0b00000000, //0b00000000,
		0b00000000, //0b00000000,
		0b00000000, //0b00000000,
		0b00000000, //0b00000000,
		0b11111111, //0b11111111,
		0b11111111, //0b11111111,
		0b11111111, //0b11111111,
	};

	Color32 mPreviousCursorBuffer[128];

};
#endif