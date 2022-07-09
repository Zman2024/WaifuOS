#pragma once
#ifndef H_Scheduling
#define H_Scheduling
#include <Types.h>
#include <BitMap.h>

struct TaskStateSegment
{
	uint32 rsv0;
	
	uint64 rsp0;
	uint64 rsp1;
	uint64 rsp2;

	uint64 rsv1;

	uint64 ist_rsp1;
	uint64 ist_rsp2;
	uint64 ist_rsp3;
	uint64 ist_rsp4;
	uint64 ist_rsp5;
	uint64 ist_rsp6;
	uint64 ist_rsp7;

	uint64 rsv2;
	
	uint16 rsv3;
	uint16 IOMapBase = sizeof(TaskStateSegment); // we put the IOPB right after the TSS

};

global TaskStateSegment gTSS;
global void LoadTSS(size_t entryOffset); // defined in asmUtils.asm

#endif // !H_Scheduling