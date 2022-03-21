#include <PIC.hpp>

namespace PIC
{
	using namespace IO;

	constexpr byte Icw1Init = 0x10;
	constexpr byte Icw1Icw4 = 0x01;
	constexpr byte Icw18086 = 0x01;

	void Remap()
	{
		// save old mask
		byte masterBitmask = inb(MasterData);
		wait();
		byte slaveBitmask = inb(SlaveData);
		wait();

		outb(MasterCommand, Icw1Init | Icw1Icw4);
		wait();

		outb(SlaveCommand, Icw1Init | Icw1Icw4);
		wait();

		outb(MasterData, PIC_MASTER_OFFSET);
		wait();

		outb(SlaveData, PIC_SLAVE_OFFSET);
		wait();

		outb(MasterData, 0x4);
		wait();
		outb(SlaveData, 0x2);
		wait();

		outb(MasterData, Icw18086);
		wait();
		outb(SlaveData, Icw18086);
		wait();

		// restore old mask (like dream)
		outb(MasterData, masterBitmask);
		wait();
		outb(SlaveData, slaveBitmask);
		wait();

	}

	void Mask(uint16 mask, bool state)
	{
		byte master = mask & 0xFF;
		byte slave = (mask >> 0x8) & 0xFF;

		// save old mask
		byte masterBitmask = inb(MasterData);
		wait();
		byte slaveBitmask = inb(SlaveData);
		wait();

		// cant think of a better way to do this rn
		if (state)
		{
			masterBitmask &= ~master;
			slaveBitmask &= ~slave;
		}
		else
		{
			masterBitmask |= master;
			slaveBitmask |= slave;
		}

		outb(MasterData, masterBitmask);
		wait();
		outb(SlaveData, slaveBitmask);
		wait();

	}

}