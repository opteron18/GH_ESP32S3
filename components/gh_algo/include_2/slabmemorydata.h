#ifndef SLABMEMORYDATA_H
#define SLABMEMORYDATA_H

#include <stdint.h>
//#include "gh_rpccore.h"
#include "gh_errorcode.h"

typedef struct
{
    uint8_t frame_idx;
    uint8_t invoke_idx;
} GhIndex;

typedef struct OrderedBuffer
{
  GhIndex index;
  struct OrderedBuffer* next;
  struct OrderedBuffer* front;
  int length;
  uint8_t buff[FLEXIBLE_ARRAY];
} LinkedBuff;

#endif
