/**
 * @file staticrbtreemapimp.h
 * @author wangjinxin (wangjinxin@goodix.com)
 * @brief sort array base map
 * @version 0.1
 * @date 2023-03-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CARDIFF_CHARMAP_H
#define CARDIFF_CHARMAP_H

#include "gh_rpccore.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

//***********************define***************************
typedef char* CharMapBase; //

typedef struct
{
    CharMapBase key;
    uint8_t other[FLEXIBLE_ARRAY];
} CharMapNode; //&CharMapNode == char** = CharMapBase*

enum
{
    GHRPC_ERROR_BASE(CHAMMAP),
    CHARMAP_ERROR_HAS_ITEM,
    CHARMAP_ERROR_IS_FULL,
    CHARMAP_ERROR_NO_SUCH_ITEM
};

//**********************Interface*************************
typedef struct{
    int now_size;
    int unit_size;
    int max_size;
    CharMapBase** buff; //3-dim char list ,&CharMapNode[]
} CharMapImp;

/**
 * @brief Binary lookup
 * 
 * @param obj 
 * @param key 
 * @return CharMapBase* 
 */
CharMapBase* CHARMAP_get(CharMapImp* obj,char* key);

/**
 * @brief 
 * 
 * @param obj 
 * @param key 
 * @return CharMapBase* 
 */
CharMapBase* CHARMAP_find(CharMapImp* obj,char* key);

/**
 * @brief insert a node to map
 * node strcut need match to unit_size
 */
int CHARMAP_insert(CharMapImp* obj,CharMapBase* node); 

/**
 * @brief Sequential lookup
 * 
 * @param obj 
 * @param node 
 * @return int 
 */
int CHARMAP_append(CharMapImp* obj,CharMapBase* node);

/**
 * @brief Sequential remove
 * 
 * @param obj 
 * @param key 
 * @return int 
 */
int CHARMAP_remove(CharMapImp* obj, char* key);

#ifdef __cplusplus
}
#endif

#endif  // CARDIFF_CHARMAP_H
