#include "staticmapimp.h"
#include <string.h>


CharMapBase* CHARMAP_get(CharMapImp* obj,char* key)
{
    if(obj->now_size==0)
        return NULL;
    int higher = obj->now_size-1;
    if(strcmp(*obj->buff[higher],key)==0)
    {
        return obj->buff[higher];
    }
    int lower = 0;
    if(strcmp(*obj->buff[lower],key)==0)
    {
        return obj->buff[lower];
    }
    int pos = (higher+lower)/2;
    while(pos!=lower && pos!=higher)
    {
        int ret = strcmp(*obj->buff[pos],key);
        if( ret == 0)
        {
            return obj->buff[pos];
        }
        if(ret<0)
        {
            lower = pos;
        }
        else
        {
            higher = pos;
        }
        pos = (higher+lower)/2;
    }
    return NULL;
}

int CHARMAP_insert(CharMapImp* obj, CharMapBase* node)
{
    if(obj->now_size==obj->max_size)
    {
        return CHARMAP_ERROR_IS_FULL;
    }
    if(CHARMAP_get(obj,*node))
    {
        return CHARMAP_ERROR_HAS_ITEM;
    }
    int lower = obj->now_size-1;
    if(strcmp(*obj->buff[lower],*node)>0)
    {
        obj->buff[obj->now_size++] = node;
        return 0;
    }

    int higher = 0;
    if(strcmp(*obj->buff[higher],*node)<0)
    {
        memmove(obj->buff+1,obj->buff,obj->now_size++);
        obj->buff[0]=node;
    }

    int pos = obj->now_size/2;
    while(higher-lower!=1)
    {
        if(strcmp(*obj->buff[pos],*node)>0)
        {
            higher = pos;
        }
        else
        {
            lower = pos;
        }
        pos = (higher+lower)/2;
    }
    memmove(obj->buff+pos+1,obj->buff+pos,obj->now_size-pos);
    obj->now_size++;
    obj->buff[pos]=node;
    return 0;
}


CharMapBase* CHARMAP_find(CharMapImp* obj,char* key)
{
    for(int i=obj->now_size;i>0;)
    {
        if(strcmp((*obj->buff[--i]),key)==0)
        {
            return obj->buff[i];
        }
    }
    return NULL;
}

int CHARMAP_append(CharMapImp* obj,CharMapBase* node)
{
    if(CHARMAP_find(obj,*node))
    {
        return CHARMAP_ERROR_HAS_ITEM;
    }
    if(obj->now_size>=obj->max_size)
    {
        return CHARMAP_ERROR_IS_FULL;
    }
    obj->buff[obj->now_size++] = node;
    return 0;
}

int CHARMAP_remove(CharMapImp* obj, char* key)
{
    int pos = 0;
    for(;pos<obj->now_size;pos++)
    {
        if(strcmp((*(obj->buff[pos])),key)==0)
        {
            break;
        }
    }
    if(pos==obj->now_size)
    {
        return CHARMAP_ERROR_NO_SUCH_ITEM;
    }
    obj->now_size--;
    for(int i = pos; i<obj->now_size; i++)
    {
          obj->buff[i] = obj->buff[i+1];
    }
    obj->buff[obj->now_size] = 0;
    return 0;
}
