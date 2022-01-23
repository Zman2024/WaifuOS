#include <PageMapIndexer.h>



PageMapIndexer::PageMapIndexer(void* vAddress)
{
	PageMapIndexer((u64)vAddress);
}

PageMapIndexer::PageMapIndexer(uint64 vAddress)
{
	vAddress >>= 12;

	Pi = vAddress & 0x01FF;
	vAddress >>= 9;
	PTi = vAddress & 0x01FF;
	vAddress >>= 9;
	PDi = vAddress & 0x01FF;
	vAddress >>= 9;
	PDPi = vAddress & 0x01FF;

}