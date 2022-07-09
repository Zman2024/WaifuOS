#include <Scheduling.h>

attribute((aligned(PAGE_SIZE))) TaskStateSegment gTSS;
byte IOPB[33];
