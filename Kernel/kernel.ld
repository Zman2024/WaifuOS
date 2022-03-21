OUTPUT_FORMAT(elf64-x86-64)
ENTRY(KernelStart)

SECTIONS
{
	_KernelStart = .;
	.text : ALIGN(0x1000)
	{
		*(.text)
	}

	_DataStart = .;
	.data : ALIGN(0x1000)
	{
		*(.data)
	}
	_DataEnd = .;

	_RODataStart = .;
	.rodata : ALIGN(0x1000)
	{
		*(.rodata)
	}
	_RODataEnd = .;

	_BssDataStart = .;
	.bss : ALIGN(0x1000)
	{
		*(COMMON)
		*(.bss)
	}
	_BssDataEnd = .;

	_KernelEnd = .;
}