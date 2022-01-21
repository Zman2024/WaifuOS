#include <Types.h>
#include <Structs.h>
#include <KernelUtils.h>

namespace Kernel
{
	global void KernelStart(BootInfo bootInfo)
	{
		Kernel::InitializeKernel(&bootInfo);
		OS_HLT;
	}
}