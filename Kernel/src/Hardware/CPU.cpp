#include <CPU.h>
#include <Globals.h>

namespace CPU
{
	namespace Features
	{
		bool SSE = false;
		bool SSE2 = false;
		bool SSE3 = false;
		bool SSSE3 = false;
		bool SSE4_1 = false;
		bool SSE4_2 = false;
		bool SSE4A = false;

		bool AVX = false;
		bool XSAVE = false;
		bool OSXSAVE = false;
		bool AVX2 = false;
		bool AVX512 = false; // theres more to this but aaaaauuuh

		bool AES = false;
		bool VAES = false;
		bool SHA = false;

		bool RDRAND = false;
		bool RDSEED = false;
	}

	void GetFeatures()
	{
		using namespace Features;
		uint32 eax = 0, ebx = 0, ecx = 0, edx = 0;

		// compiler is dumb and generates this stupidity
		// i should just write this in asm but i too lazy
		/*
			mov eax, CPUID::GET_FEATURES
			mov esi, dword [stack.ebx]		<---- why???
			mov ecx, dword [stack.ecx]
			mov edx, dword [stack.edx]
			mov ebx, esi					<---- why???
			cpuid
		*/
		cpuid(CPUID::GET_FEATURES, eax, ebx, ecx, edx);

		SSE = CPUID::EDX1::SSE & edx;
		SSE2 = CPUID::EDX1::SSE2 & edx;
		SSE3 = CPUID::ECX1::SSE3 & ecx;
		SSSE3 = CPUID::ECX1::SSSE3 & ecx;
		SSE4_1 = CPUID::ECX1::SSE4_1 & ecx;
		SSE4_2 = CPUID::ECX1::SSE4_2 & ecx;
		SSE4A = CPUID::ECX1::SSE4A & ecx;
		AES = CPUID::ECX1::AES & ecx;

		AVX = CPUID::ECX1::AVX & ecx;
		XSAVE = CPUID::ECX1::XSAVE & ecx;
		OSXSAVE = CPUID::ECX1::OSXSAVE & ecx;

		RDRAND = CPUID::ECX1::RDRAND & ecx;

		eax = 0, ebx = 0, ecx = 0, edx = 0;

		cpuid(CPUID::GET_FEATURES_EXT, eax, ebx, ecx, edx);

		AVX2 = CPUID::EBX7::AVX2 & ebx;
		SHA = CPUID::EBX7::SHA & ebx;
		RDSEED = CPUID::EBX7::RDSEED & ebx;
		VAES = CPUID::ECX7::VAES & ecx;

	}

}