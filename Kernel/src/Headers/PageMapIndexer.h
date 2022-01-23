#pragma once
#ifndef H_PageMapIndexer
#define H_PageMapIndexer
#include <Types.h>

struct PageMapIndexer
{
	uint64 PDPi;
	uint64 PDi;
	uint64 PTi;
	uint64 Pi;

	PageMapIndexer(void* vAddress);
	PageMapIndexer(uint64 vAddress);
};

#endif