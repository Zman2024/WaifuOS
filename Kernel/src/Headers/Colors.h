#pragma once
#include <Types.h>
#include <Structs.h>
#ifndef H_Colors
#define H_Colors

enum struct Color : uint32
{
	White = 0xFFFFFFFF,
	Black = 0xFF000000,

	Red = 0xFFFF0000,
	Green = 0xFF00FF00,
	Blue = 0xFF0000FF,

	Cyan = 0xFF00FFFF,
	Yellow = 0xFFFFFF00,
	Purple = 0xFFFF00FF,

	DarkGray = 0xFF101010,
	DarkRed = 0xFF900000,
};
#endif