#ifndef GHRPCCORE_H
#define GHRPCCORE_H

#include <stdint.h>

#include "gh_errorcode.h"
#include "slabmemorydata.h"
#include "gh_packagedata.h"

#ifdef __cplusplus
extern "C"{
#endif
static const uint8_t Frame_Header[] = {0xAA,0x11};
#define GHRPC_FRAME_SIZE 240
#define MAX_SUPPORT_KEY_SIZE 32
#define DYNAMIC_NODE_SIZE 3
#define COMM_RETRY_TIME 500
#define COMM_RETRY_ROUND 100
#define PASS_MESSAGE_KEEP_TIME 10 //if one split frame comm interrupt over this number infos will be clear

typedef void(*RpcFunction)(void* struct_point,int size,void* info);

typedef struct
{
    char* key; //alias char*
    char* detail;
    LinkedBuff* header;
    RpcFunction func;
    void* info; //state 
} InvokeNode;

enum
{
    GHRPC_ERROR_BASE(INVOKE),
    CARIDFFRPC_ERROR_FORMAT_ERROR,
    GHRPC_ERROR_KEY_OVER_MAX_SIZE,
    GHRPC_ERROR_NOT_UNDER_INVOKE,
    GHRPC_ERROR_SEND_FAIL,
    GHRPC_ERROR_SEND_STATUS,
    GHRPC_ERROR_MEMORY_NOT_ENOUGH,
    GHRPC_ERROR_LOSE_FRAME,
};

enum
{
    CARIDFFRPC_MODE_NO_MODE,
    CARIDFFRPC_MODE_CHANGE_ENDIAN,
};

typedef struct
{
  void (*lock)();  //控制多线程发送时的时序，无操作系统不用管
  void (*unlock)(); //控制多线程发送时的时序，无操作系统不用管
  void (*sendFunction)(void*,int); //发送函数
  void (*delay)(); //延迟函数，用于可靠传输和有返回值的时候，控制轮询的时间片，无操作系统不用管
  int mode;
}GhRPCInitialInfo; //对象，init后内部以此信息进行操作

/**
 * @brief 
 * 
 * @param info 
 */
void GHRPC_init(GhRPCInitialInfo info);

/**
 * @brief 
 * 
 * @param buff 
 * @param frame_size 
 */
void GHRPC_process(uint8_t* buff,int frame_size,uint8_t restart);

/**
 * @brief no block, quick&no repeat way 
 * 
 * @param key 
 * @param format 
 * @param ... 
 * @return int 
 */
int GHRPC_publish(const char* key,const char* format,...);

/**
 * @brief block, repeat send left
 * 
 * @param key 
 * @param format 
 * @param ... 
 * @return int 
 */
int GHRPC_send(const char* key,const char* format,...);

/**
 * @brief block, loop to check return
 * 
 * @param ret_buff 
 * @param key 
 * @param format 
 * @param ... 
 * @return int 
 */
int GHRPC_call(void* ret_buff, const char* key,const char* format,...);
/**
 * @brief block, loop to check return& repeat send
 * 
 * @param ret_buff 
 * @param key 
 * @param format 
 * @param ... 
 * @return int 
 */
int GHRPC_sall(void* ret_buff, const char* key,const char* format,...);

/**
 * @brief some static function or secure comm has return param
 * 
 * @param format 
 * @param ... 
 * @return int 
 */
int GHRPC_return(const char* format, ...);
/**
 * @brief 
 * 
 * @param ret_buff 
 * @param format 
 * @param ... 
 * @return int 
 */
int GHRPC_unpack(void* ret_buff, const char* format,...);

#ifdef __cplusplus
}
#endif

#endif //GHRPCCORE_H
