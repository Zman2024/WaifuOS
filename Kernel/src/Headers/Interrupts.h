#pragma once
#ifndef H_Interrupts
#define H_Interrupts

#include <Types.h>
#include <Globals.h>
#include <cstr.h>
#include <IOBus.hpp>

namespace Interrupts
{
	constexpr byte SpuriousInterruptVector = 0xFF;
	constexpr byte IRQ_OFFSET = 0x20;

	struct InterruptFrame
	{
		nint rip = 0x00;
		nint cs = 0x00;
		nint rflags = 0x00;
		nint rsp = 0x00;
		nint ss = 0x00;
	};

	struct RegisterState
	{
		nint rax = 0;
		nint rbx = 0;
		nint rcx = 0;
		nint rdx = 0;

		nint rsi = 0;
		nint rdi = 0;

		nint rbp = 0;

		nint r8 = 0;
		nint r9 = 0;
		nint r10 = 0;
		nint r11 = 0;
		nint r12 = 0;
		nint r13 = 0;
		nint r14 = 0;
		nint r15 = 0;

		nint ds = 0;
		nint es = 0;
		nint fs = 0;
		nint gs = 0;

		fp64 mm0 = 0;
		fp64 mm1 = 0;
		fp64 mm2 = 0;
		fp64 mm3 = 0;
		fp64 mm4 = 0;
		fp64 mm5 = 0;
		fp64 mm6 = 0;
		fp64 mm7 = 0;

		fp64 ymm0[4] = { 0, 0, 0, 0 };
		fp64 ymm1[4] = { 0, 0, 0, 0 };
		fp64 ymm2[4] = { 0, 0, 0, 0 };
		fp64 ymm3[4] = { 0, 0, 0, 0 };
		fp64 ymm4[4] = { 0, 0, 0, 0 };
		fp64 ymm5[4] = { 0, 0, 0, 0 };
		fp64 ymm6[4] = { 0, 0, 0, 0 };
		fp64 ymm7[4] = { 0, 0, 0, 0 };
		fp64 ymm8[4] = { 0, 0, 0, 0 };
		fp64 ymm9[4] = { 0, 0, 0, 0 };
		fp64 ymm10[4] = { 0, 0, 0, 0 };
		fp64 ymm11[4] = { 0, 0, 0, 0 };
		fp64 ymm12[4] = { 0, 0, 0, 0 };
		fp64 ymm13[4] = { 0, 0, 0, 0 };
		fp64 ymm14[4] = { 0, 0, 0, 0 };
		fp64 ymm15[4] = { 0, 0, 0, 0 };

	} attribute((packed));

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
	namespace Interrupt
	{
		enum : byte
		{
			/* CPU Exception Table */
			DivideByZero = 0x00,
			Debug = 0x01,
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
			PIT = IRQ_OFFSET, HPET0 = IRQ_OFFSET,
			Keyboard = IRQ_OFFSET + 1,
			SlaveEnabled = IRQ_OFFSET + 2,
			COM2 = IRQ_OFFSET + 3,
			COM1 = IRQ_OFFSET + 4,
			DataLPT2 = IRQ_OFFSET + 5,
			FloppyDisk = IRQ_OFFSET + 6,
			DataLPT1 = IRQ_OFFSET + 7,

			// Slave
			RTC = IRQ_OFFSET + 8, HPET1 = IRQ_OFFSET + 8,
			Redirect0 = IRQ_OFFSET + 9,
			Reserved1 = IRQ_OFFSET + 0xA,
			Reserved2 = IRQ_OFFSET + 0xB,
			Mouse = IRQ_OFFSET + 0xC,
			NumericCoprocessorError = IRQ_OFFSET + 0xD,
			FixedDiskController1 = IRQ_OFFSET + 0xE,
			FixedDiskController2 = IRQ_OFFSET + 0xF,

			SpuriousInterrupt = 0xFF,

		};
	};

	forceinline void LoadGIDT() { asm("lidt %0" : : "m" (GlobalIDTR)); }

	void RegisterInterruptStubs();
	void RegisterInterrupt(vptr handlerAddress, u16 inter, IdtType interruptType = IdtType::InterruptGate);

	void hDivideByZeroFault(nint code, InterruptFrame* frame);
	void hDebug();
	void hNonMaskableInterrupt(nint code, InterruptFrame* frame);
	void hBreakpointFault(nint code, InterruptFrame* frame);
	void hOverflowTrap(nint code, InterruptFrame* frame);
	void hBoundRangeFault(nint code, InterruptFrame* frame);
	void hInvalidOpcodeFault(nint code, InterruptFrame* frame);
	void hCoprocessorNAFault(nint code, InterruptFrame* frame);
	void hDoubleFault(nint intr, InterruptFrame* frame, nint code);
	void hCoprocessorSegmentOverrunFault(nint code, InterruptFrame* frame);
	void hInvalidStateSegmentFault(nint intr, InterruptFrame* frame, nint code);
	void hSegmentMissingFault(nint intr, InterruptFrame* frame, nint code);
	void hStackFault(nint intr, InterruptFrame* frame, nint code);
	void hGeneralProtectionFault(nint intr, InterruptFrame* frame, nint code);
	void hPageFault(nint intr, InterruptFrame* frame, nint code);
	void hCoprocessorFault(nint code, InterruptFrame* frame);
	void hAlignmentCheck(nint intr, InterruptFrame* frame, nint code);
	void hMachineCheck(nint code, InterruptFrame* frame);
	void hSIMDFault(nint code, InterruptFrame* frame);
	void hKeyboardInt();
	void hPitTick(nint intr, InterruptFrame* frame);
	void hRtcTick();

	void hStub(nint intr);

}
#endif