#include "slabmemory.h"

#include <string.h>
#include <stdio.h>
uint8_t s_inner_slab_buffer[INNER_BUFF_SIZE];

//*****************slab memory manage singleton class*****************
//todo: errors
static struct SlabStruct
{
  uint16_t max_size;
  uint16_t min_alloc_index;
  uint16_t slab_size;
  uint8_t* slab_base_addr;
  uint16_t next_alloc;
} s_slab_handle;

void initialSlab(int aligned_size)
{
    s_slab_handle.max_size = INNER_BUFF_SIZE/aligned_size;
    s_slab_handle.min_alloc_index = s_slab_handle.max_size;
    s_slab_handle.slab_base_addr=s_inner_slab_buffer;
    s_slab_handle.next_alloc = s_slab_handle.max_size-1;
    s_slab_handle.slab_size=aligned_size;
    *(uint16_t*)(s_slab_handle.slab_base_addr) = s_slab_handle.max_size;
    for(int idx=1;idx<s_slab_handle.max_size;idx++)
    {
        *(uint16_t*)(s_slab_handle.slab_base_addr+idx*aligned_size) = idx-1;
    }
}

LinkedBuff* slabAlloc(LinkedBuff* front)
{
//    printf("alloc:%d\n",s_slab_handle.next_alloc);
//    fflush(stdout);
    if(s_slab_handle.next_alloc>=s_slab_handle.max_size)
    {
        return NULL;
    }
    LinkedBuff* base = (LinkedBuff*)(s_slab_handle.slab_base_addr+s_slab_handle.slab_size*s_slab_handle.next_alloc);
    if(s_slab_handle.min_alloc_index>s_slab_handle.next_alloc)
    {
        s_slab_handle.min_alloc_index = s_slab_handle.next_alloc;
        *(uint16_t*)(s_slab_handle.slab_base_addr) = s_slab_handle.max_size;
        for(int idx=1;idx<s_slab_handle.min_alloc_index;idx++)
        {
            *(uint16_t*)(s_slab_handle.slab_base_addr+idx*s_slab_handle.slab_size) = idx-1;
        }
    }
    if(*(uint16_t*)base>=s_slab_handle.max_size)
    {
        return NULL;
    }
    s_slab_handle.next_alloc= *(uint16_t*)base;
    memset(base,0,s_slab_handle.slab_size);
    if(front)
    {
        if(front->next)
        {
            base->next = front->next;
            base->next->front = base;
        }
        base->front=front;
        base->front->next = base;
    }
    return base;
}

static void insertSlab(int idx,LinkedBuff* node)
{
    int insert_index = s_slab_handle.next_alloc;
    uint16_t* last_addr = NULL;
    while(insert_index>idx)
    {
        last_addr = (uint16_t*)(s_slab_handle.slab_base_addr+insert_index*s_slab_handle.slab_size);
        insert_index = *last_addr;
    }
    *(uint16_t*)node = insert_index;
    *last_addr = idx;
}

/**
 * @brief loop idx and find the minimal index not in memory pool
 */
static int insertWithFindMin(LinkedBuff* node)
{
    int now_index = s_slab_handle.max_size;
    int min_index = now_index;
    int past_min = s_slab_handle.min_alloc_index;
    uint16_t* last_addr =  (uint16_t*)(s_slab_handle.slab_base_addr+s_slab_handle.next_alloc*s_slab_handle.slab_size);
    uint16_t last_index =  *(uint16_t*)last_addr;
    while(now_index>past_min)
    {
        if(last_index == now_index)
        {
            last_addr = (uint16_t*)(s_slab_handle.slab_base_addr+last_index*s_slab_handle.slab_size);
            last_index = *(uint16_t*)last_addr;
        }
        else
        {
            min_index=now_index;
        }
        now_index--;
    }
    *(uint16_t*)node = last_index;
    *last_addr = past_min;
    return min_index;
}

int slabFree(LinkedBuff* node)
{
    if(node==NULL)
        return 0;
    if(node->front)
        node->front->next = node->next;
    if(node->next)
        node->next->front = node->front;
    int index = ((uint8_t*)node - (uint8_t*)(s_slab_handle.slab_base_addr))/s_slab_handle.slab_size;
//    printf("free:%d\n",index);
//    fflush(stdout);
    if(index<0 || index >s_slab_handle.max_size)
    {
        return 1;
    }
    *(uint16_t*)(s_slab_handle.slab_base_addr) = s_slab_handle.max_size;
    for(int idx=1;idx<s_slab_handle.min_alloc_index;idx++)
    {
        *(uint16_t*)(s_slab_handle.slab_base_addr+idx*s_slab_handle.slab_size) = idx-1;
    }
    for(uint16_t next_alloc=s_slab_handle.next_alloc;next_alloc<s_slab_handle.max_size;\
        next_alloc = *(uint16_t*)(s_slab_handle.slab_base_addr+next_alloc*s_slab_handle.slab_size))
    {
        if(next_alloc==index)
        {
            return 1;
        }
    }

    if(index == s_slab_handle.min_alloc_index)
    {
        if(s_slab_handle.min_alloc_index>s_slab_handle.next_alloc)
        {
            *(uint16_t*)node = s_slab_handle.next_alloc;
            s_slab_handle.next_alloc++;
            s_slab_handle.min_alloc_index++;
        }
        else
        {
            s_slab_handle.min_alloc_index = insertWithFindMin(node);
        }
    }
    else if (index>s_slab_handle.next_alloc || s_slab_handle.next_alloc >= s_slab_handle.max_size)
    {
        *(uint16_t*)node = s_slab_handle.next_alloc;
        s_slab_handle.next_alloc = index;
    } 
    else
    {
        insertSlab(index,node);
    }
    return 0;
}

//for invoke there,not provide for other
uint8_t* slabBase(void)
{
    return s_slab_handle.slab_base_addr;
}
