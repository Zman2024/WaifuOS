#pragma once
#ifndef H_Scheduling
#define H_Scheduling
#include <Types.h>

struct TaskStateSegment
{
	uint32 rsv0;
	
	uint64 rsp0;
	uint64 rsp1;
	uint64 rsp2;

	uint64 rsv1;

	uint64 ist0;
	uint64 ist1;
	uint64 ist2;
	uint64 ist3;
	uint64 ist4;
	uint64 ist5;
	uint64 ist6;

	uint64 rsv2;
	
	uint16 rsv3;
	uint16 IOMapBase;

};

global TaskStateSegment gTSS;

#endif // !H_Scheduling