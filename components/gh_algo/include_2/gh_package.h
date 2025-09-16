/**
 * @file verification_module.h
 * @author wangjinxin (wangjinxin@goodix.com)
 * @brief  byte array function call
 * @version 0.1
 * @date 2023-03-27
 * 
 * @copyright Copyright (c) 2023
 */

#ifndef GHPACKAGE_H
#define GHPACKAGE_H

#include <stdint.h>
#include <stddef.h>
#include "gh_errorcode.h"
#include "gh_packagedata.h"

//***********************Config*********************
//SINGLE PARAM SIZE CAN NOT EXCESS SLAB SIZE
#define MAX_SUPPORT_FORMAT_LENGTH 50  //stack to support
#define MAX_PARAM_NUMBER 10 //pointer type(RPCPoint) regard as 1
#define MAX_SINGLE_PARAM_SIZE 256

//======================interface struct====================
enum
{
    GHRPC_ERROR_BASE(PACKAGE),
    GHRPC_ERROR_DETAIL_FORMAT,
    GHRPC_ERROR_DETAIL_SIZE_OVER_BLOCK,
    GHRPC_ERROR_UNPACKAGE,
    GHRPC_ERROR_PARAM_TOO_MUCH,
    GHRPC_ERROR_ANALYSIS_INNER_ERROR
};

typedef enum{
    GH_PRO_TYPE_DOUBLE,/// float, array use
    GH_PRO_TYPE_UNSIGNED,/// unsigned integer, array use
    GH_PRO_TYPE_SIGNED,/// signed integer
    GH_PRO_TYPE_PACK/**< extern */
} ProPackType;  //use for u8 compress

typedef struct{
uint8_t pack_type : 2; // type identification
uint8_t reserve: 5; // subtype reserve bits
uint8_t flag:1; //flag bit
} HeaderBase; //base header

//**************Interface**************************
typedef struct{
    uint8_t pack_type : 2; // ProPackType
    uint8_t is_array: 1; // if has length info
    uint8_t width : 3; // unit lenth 2^x
    uint8_t end:1;//0-0-...-1
    uint8_t split:1;
} TypeHeader;  // type mark header
typedef int(*PackagerHeader)(void* info,void* base,int size);

// custom base class
typedef struct 
{
    PackagerHeader move_to_buffer;
    uint8_t info[FLEXIBLE_ARRAY];
} PackageBase;


/**
 * @brief 
 * 
 * @param base_addr 
 * @param align 
 * @param format 
 * @param args 
 * @return size_t 
 */
int GHRPC_vpackage(PackageBase* packager,char* format,va_list args); // va_list版本，用于传递参数

/**
 * @brief 
 * 
 * @param base_addr 
 * @param align 
 * @param format 
 * @param ... 
 * @return size_t 
 */
int GHRPC_package(PackageBase* packager,char* format,...); // 主版本，使用可变参数

typedef struct 
{
    char* detail;
    uint8_t*(*getByte)(void* info);
    int(*getBytes)(void*info,uint8_t* base,int size);
} UnpackBase;

typedef struct 
{
    UnpackBase base;
    uint8_t info[];
} Unpacker;

/**
 * @brief 
 * 
 * @param node 
 * @param struct_pointer 
 * @return int 
 */
int GHRPC_unpackage(Unpacker* datas, uint8_t* struct_pointer,int* totel_size);

/**
 * @brief return data unpack
 * 
 * @param struct_pointer 
 * @param detail 
 * @param args 
 * @return int 
 */
int GHRPC_vunpack(uint8_t* struct_pointer, char* detail, va_list args);

enum
{
    GHRPC_HEAD_DATA,
    GHRPC_HEAD_ARRAY,
    GHRPC_HEAD_ERROR
};
uint8_t GHRPC_headType(uint8_t* head);

uint8_t GHRPC_setMode(int mode);

#endif
