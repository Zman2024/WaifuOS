#include <Globals.h>

PrimitiveConsole gConsole = PrimitiveConsole();
IDTR GlobalIDTR;
byte GlobalIDTROffset[PAGE_SIZE];

attribute((aligned(0x10))) GDT GlobalGDT = 
{
	{0, 0, 0, 0x00, 0x00, 0}, // Kernel Null
	{0, 0, 0, 0b10011010, 0b10100000, 0}, // Kernel Code
	{0, 0, 0, 0b10010010, 0b10100000, 0}, // Kernel Data

	{0, 0, 0, 0x9A, 0xA0, 0}, // User Code
	{0, 0, 0, 0x92, 0xA0, 0}, // User Data
	{0, 0, 0, 0x00, 0x00, 0, 0, 0} // TSS
};
