#include <Globals.h>

PrimitiveConsole gConsole = PrimitiveConsole();
IDTR GlobalIDTR;
byte GlobalIDTROffset[PAGE_SIZE];

attribute((aligned(PAGE_SIZE))) GDT GlobalGDT = 
{
	{0, 0, 0, 0x00, 0x00, 0}, // Kernel Null
	{0, 0, 0, 0x9A, 0xA0, 0}, // Kernel Code
	{0, 0, 0, 0x92, 0xA0, 0}, // Kernel Data

	{0, 0, 0, 0x00, 0x00, 0}, // User Null
	{0, 0, 0, 0x9A, 0xA0, 0}, // User Code
	{0, 0, 0, 0x92, 0xA0, 0}, // User Data
};
