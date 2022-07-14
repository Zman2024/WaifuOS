#pragma once
#ifndef H_CPU
#define H_CPU
#include <Types.h>

// Writing this gave me brain worms
// https://en.wikipedia.org/wiki/CPUID
namespace CPUID
{
	constexpr uint32 GET_VENDORSTRING = 0x00;
	constexpr uint32 GET_FEATURES = 0x01;
	constexpr uint32 GET_TLB = 0x02;
	constexpr uint32 GET_SERIAL = 0x03;
	constexpr uint32 GET_FEATURES_EXT = 0x07;

	enum ECX1 : uint32
	{
		SSE3 = 1 << 0,
		PCLMUL = 1 << 1,
		DTES64 = 1 << 2,
		MONITOR = 1 << 3,
		DS_CPL = 1 << 4,
		VMX = 1 << 5,
		SSE4A = 1 << 6,
		EST = 1 << 7,
		TM2 = 1 << 8,
		SSSE3 = 1 << 9,
		CID = 1 << 10,
		SDBG = 1 << 11,
		FMA = 1 << 12,
		CX16 = 1 << 13,
		XTPR = 1 << 14,
		PDCM = 1 << 15,
		PCID = 1 << 17,
		DCA = 1 << 18,
		SSE4_1 = 1 << 19,
		SSE4_2 = 1 << 20,
		X2APIC = 1 << 21,
		MOVBE = 1 << 22,
		POPCNT = 1 << 23,
		C_TSC = 1 << 24,
		AES = 1 << 25,
		XSAVE = 1 << 26,
		OSXSAVE = 1 << 27,
		AVX = 1 << 28,
		F16C = 1 << 29,
		RDRAND = 1 << 30,
		HYPERVISOR = 1U << 31,
	};

	enum EDX1 : uint32
	{
		FPU = 1 << 0,
		VME = 1 << 1,
		DE = 1 << 2,
		PSE = 1 << 3,
		D_TSC = 1 << 4,
		MSR = 1 << 5,
		PAE = 1 << 6,
		MCE = 1 << 7,
		CX8 = 1 << 8,
		APIC = 1 << 9,
		SEP = 1 << 11,
		MTRR = 1 << 12,
		PGE = 1 << 13,
		MCA = 1 << 14,
		CMOV = 1 << 15,
		PAT = 1 << 16,
		PSE36 = 1 << 17,
		PSN = 1 << 18,
		CLFLUSH = 1 << 19,
		DS = 1 << 21,
		ACPI = 1 << 22,
		MMX = 1 << 23,
		FXSR = 1 << 24,
		SSE = 1 << 25,
		SSE2 = 1 << 26,
		SS = 1 << 27,
		HTT = 1 << 28,
		TM = 1 << 29,
		IA64 = 1 << 30,
		PBE = 1U << 31,
	};

	enum EBX7 : uint32
	{
		FSGSBASE =				1	<<	0,			// Access to base of fs and gs
		SGX =					1	<<	2,			// Software Guard Extensions
		BMI1 =					1	<<	3,			// Bit Manipulation Instruction Set 1
		HLE =					1	<<	4,			// TSX Hardware Lock Elision
		AVX2 =					1	<<	5,			// Advanced Vector Extensions 2
		SMEP =					1	<<	7,			// Supervisor Mode Execution Prevention
		BMI2 =					1	<<	8,			// Bit Manipulation Instruction Set 2
		ERMS =					1	<<	9,			// Enhanced REP MOVSB/STOSB
		INVPCID =				1	<<	10,			// INVPCID instruction
		RTM =					1	<<	11,			// TSX Restricted Transactional Memory
		PQM =					1	<<	12,			// Platform Quality of Service Monitoring
		MPX =					1	<<	14,			// Intel MPX (Memory Protection Extensions)
		PQE =					1	<<	15,			// Platform Quality of Service Enforcement

		AVX512_F =				1	<<	16,			// AVX-512 Foundation
		AVX512_DQ =				1	<<	17,			// AVX-512 Doubleword and Quadword Instructions
		RDSEED =				1	<<	18,			// RDSEED instruction
		ADX =					1	<<	19,			// Intel ADX (Multi-Precision Add-Carry Instruction Extensions)
		SMAP =					1	<<	20,			// Supervisor Mode Access Prevention
		AVX512_IFMA =			1	<<	21,			// AVX-512 Integer Fused Multiply-Add Instructions
		PCOMMIT =				1	<<	22,			// PCOMMIT instruction
		CLFLUSHSHOPT =			1	<<	23,			// CLFLUSHOPT instruction
		CLWB =					1	<<	24,			// CLWB instruction
		INTEL_PT =				1	<<	25,			// Intel Processor Trace
		AVX512_PF =				1	<<	26,			// AVX-512 Prefetch Instructions
		AVX512_ER =				1	<<	27,			// AVX-512 Exponential and Reciprocal Instructions
		AVX512_CD =				1	<<	28,			// AVX-512 Conflict Detection Instructions
		SHA =					1	<<	29,			// Intel SHA extensions
		AVX512_BW =				1	<<	30,			// AVX-512 Byte and Word Instructions
		AVX512_VL =				1u	<<	31,			// AVX-512 Vector Length Extensions

	};

	enum ECX7 : uint32
	{
		PREFETCHWT1 =			1	<<	0,			// PREFETCHWT1 instruction
		AVX512_VBMI =			1	<<	1,			// AVX-512 Vector Bit Manipulation Instructions
		UMIP =					1	<<	2,			// User-mode Instruction Prevention
		PKU =					1	<<	3,			// Memory Protection Keys for User-mode pages
		OSPKU =					1	<<	4,			// PKU enabled by OS
		WAITPKG =				1	<<	5,			// Timed pause and user-level monitor/wait
		AVX512_VBMI2 =			1	<<	6,			// AVX-512 Vector Bit Manipulation Instructions 2
		CET_SS =				1	<<	7,			// Control flow enforcement (CET) shadow stack
		GFNI =					1	<<	8,			// Galois Field instructions
		VAES =					1	<<	9,			// Vector AES instruction set (VEX-256/EVEX)
		VPCLMULQDQ =			1	<<	10,			// CLMUL instruction set (VEX-256/EVEX)
		AVX512_VNNI =			1	<<	11,			// AVX-512 Vector Neural Network Instructions
		AVX512_BITALG =			1	<<	12,			// AVX-512 BITALG instructions
		TME_EN =				1	<<	13,			// IA32_TME related MSRs are supported
		AVX512_VPOPCNTDQ =		1	<<	14,			// AVX-512 Vector Population Count Double and Quad-word

		RDPID =					1	<<	22,			// Read Processor ID and IA32_TSC_AUX
		KL =					1	<<	23,			// Key Locker
		CLDEMOTE =				1	<<	25,			// Cache line demote
		MOVDIRI =				1	<<	27,			// ???
		MOVDIR64B =				1	<<	28,			// ???
		ENQCMD =				1	<<	29,			// Enqueue Stores
		SGX_LC =				1	<<	30,			// SGX Launch Configuration
		PKS =					1u	<<	31,			// Protection keys for supervisor-mode pages

	};

	enum EDX7 : uint32
	{
		AVX512_4VNNIW =			1	<<	2,			// 
		AVX512_4FMAPS =			1	<<	3,			// 
		FSRM =					1	<<	4,			// 
		UINTR =					1	<<	5,			// 
		AVX512_VP2INTERSECT =	1	<<	8,			// 
		SRBDS_CTRL =			1	<<	9,			// 
		MD_CLEAR =				1	<<	10,			// 
		RTM_ALWAYS_ABORT =		1	<<	11,			// 
		TSX_FORCEABORT =		1	<<	13,			// 
		SERIALIZE =				1	<<	14,			// 
		Hybrid =				1	<<	15,			// 

		TSXLDTRK =				1	<<	16,			// 
		PCONFIG =				1	<<	18,			// 
		LBR =					1	<<	17,			// 
		CET_IBT =				1	<<	18,			// 
		AMX_BF16 =				1	<<	22,			//
		AVX512_PF16 =			1	<<	23,			// 
		AMX_TILE =				1	<<	24,			// 
		AMX_INT8 =				1	<<	25,			// 
		SPEC_CTRL =				1	<<	26,			// 
		STIPB =					1	<<	27,			// 
		L1D_FLUSH =				1	<<	28,			// 
		IA32_ARCH_CAP =			1	<<	29,			// 
		IA32_CORE_CAP =			1	<<	30,			// 
		SSBD =					1u	<<	31,			// 

	};

}

namespace ControlRegister
{
	enum CR0 : uint32
	{
		PE = 1 << 0, // Protected Mode Enable
		MP = 1 << 1, // Monitor Co-Processor
		EM = 1 << 2, // x87 FPU Emulation
		TS = 1 << 3, // Task Switched
		ET = 1 << 4, // Extension Type (???)
		NE = 1 << 5, // Numeric Error
		
		WP = 1 << 16,  // Write Protect
		AM = 1 << 18,  // Alignment Mask
		NW = 1 << 29,  // Not Write Through
		CD = 1 << 30,  // Cache Disable
		PG = 1U << 31, // Paging
	};

	enum XCR0 : uint32
	{
		X87 = 1 << 0,		// x87 FPU/MMX support (must be 1)
		SSE = 1 << 1,		// XSAVE support for MXCSR and XMM registers
		AVX = 1 << 2,		// AVX enabled and XSAVE support for upper halves of YMM registers
		BNDREG = 1 << 3,	// MPX enabled and XSAVE support for BND0-BND3 registers
		BNDCSR = 1 << 4,	// MPX enabled and XSAVE support for BNDCFGU and BNDSTATUS registers
		OPMASK = 1 << 5,	// AVX-512 enabled and XSAVE support for opmask registers k0-k7
		ZMM_Hi256 = 1 << 6,	// AVX-512 enabled and XSAVE support for upper halves of lower ZMM registers
		Hi16_ZMM = 1 << 7,	// AVX-512 enabled and XSAVE support for upper ZMM registers
		PKRU = 1 << 9,		// XSAVE support for PKRU register
	};

	enum CR3 : uint32
	{
		PWT = 1 << 3, // Page Level Write Through (Not used if bit 17 of CR4 is 1)
		PCD = 1 << 4, // Page Level Cache Disable (Not used if bit 17 of CR4 is 1)

		// There is also the Page Directory Base Register

	};

	enum CR4 : uint32
	{
		VME = 1 << 0,			// Virtual 8086 Mode Extensions
		PVI = 1 << 1,			// Protected-mode Virtual Interrupts
		TSD = 1 << 2,			// Time Stamp Disable
		DE = 1 << 3,			// Debugging Extensions
		PSE = 1 << 4,			// Page Size Extension
		PAE = 1 << 5,			// Physical Address Extension
		MCE = 1 << 6,			// Machine Check Exception
		PGE = 1 << 7,			// Page Global Enabled
		PCE = 1 << 8,			// Performance-Monitoring Counter enable
		OSFXSR = 1 << 9,		// Operating system support for FXSAVE and FXRSTOR instructions
		OSXMMEXCPT = 1 << 10,	// Operating System Support for Unmasked SIMD Floating-Point Exceptions
		UMIP = 1 << 11,			// User-Mode Instruction Prevention (if set, #GP on SGDT, SIDT, SLDT, SMSW, and STR instructions when CPL > 0)
		VMXE = 1 << 13,			// Virtual Machine Extensions Enable
		SMXE = 1 << 14,			// Safer Mode Extensions Enable

		FSGSBASE = 1 << 16,		// Enables the instructions RDFSBASE, RDGSBASE, WRFSBASE, and WRGSBASE
		PCIDE = 1 << 17,		// PCID Enable
		OSXSAVE = 1 << 18,		// XSAVE and Processor Extended States Enable
		SMEP = 1 << 20,			// Supervisor Mode Execution Protection Enable
		SMAP = 1 << 21,			// Supervisor Mode Access Prevention Enable
		PKE = 1 << 22,			// Protection Key Enable
		CET = 1 << 23,			// Control-flow Enforcement Technology
		PKS = 1 << 24,			// Enable Protection Keys for Supervisor-Mode Pages

	};

	// this just has Task Priority Level
	enum CR8 : uint32
	{


	};

}

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

namespace CPU
{
	namespace Features
	{
		extern bool SSE;
		extern bool SSE2;
		extern bool SSE3;
		extern bool SSSE3;
		extern bool SSE4_1;
		extern bool SSE4_2;
		extern bool SSE4A;

		extern bool AVX;
		extern bool XSAVE;
		extern bool OSXSAVE;
		extern bool AVX2;
		extern bool AVX512; // there more to this but aaaaauuuh

		extern bool AES;
		extern bool VAES;
		extern bool SHA;

		extern bool RDRAND;
		extern bool RDSEED;
	}

	void GetFeatures();

	global uint32 GetXCR0();				// CPU.asm
	global void SetXCR0(uint32 xrc0);		// CPU.asm
}

#endif // !H_CPU
