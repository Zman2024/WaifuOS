#pragma once
#ifndef H_CPU
#define H_CPU
#include <Types.h>

enum struct CPUIDCodes : uint32
{
	ECX_SSE3 = 1 << 0,
	ECX_PCLMUL = 1 << 1,
	ECX_DTES64 = 1 << 2,
	ECX_MONITOR = 1 << 3,
	ECX_DS_CPL = 1 << 4,
	ECX_VMX = 1 << 5,
	ECX_SMX = 1 << 6,
	ECX_EST = 1 << 7,
	ECX_TM2 = 1 << 8,
	ECX_SSSE3 = 1 << 9,
	ECX_CID = 1 << 10,
	ECX_SDBG = 1 << 11,
	ECX_FMA = 1 << 12,
	ECX_CX16 = 1 << 13,
	ECX_XTPR = 1 << 14,
	ECX_PDCM = 1 << 15,
	ECX_PCID = 1 << 17,
	ECX_DCA = 1 << 18,
	ECX_SSE4_1 = 1 << 19,
	ECX_SSE4_2 = 1 << 20,
	ECX_X2APIC = 1 << 21,
	ECX_MOVBE = 1 << 22,
	ECX_POPCNT = 1 << 23,
	ECX_TSC = 1 << 24,
	ECX_AES = 1 << 25,
	ECX_XSAVE = 1 << 26,
	ECX_OSXSAVE = 1 << 27,
	ECX_AVX = 1 << 28,
	ECX_F16C = 1 << 29,
	ECX_RDRAND = 1 << 30,
	ECX_HYPERVISOR = 0b10000000000000000000000000000000,

	EDX_FPU = 1 << 0,
	EDX_VME = 1 << 1,
	EDX_DE = 1 << 2,
	EDX_PSE = 1 << 3,
	EDX_TSC = 1 << 4,
	EDX_MSR = 1 << 5,
	EDX_PAE = 1 << 6,
	EDX_MCE = 1 << 7,
	EDX_CX8 = 1 << 8,
	EDX_APIC = 1 << 9,
	EDX_SEP = 1 << 11,
	EDX_MTRR = 1 << 12,
	EDX_PGE = 1 << 13,
	EDX_MCA = 1 << 14,
	EDX_CMOV = 1 << 15,
	EDX_PAT = 1 << 16,
	EDX_PSE36 = 1 << 17,
	EDX_PSN = 1 << 18,
	EDX_CLFLUSH = 1 << 19,
	EDX_DS = 1 << 21,
	EDX_ACPI = 1 << 22,
	EDX_MMX = 1 << 23,
	EDX_FXSR = 1 << 24,
	EDX_SSE = 1 << 25,
	EDX_SSE2 = 1 << 26,
	EDX_SS = 1 << 27,
	EDX_HTT = 1 << 28,
	EDX_TM = 1 << 29,
	EDX_IA64 = 1 << 30,
	EDX_PBE = 0b10000000000000000000000000000000
};

namespace FLAGS
{
	enum : nint
	{
		CF = 0b1,				// Carry Flag
		RSV0 = CF << 1,
		PF = RSV0 << 1,			// Parity Flag
		RSV1 = PF << 1,
		AF = RSV1 << 1,			// Adjust Flag
		RSV2 = AF << 1,
		ZF = RSV2 << 1,			// Zero Flag
		SF = ZF << 1,			// Sign Flag
		TF = SF << 1,			// Trap Flag
		IF = TF << 1,			// Interrupt Flag
		DF = IF << 1,			// Direction Flag
		OF = DF << 1,			// Overflow Flag
		IOPL0 = OF << 1,		// IO Privelige Level Bit 0
		IOPL1 = IOPL0 << 1,		// IO Privelige Level Bit 1
		NT = IOPL1 << 1,		// Nested Task Flag
		RSV3 = NT << 1,

		// ===== EFLAGS =====

		RF = 0x10000,			// Resume Flag
		VM = RF << 1,			// Virtual 8086 Mode
		AC = VM << 1,			// Alignment Check
		VIF = AC << 1,			// Virtual Interrupt Flag
		VIP = VIF << 1,			// Virtual Interrupt Pending
		ID = VIP << 1,			// CPUID Allowed Flag

		// ===== RFLAGS =====

		/* none cause all reserved */

	};
}

#endif // !H_CPU
