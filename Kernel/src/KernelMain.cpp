#include <Types.h>
#include <Structs.h>
#include <KernelUtils.h>
#include <Globals.h>
#include <cstr.h>

namespace Kernel
{
	global void KernelStart(BootInfo bootInfo)
	{
		Kernel::InitializeKernel(&bootInfo);

		gConsole.WriteLine("Ok this shit works. good");
		gConsole.WriteLine(cstr::ToString(123456789));
		gConsole.WriteLine(cstr::ToString(0x123456789, true));


		OS_HLT;
	}
}