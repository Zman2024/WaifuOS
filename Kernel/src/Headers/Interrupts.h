#pragma once
#ifndef H_Interrupts
#define H_Interrupts
#include <Types.h>
#include <Globals.h>

namespace Interrupts
{
	constexpr byte PIC_MASTER_OFFSET = 0x20;
	constexpr byte PIC_SLAVE_OFFSET = PIC_MASTER_OFFSET + 8;

	struct InterruptFrame;

	/* From: https://wiki.osdev.org/Interrupt_Vector_Table#CPU_Interrupt_Layout
	| INT #     | Description
	+-----------+-----------------------------------
	| 0x00      | Divide by 0
	| 0x01      | Reserved
	| 0x02      | NMI Interrupt
	| 0x03      | Breakpoint (INT3)
	| 0x04      | Overflow (INTO)
	| 0x05      | Bounds range exceeded (BOUND)
	| 0x06      | Invalid opcode (UD2)
	| 0x07      | Device not available (WAIT/FWAIT)
	| 0x08      | Double fault
	| 0x09      | Coprocessor segment overrun
	| 0x0A      | Invalid TSS
	| 0x0B      | Segment not present
	| 0x0C      | Stack-segment fault
	| 0x0D      | General protection fault
	| 0x0E      | Page fault
	| 0x0F      | Reserved
	| 0x10      | x87 FPU error
	| 0x11      | Alignment check
	| 0x12      | Machine check
	| 0x13      | SIMD Floating-Point Exception
	| 0x14-0x1F | Reserved
	| 0x20-0xFF | User definable (We will put the IRQs here)

	From: https://wiki.osdev.org/Interrupt_Vector_Table#Default_Hardware_Interrupt_Layout
	Master:
	| IRQ # | Description
	--------+------------------------------
	| 0     | PIT
	| 1     | Keyboard
	| 2     | 8259A slave controller
	| 3     | COM2 / COM4
	| 4     | COM1 / COM3
	| 5     | LPT2
	| 6     | Floppy controller
	| 7     | LPT1

	Slave:
	--------+------------------------------
	| 8     | RTC
	| 9     | Unassigned
	| 10    | Unassigned
	| 11    | Unassigned
	| 12    | Mouse controller
	| 13    | Math coprocessor
	| 14    | Hard disk controller 1
	| 15    | Hard disk controller 2
	*/
	struct Interrupt
	{
		enum : byte
		{
			/* CPU Exception Table */
			DivideByZero = 0x00,
			SingleStep = 0x01,
			NonMaskable = 0x02,
			Breakpoint = 0x03,
			OverflowTrap = 0x04,
			BoundRangeExceeded = 0x05,
			InvalideOpcode = 0x06,
			CoprocessorNA = 0x07,
			DoubleFault = 0x08,
			CoprocessorSegmentOverrun = 0x09,
			InvalidStateSegment = 0x0A,
			SegmentMissing = 0x0B,
			StackException = 0x0C,
			GeneralProtection = 0x0D,
			PageFault = 0x0E,
			Reserved = 0x0F,
			CoprocessorError = 0x10,	//x87 FPU error
			AlignmentCheck = 0x11,
			MachineCheck = 0x12,
			SIMDException = 0x13,

			// Master
			PIT = PIC_MASTER_OFFSET, HPET0 = PIC_MASTER_OFFSET,
			Keyboard = PIC_MASTER_OFFSET + 1,
			SlaveEnabled = PIC_MASTER_OFFSET + 2,
			COM2 = PIC_MASTER_OFFSET + 3,
			COM1 = PIC_MASTER_OFFSET + 4,
			DataLPT2 = PIC_MASTER_OFFSET + 5,
			FloppyDisk = PIC_MASTER_OFFSET + 6,
			DataLPT1 = PIC_MASTER_OFFSET + 7,

			// Slave
			RTC = PIC_SLAVE_OFFSET, HPET1 = PIC_SLAVE_OFFSET,
			Redirect0 = PIC_SLAVE_OFFSET + 1,
			Reserved1 = PIC_SLAVE_OFFSET + 2,
			Reserved2 = PIC_SLAVE_OFFSET + 3,
			Mouse = PIC_SLAVE_OFFSET + 4,
			NumericCoprocessorError = PIC_SLAVE_OFFSET + 5,
			FixedDiskController1 = PIC_SLAVE_OFFSET + 6,
			FixedDiskController2 = PIC_SLAVE_OFFSET + 7,

		};
	};


	void RegisterInterrupt(void* handlerAddress, byte interrupt, IdtType interruptType = IdtType::InterruptGate);

	void LoadIDT();

	attribute((interrupt)) void hDivideByZeroFault(struct InterruptFrame* frame);
	attribute((interrupt)) void hDoubleFault(struct InterruptFrame* frame);
	attribute((interrupt)) void hGeneralProtectionFault(struct InterruptFrame* frame);
	attribute((interrupt)) void hPageFault(struct InterruptFrame* frame);
	attribute((interrupt)) void hCoprocessorFault(struct InterruptFrame* frame);
}

#endif