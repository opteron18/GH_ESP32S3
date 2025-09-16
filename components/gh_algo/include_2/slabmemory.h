#ifndef SLABMEMORY_H
#define SLABMEMORY_H

#include <stdint.h>
#include "gh_errorcode.h"
#include "slabmemorydata.h"

#define INNER_BUFF_SIZE 4096

void initialSlab(int aligned_size);

LinkedBuff* slabAlloc(LinkedBuff* front);

int slabFree(LinkedBuff* node);

uint8_t* slabBase(void);

#endif //SLABMEMORY_H
