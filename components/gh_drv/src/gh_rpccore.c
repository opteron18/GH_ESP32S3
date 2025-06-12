/**
 * @file ghrpccore.c
 * @author your name (you@domain.com)
 * @brief signleton comm,
 * rx only support running in one thread!!!
 * tx support multi thread
 * 
 * @version 0.1
 * @date 2023-06-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "gh_rpccore.h"
#include "gh3x_demo_config.h"
#include "gh_rpc_functions.h"
#include <stdarg.h>
#include <string.h>

#include "staticmapimp.h"
#include "gh_package.h"
#include "slabmemory.h"


#define FRAME_NUMBER_INDEX sizeof(Frame_Header)
#define LAST_FRAME_FIX_INDEX 255

static TypeHeader u8_header = {
    GH_PRO_TYPE_UNSIGNED,
    0,
    3,
    0,
    0
};

//==================signleton obj(wrap all s_xxx datas with a handle class to support multi interface)==================
static CharMapBase* dynamic_nodes_buffer[DYNAMIC_NODE_SIZE];
static CharMapImp s_dynamic_nodes;
static CharMapImp s_static_nodes;

static struct 
{
    uint8_t is_secure;
    char topic[MAX_SUPPORT_KEY_SIZE];
    uint8_t is_fin;
    InvokeNode* node;
} s_invoke_context = {0};

static struct 
{
    uint8_t frame_size;
    GhIndex index;
    uint8_t crc;
    LinkedBuff* now_buffer;
} s_frame_context = {0};

GhRPCInitialInfo s_com_infos;

static uint8_t s_send_index;


//==============================tx group functions=================
//----------------------comm part --------------------
enum
{
    SERCURE_INVOKE_RECIEVE_FRAME,
    SERCURE_INVOKE_NO_SUCH_FUNCTION,
    SERCURE_INVOKE_RETURN,
    SERCURE_INVOKE_ERROR
};

//Frame format
typedef struct{
    uint8_t pack_type : 2; // ProPackType
    uint8_t is_array: 1; // if has length info
    uint8_t width : 3; // unit lenth 2^x
    uint8_t secure:1;// secure comm type
    uint8_t fin:1; // final frame of invok
} TypeKEY;

typedef struct
{
    uint8_t header[sizeof(Frame_Header)];
    uint8_t length;
    TypeKEY key_header;
    RPCPoint key_data;
    uint8_t com_id;
    uint8_t frame_id;
    RPCPoint param_data;
    uint8_t crc;
}FrameData;

static uint8_t calCrc(uint8_t* base, size_t size)
{
    uint8_t crc =0;
    for(int index=0;index<size;index++ )
    {
        crc += base[index];
    }
    return crc;
}

static int toFrameData(FrameData* data,uint8_t* base)
{
    data->crc =0;
    data->length = 0;
    int index = sizeof(Frame_Header);
    GH3X_Memcpy(base , Frame_Header, index);
    index++;//left for length
    base[index++] = T2(uint8_t,data->key_header);
    data->crc += base[index-1];
    data->length+=1;
    if(data->key_data.size==1)
    {
        base[index++] = *(uint8_t*)(data->key_data.point);
        data->crc += base[index-1];
        data->length +=1;
    }
    else
    {
        base[index++] = data->key_data.size;
        data->crc += base[index-1];
        GH3X_Memcpy(base+index,data->key_data.point,data->key_data.size);
        index += data->key_data.size;
        data->crc += calCrc(data->key_data.point,data->key_data.size);
        data->length += data->key_data.size +1;
    }
    if(data->key_header.secure)
    {
        base[index++] = data->com_id;
        data->crc +=  data->com_id;
        data->length +=1;
    }
    if(data->key_header.fin==0)
    {
        base[index++] = data->frame_id;
        data->crc += data->frame_id;
        data->length +=1;
    }
    GH3X_Memcpy(base+index,data->param_data.point,data->param_data.size);
    index += data->param_data.size;
    data->length += data->param_data.size;
    data->crc += calCrc(data->param_data.point,data->param_data.size);
    base[index++]=data->crc;
    base[sizeof (Frame_Header)] = data->length;
    return index;
}

static int initialFrameData(FrameData* datas,char* key)
{
    memset(datas,0,sizeof(FrameData));
    s_com_infos.lock();
    datas->param_data.point = slabAlloc(NULL);
    s_com_infos.unlock();
    if(datas->param_data.point==NULL)
    {
        return GHRPC_ERROR_MEMORY_NOT_ENOUGH;
    }
    datas->param_data.size =0;
    datas->key_data.point=key;
    datas->key_data.size = strlen(key);
    datas->key_header.pack_type=GH_PRO_TYPE_SIGNED;
    datas->key_header.width=3;
    datas->key_header.is_array = (datas->key_data.size!=1);
    return 0;
}

static int freeFrameData(FrameData* datas)
{
    /*CARIDFF_EXCEPTION_FUNCTION*/
    if(datas->param_data.point)
    {
        s_com_infos.lock();
        memset(datas->param_data.point,0,sizeof(LinkedBuff));
        slabFree(datas->param_data.point);
        s_com_infos.unlock();
    }
    return 0;
}

static int insertDynamicNode(InvokeNode* node)
{

    int retry_time = COMM_RETRY_TIME+1;
    while(--retry_time)
    {
        s_com_infos.lock();
        int ret = CHARMAP_append(&s_dynamic_nodes,(CharMapBase*)node);
        s_com_infos.unlock();
        if(ret)
        {
            if(retry_time>1)
                s_com_infos.delay();
        }
        else
        {
            break;
        }
    }
    if(retry_time==0)
    {
        return GHRPC_ERROR_SEND_STATUS;
    }
    return 0;
}

static int removeAllBufferList(LinkedBuff* head)
{
    LinkedBuff* delete_bf;
    while(head)
    {
        delete_bf =head;
        head = head->next;
        slabFree(delete_bf);
    }
    return 0;
}

static int removeDynamicNode(char* key)
{
    s_com_infos.lock();
    if(strcmp(s_invoke_context.topic,key)==0)
    {
        s_invoke_context.node =NULL;
    }
    InvokeNode* node = (InvokeNode*)CHARMAP_find(&s_dynamic_nodes,key);
    if(node)
    {
        removeAllBufferList(node->header);
    }
    int ret = CHARMAP_remove(&s_dynamic_nodes,key);
    s_com_infos.unlock();   
    return ret;
}



//-------------------------unsecure part-------------------
/**
 * @brief keep state between pack & return
 *
 */
typedef struct
{
    int max_payload_size;
    FrameData data;
    LinkedBuff* send_buff;
    void* ret_buff;
} GhQuickPackInfo;

/**
 * @brief packer for unsecure comm
 *
 */
typedef struct
{
    PackagerHeader header;
    GhQuickPackInfo info;
} GhQuickPackager;

/**
 * @brief
 *
 * @param info
 */
void sendQuickPack(GhQuickPackInfo* info)
{
    info->send_buff->length = toFrameData(&info->data,info->send_buff->buff);
    s_com_infos.sendFunction(info->send_buff->buff,info->send_buff->length);
    info->data.frame_id++;
    info->data.param_data.size=0;
}

static int quickContentPack(GhQuickPackInfo* info, uint8_t* base, int size)
{
    RPCPoint* buff = &(info->data.param_data);
    int this_time = info->max_payload_size - buff->size;
    uint8_t* origin_base = base;
    while(size >= this_time)
    {
        GH3X_Memcpy((uint8_t*)buff->point+buff->size,base,this_time);
        base += this_time;
        buff->size=info->max_payload_size;
        sendQuickPack(info);
        size -= this_time;
        this_time = info->max_payload_size - buff->size;
    }
    if(size)
    {
//        char* point = (char*)buff->point;
        GH3X_Memcpy((char*)buff->point+buff->size,base,size);
        buff->size+=size;
    }
    base = origin_base;
    if(base==NULL)
    {
        info->data.key_header.fin=1;
        sendQuickPack(info);
    }
    return 0;
}
#define UNSERCURE_LENGTH_KEYHEADER_INDEX_CRC 5
static int inner_publish(const char* key,const char* format,va_list args,uint8_t* type)
{
    CARIDFF_EXCEPTION_FUNCTION
    //packinfo
    GhQuickPackager packager = {0};
    packager.header = (PackagerHeader)quickContentPack;
    TRY_RUN(initialFrameData(&packager.info.data,(char*)key));
    packager.info.data.key_header.secure=0;
    packager.info.max_payload_size = GHRPC_FRAME_SIZE-sizeof(Frame_Header)-strlen(key)-UNSERCURE_LENGTH_KEYHEADER_INDEX_CRC;
    s_com_infos.lock();
    packager.info.send_buff = slabAlloc(NULL);
    s_com_infos.unlock();

    if(type)
    {
        quickContentPack(&packager.info,(uint8_t*)&u8_header,1);
        quickContentPack(&packager.info,type,1);
        quickContentPack(&packager.info,(uint8_t*)&u8_header,1);
        quickContentPack(&packager.info,&(s_frame_context.index.invoke_idx),1);
    }

    error_code = GHRPC_vpackage((PackageBase* )&packager,(char*)format,args);

    freeFrameData(&packager.info.data);

    if(type&&*type == SERCURE_INVOKE_RETURN)// only replace info of static
    {
        if(s_invoke_context.is_secure)
        {
            packager.info.send_buff->index=s_frame_context.index;
            s_invoke_context.node->info = packager.info.send_buff;
            return 0;
        }
    }

    CARIDFF_EXCEPTION_HANDLE;
    s_com_infos.lock();
    slabFree(packager.info.send_buff);
    s_com_infos.unlock();
    return error_code;
}

typedef struct
{
    int max_payload_size;
    FrameData data;
    LinkedBuff* header;
    LinkedBuff* tail;
    uint8_t* ret_buff;
} GhSecurePackInfo;

typedef struct
{
    PackagerHeader header_pack; //return need package length
    GhSecurePackInfo info;
} GhSecurePackager;


static void saveSecurePack(GhSecurePackInfo* info)
{
    if(info->tail==NULL)
    {
        info->header=slabAlloc(NULL);
        info->tail=info->header;
    }
    else
    {
        info->tail = slabAlloc(info->tail);
    }
    info->tail->length = toFrameData(&info->data,info->tail->buff);
    info->tail->index.frame_idx = info->data.frame_id++;
    info->tail->index.invoke_idx = info->data.com_id;
    info->data.param_data.size=0;
}

static int secureContentPack(GhSecurePackInfo* info, uint8_t* base, int size)
{
    RPCPoint* buff = &(info->data.param_data);
    int this_time = info->max_payload_size - buff->size;
    while(size >= this_time)
    {
        GH3X_Memcpy((char*)buff->point+buff->size,base,this_time);
        buff->size=info->max_payload_size;
        saveSecurePack(info);
        size -= this_time;
        this_time = info->max_payload_size - buff->size;

    }
    if(size)
    {
        GH3X_Memcpy((char*)buff->point+buff->size,base,size);
        buff->size+=size;
    }
    if(base==NULL)
    {
        info->data.key_header.fin=1;
        saveSecurePack(info);
    }
    return 0;
}


typedef struct
{
    uint8_t header;
    uint8_t type;
    uint8_t index_header;
    uint8_t com_id;
    uint8_t datas[FLEXIBLE_ARRAY];
} SecureReturn;

//send&scall-receive
void removeFrameBuffer(GhSecurePackInfo* info, int frame)
{
    s_com_infos.lock();

    if(frame == LAST_FRAME_FIX_INDEX)
    {
        LinkedBuff* this_buff =info->tail->front;
        slabFree(this_buff);
        info->tail=info->tail->front;
    }
    else
    {
        LinkedBuff* this_buff = info->header;
        while(this_buff)
        {
            if(this_buff->index.frame_idx==frame)
            {
                if(this_buff->front==NULL)
                    info->header = this_buff->next;

                if(this_buff->next==NULL)
                    info->tail = this_buff->front;

                slabFree(this_buff);
                break;
            }
            this_buff= this_buff->next;
        }
    }
    s_com_infos.unlock();
}

void clearSecurePackInfo(GhSecurePackInfo* info)
{
    removeDynamicNode(info->data.key_data.point);
}

static void secureCallback(SecureReturn* base,int size, GhSecurePackInfo* info)
{
    if( base->com_id != info->data.com_id)
    {
        return;
    }
    switch (base->type)
    {
    case SERCURE_INVOKE_RECIEVE_FRAME:
        removeFrameBuffer(info,*(base->datas+1));
        break;
    case SERCURE_INVOKE_RETURN:
        if(info->ret_buff)
        {
            GH3X_Memcpy(info->ret_buff,base->datas,size-2);
        }
    case SERCURE_INVOKE_NO_SUCH_FUNCTION:
    case SERCURE_INVOKE_ERROR:
    default:
        removeAllBufferList(info->header);
        info->header=NULL;
        info->tail =NULL;
        removeDynamicNode(info->data.key_data.point);
        break;
    }
}

static void clearSecurePack(GhSecurePackInfo* info)
{
    s_com_infos.lock();
    CHARMAP_remove(&s_dynamic_nodes,info->data.key_data.point);
    removeAllBufferList(info->header);
    info->header =NULL;
    info->tail =NULL;
    s_com_infos.unlock();
}

static int secureTypeSendProcess(GhSecurePackInfo* info)
{
    int retry_time = COMM_RETRY_TIME+1;
    while((--retry_time))
    {
        s_com_infos.lock();
        if(info->header == info->tail)
        {
            s_com_infos.unlock();
            break;
        }
        if((COMM_RETRY_TIME-retry_time)%COMM_RETRY_ROUND==0)
        {
            LinkedBuff* now =info->header;
            while(now!= info->tail)
            {
                s_com_infos.sendFunction(now->buff,now->length);
                now= now->next;
            }
        }
        s_com_infos.unlock();

        if(retry_time>1)
            s_com_infos.delay();
    }

    if(retry_time == 0)
    {
        clearSecurePack(info);
        return GHRPC_ERROR_SEND_FAIL;
    }

    retry_time = COMM_RETRY_TIME+1;

    while(--retry_time)
    {

            s_com_infos.lock();
            LinkedBuff* now = info->tail;
            if(now==NULL)
            {
                s_com_infos.unlock();
                break;
            }
            if(((COMM_RETRY_TIME-retry_time)%COMM_RETRY_ROUND)==0)
            {
                s_com_infos.sendFunction(now->buff,now->length);
            }
            s_com_infos.unlock();

        if(retry_time>1)
            s_com_infos.delay();
    }
    if(retry_time==0 )
    {
        clearSecurePack(info);
        return GHRPC_ERROR_SEND_FAIL;
    }
    clearSecurePack(info);
    return 0;
}

void initGhSecurePackager(GhSecurePackager* packager,char* key)
{
    packager->header_pack = (PackagerHeader)secureContentPack;
    initialFrameData(&packager->info.data,key);
    packager->info.data.key_header.secure=1;
    packager->info.data.com_id = ++s_send_index;
    if(!s_send_index)s_send_index++;
    packager->info.max_payload_size = GHRPC_FRAME_SIZE-sizeof(Frame_Header)-strlen(key)-UNSERCURE_LENGTH_KEYHEADER_INDEX_CRC-1;
}

static int inner_send(char* key,char* format,va_list args,uint8_t* type)
{
    CARIDFF_EXCEPTION_FUNCTION
    //package
    GhSecurePackager packager ={0};
    initGhSecurePackager(&packager,key);
    if(type)
    {
        secureContentPack(&packager.info,(uint8_t*)&u8_header,1);
        secureContentPack(&packager.info,type,1);
        secureContentPack(&packager.info,(uint8_t*)&u8_header,1);
        secureContentPack(&packager.info,&(s_frame_context.index.invoke_idx),1);
    }
    error_code = GHRPC_vpackage((PackageBase*)&packager,format,args);
    if(error_code)
    {
        freeFrameData(&packager.info.data);
        return error_code;
    }
    //insert function
    InvokeNode node={0};
    node.detail = NULL;
    node.func = (RpcFunction)secureCallback;
    node.info=&packager.info;
    node.key = key;
    error_code = insertDynamicNode(&node);
    if(error_code)
    {
        removeDynamicNode(node.key);
        freeFrameData(&packager.info.data);
        return error_code;
    }
    error_code = secureTypeSendProcess(node.info);
    removeDynamicNode(node.key);
    freeFrameData(&packager.info.data);
    return error_code;
}

static int replySecure(uint8_t type,char* key,char* format,...)
{
    CARIDFF_EXCEPTION_FUNCTION
    va_list args;
    va_start(args,format);
    error_code = inner_publish(key,format,args,&type);
    va_end(args);
    return error_code;
}

static void normalCallback(void* datas, int size, GhQuickPackInfo* info)
{
    if(info->ret_buff)
    {
        GH3X_Memcpy(info->ret_buff,datas,size);
    }
    removeDynamicNode(info->data.key_data.point);
}

static int waitSendComplete(char* key)
{
    int retry_time = COMM_RETRY_TIME+1;
    while(--retry_time)
    {
        s_com_infos.lock();
        if(CHARMAP_find(&s_dynamic_nodes,key)==NULL)
        {
            s_com_infos.unlock();
            break;
        }
        s_com_infos.unlock();
        if(retry_time>1)
        {
            s_com_infos.delay();
        }
    }
    if(retry_time==0)
    {
        return GHRPC_ERROR_SEND_STATUS;
    }
    return 0;
}



//===================rx group functions=============
static InvokeNode* s_keep_alive_array[PASS_MESSAGE_KEEP_TIME]={0};
void insertAndClearInfo(InvokeNode* node)
{
    if(node != gh_static_nodes[0])
    {
        node =NULL;
    }
    InvokeNode* remove_node = s_keep_alive_array[0];
    uint8_t clear_flag = (remove_node && node!=remove_node);

    for(int i =0;i<PASS_MESSAGE_KEEP_TIME-1;i++)
    {
        s_keep_alive_array[i]=s_keep_alive_array[i+1];
        if(node && s_keep_alive_array[i]==node)
            s_keep_alive_array[i]=NULL;
        if(clear_flag && s_keep_alive_array[i] == remove_node)
        {
            clear_flag = 0;
        }
        if(s_keep_alive_array[i] && s_keep_alive_array[i] != gh_static_nodes[0])
        {
            node =NULL;
        }
    }

    s_keep_alive_array[PASS_MESSAGE_KEEP_TIME-1]=node;

    if(clear_flag)
    {
        removeAllBufferList(remove_node->header);
        remove_node->header=NULL;
        if(remove_node->info)
        {
            slabFree(remove_node->info);
            remove_node->info=NULL;
        }
    }
}
//char type return
#define CHAR_RESET  4
#define CHAR_FAIL 3
#define CHAR_SUCESS 1
#define CHAR_NULL 0
#define CHAR_PASS 2

enum ProcessStatus
{
    IN_PROCESS_FRAMEHEADER,
    IN_PROCESS_CHECK_KEY,
    IN_PROCESS_CHECK_INDEX,
    IN_PROCESS_CHECK_PARAM,
    IN_PROCESS_CHECK_CRC,
    IN_PROCESS_ALL
};

typedef uint8_t HeaderState;
static char findFrameHeader(uint8_t byte,HeaderState* hd_index)
{
    switch (*hd_index)
    {
    case FRAME_NUMBER_INDEX:
        s_frame_context.frame_size=byte;
        return 1;
    default:
        if(*hd_index<FRAME_NUMBER_INDEX && byte == Frame_Header[*hd_index])
        {
            (*hd_index)++;
        }
        break;
    }
    return 0;
}

typedef struct 
{
    int flag_idx;
    uint8_t now_index;
} KeyState;

static char analysisKey(uint8_t byte,KeyState* state)
{
    switch (state->flag_idx)
    {
    case -1:
        state->flag_idx = byte;
        if(byte>MAX_SUPPORT_KEY_SIZE - 1)
        {
            return CHAR_FAIL;
        }
        break;
    case 0:
    {
        TypeKEY index_key = T2(TypeKEY,byte);
        if(index_key.pack_type != GH_PRO_TYPE_SIGNED)
        {
            return CHAR_FAIL;
        }
        state->flag_idx = index_key.is_array?-1:1;
        s_invoke_context.is_fin = index_key.fin;
        s_invoke_context.is_secure = index_key.secure;
    } 
    break;
    default:
        s_invoke_context.topic[state->now_index++] = byte;
        break;
    }

    if(state->flag_idx==state->now_index)
    {
        s_com_infos.lock();
        s_invoke_context.topic[state->flag_idx] = '\0';
        //RTT_LOG("recv: %s", s_invoke_context.topic);
        s_invoke_context.node = (InvokeNode*)CHARMAP_find(&s_dynamic_nodes,s_invoke_context.topic);
        if(s_invoke_context.node==NULL)
        {
            s_invoke_context.node = (InvokeNode*)CHARMAP_get(&s_static_nodes,s_invoke_context.topic);
        }
        s_com_infos.unlock();
        return 1;
    }
    return 0;
}

static int insertLinkedBuffer()
{
    s_com_infos.lock();
    if(s_invoke_context.node==NULL)
    {
        s_com_infos.unlock();
        return 0;
    }

    LinkedBuff* now = s_invoke_context.node->header;
    while(now && now->index.invoke_idx!=s_frame_context.index.invoke_idx)
    {
        now = now->next;
        slabFree(s_invoke_context.node->header);
        s_invoke_context.node->header = now;
    }

    if(s_invoke_context.node->header==NULL)
    {
        s_invoke_context.node->header = slabAlloc(NULL);
        s_invoke_context.node->header->index = s_frame_context.index;
        s_frame_context.now_buffer = s_invoke_context.node->header;
        s_com_infos.unlock();
        return 0;
    }
    else
    {
        now = s_invoke_context.node->header;
        while(now->next)
        {
            if(now->next->index.invoke_idx!=s_frame_context.index.invoke_idx)
            {
                slabFree(now->next);
            }
            now=now->next;
        }
    }
    now = s_invoke_context.node->header;
    //insert
    while(now)
    {
        if(now->index.frame_idx == s_frame_context.index.frame_idx)//allow keep failed invoke
        {
            s_frame_context.now_buffer = now;
            s_frame_context.now_buffer->length=0;
            s_com_infos.unlock();
            return 0;
        }
        if(now->index.frame_idx<s_frame_context.index.frame_idx &&
                (now->next == NULL||
                 now->next->index.frame_idx>s_frame_context.index.frame_idx))
        {
            break;
        }
        now= now->next;
    }

    s_frame_context.now_buffer = slabAlloc(now);
    s_frame_context.now_buffer->index=s_frame_context.index;
    s_frame_context.now_buffer->length=0;
    s_com_infos.unlock();
    return 0;
}

typedef uint8_t IndexState;

enum
{
    RECEIVE_UNFIN_AND_UNSECURE,
    RECEIVE_FIN_AND_UNSECURE,
    RECEIVE_UNFIN_AND_SECURE,
    RECEIVE_FIN_AND_SECURE
};

static char analysisIndex(uint8_t byte,IndexState* state)
{

    if(*state) //
    {
        s_frame_context.index.frame_idx = byte;
        insertLinkedBuffer();
        return CHAR_SUCESS;
    }
    //reset
    s_frame_context.index.invoke_idx=0;
    s_frame_context.index.frame_idx=0;
    int check = (s_invoke_context.is_secure<<1)+s_invoke_context.is_fin;
    char ret = 0;
    switch(check)
    {
    case RECEIVE_UNFIN_AND_SECURE:
        *state =1;
        s_frame_context.index.invoke_idx = byte;
        return 0;
    case RECEIVE_UNFIN_AND_UNSECURE:
        s_frame_context.index.frame_idx = byte;
        ret = CHAR_SUCESS;
        break;
    case RECEIVE_FIN_AND_UNSECURE:
        s_frame_context.index.frame_idx = LAST_FRAME_FIX_INDEX;
        ret = CHAR_PASS;
        break;
    case RECEIVE_FIN_AND_SECURE:
        s_frame_context.index.frame_idx = LAST_FRAME_FIX_INDEX;
        s_frame_context.index.invoke_idx = byte;
        ret = CHAR_SUCESS;
        break;
    default:
        ret = CHAR_FAIL;
        break;
    }
    insertLinkedBuffer();
    return ret;
}
typedef union 
{
    HeaderState head;
    KeyState key;
    IndexState index;
} ProcessState;

static struct 
{
    uint8_t inner_status;
    ProcessState state;
} s_process_state = {0};


static void clearAllState()
{
    memset(&s_frame_context,0,sizeof(s_frame_context));
    memset(&s_invoke_context,0,sizeof(s_invoke_context));
    memset(&s_process_state,0,sizeof(s_process_state));
}

typedef struct
{
    LinkedBuff* now_unpack;
    int unpack_index;
}GhUnpackInfo;

typedef struct
{
   UnpackBase base;
   GhUnpackInfo info;
} GhUnpack;

static uint8_t* getUnpackByte(GhUnpackInfo* info)
{
    info->unpack_index++;
    if(info->unpack_index>=info->now_unpack->length)
    {
        info->now_unpack = info->now_unpack->next;
        if(info->now_unpack==NULL)
            return NULL;
        info->unpack_index=0;
    }
    return info->now_unpack->buff+info->unpack_index;
}

static int getUnpackBytes(GhUnpackInfo* info,uint8_t* base,int size)
{
    int index = 0;
    info->unpack_index++;
    while(size)
    {
        int this_time_size = info->now_unpack->length -info->unpack_index;
        this_time_size =  size>this_time_size? this_time_size:size;
        GH3X_Memcpy(base+index,info->now_unpack->buff+info->unpack_index,this_time_size);
        index+=this_time_size;
        if(size != this_time_size)
        {
            info->unpack_index=0; //ready for next
            info->now_unpack = info->now_unpack->next;
            if(info->now_unpack==NULL)
                return index;
        }
        else
        {
            info->unpack_index+=size-1; //move to last
        }
        size -= this_time_size;
    }
    return index;
}

static int getAllBytes(InvokeNode* node,uint8_t* base)
{
    LinkedBuff* now =node->header;
    int index = 0;
    while(now)
    {
        GH3X_Memcpy(base+index,now->buff,now->length);
        index+=now->length;
        now=now->next;
    }
    return index;
}

//invoke context no lock
static int checkInvokeBuffer(LinkedBuff** head)
{
    int checked_frame = 0;
    LinkedBuff* now = *head;
    while(now)
    {
        if(now->index.invoke_idx!=s_frame_context.index.invoke_idx)
        {
            LinkedBuff* next = now->next;
            slabFree(now);
            *head=next;
            now = next;
            continue;
        }
        if(now->next && now->index.frame_idx!=checked_frame)
        {
            return GHRPC_ERROR_LOSE_FRAME;
        }
        now = now->next;
        checked_frame++;
    }
    return 0;
}

//invoke context no lock
#define SECURE_RETURN_FIX_HEADER_LEN (FRAME_NUMBER_INDEX+2)
static int invokeNode(InvokeNode** node)
{ 
    CARIDFF_EXCEPTION_FUNCTION;
    s_com_infos.lock();
    if(*node == NULL)
    {
        s_com_infos.unlock();
        return error_code;
    }
    //check&clear buffer
    checkInvokeBuffer(&((*node)->header));
    int size = 0;
    if((*node)->detail==NULL)//dynamic
    {
        size = getAllBytes((*node),slabBase());
    }
    else // static
    {
        if((*node)->info)
        {
            LinkedBuff* base = (LinkedBuff*)((*node)->info);
            if(base->index.invoke_idx == s_frame_context.index.invoke_idx)
            {
                s_com_infos.sendFunction(base->buff,base->length);
                s_com_infos.unlock();
                return error_code;
            }
            else
            {
                slabFree(base);
                (*node)->info=NULL;
            }
        }
        GhUnpack unpacker={0};
        unpacker.base.detail = (*node)->detail;
        unpacker.base.getByte = (uint8_t*(*)(void*))getUnpackByte;
        unpacker.base.getBytes = (int(*)(void*info,uint8_t* base,int size))getUnpackBytes;
        unpacker.info.now_unpack = (*node)->header;
        unpacker.info.unpack_index =-1;
        error_code = GHRPC_unpackage((Unpacker*)&unpacker,slabBase(),&size);
        if(error_code)
        {
            s_com_infos.unlock();
            CARIDFF_EXCEPTION_DEAL;
        }
    }
    s_com_infos.unlock();
    (*node)->func(slabBase(),size,(*node)->info);
    if(s_invoke_context.is_secure)
        GHRPC_return("");
    CARIDFF_EXCEPTION_HANDLE;
    //free invoke memory if non-static
    if((*node))
    {
        removeAllBufferList((*node)->header);
        (*node)->header=NULL;
    }
    return error_code;
}

//========================interfaces=======================


void GHRPC_init(GhRPCInitialInfo info)
{
    //initial function node
    s_dynamic_nodes.buff = dynamic_nodes_buffer;
    s_dynamic_nodes.max_size = DYNAMIC_NODE_SIZE;
    s_dynamic_nodes.now_size = 0;
    s_dynamic_nodes.unit_size = sizeof(InvokeNode);
    s_static_nodes.buff = (CharMapBase**)gh_static_nodes;
    s_static_nodes.max_size = sizeof(gh_static_nodes)/sizeof(InvokeNode*);
    s_static_nodes.now_size = s_static_nodes.max_size;
    s_static_nodes.unit_size = sizeof(InvokeNode);

    //initial slab
    initialSlab(GHRPC_FRAME_SIZE+sizeof(LinkedBuff));
    //initial comm
    s_com_infos = info;
    GHRPC_setMode(info.mode);
}

//only static node can use it, forbit dynamic node use
int GHRPC_return(const char* format, ...)
{
    CARIDFF_EXCEPTION_FUNCTION

    if(s_invoke_context.node==NULL)
    {
        return GHRPC_ERROR_NOT_UNDER_INVOKE;
    }
    uint8_t type = SERCURE_INVOKE_RETURN;

    va_list args;
    va_start(args,format);
    if(s_invoke_context.is_secure)
        error_code = inner_publish(s_invoke_context.topic,format,args,&type);
    else
        error_code = inner_publish(s_invoke_context.topic,format,args,NULL);
    va_end(args);
    s_invoke_context.is_secure=0;

    return error_code; 
}
 
void GHRPC_process(uint8_t* buff,int buff_size,uint8_t restart)
{
    if(restart)
    {
        clearAllState();
    }
    char analys_ret=0;
    int index = 0;
    while(index!=buff_size)
    {
        switch (s_process_state.inner_status)
        {
        case IN_PROCESS_FRAMEHEADER:
            analys_ret = findFrameHeader(buff[index],(HeaderState* )&s_process_state.state);
            break;
        case IN_PROCESS_CHECK_KEY:
            analys_ret = analysisKey(buff[index],(KeyState* )&s_process_state.state);
            s_frame_context.crc+=buff[index];
            s_frame_context.frame_size--;
            break;
        case IN_PROCESS_CHECK_INDEX:
            analys_ret = analysisIndex(buff[index],(IndexState* )&s_process_state.state);
            if(analys_ret==CHAR_PASS)
            {
                s_process_state.inner_status++;
                continue;
            }
            s_frame_context.crc+=buff[index];
            s_frame_context.frame_size--;
            break;
        case IN_PROCESS_CHECK_PARAM:
            if(s_frame_context.frame_size == 0)
            {
                s_process_state.inner_status++;
                continue;
            }
            s_frame_context.frame_size--;
            s_com_infos.lock();
            if(s_invoke_context.node)
            {
                s_frame_context.now_buffer->buff[s_frame_context.now_buffer->length++] = buff[index];
            }
            s_com_infos.unlock();
            analys_ret = (s_frame_context.frame_size==0);
            s_frame_context.crc+=buff[index];
            break;
        case IN_PROCESS_CHECK_CRC:
            if(buff[index]==s_frame_context.crc)
            {
                s_com_infos.lock();
                if(s_invoke_context.node)
                {
                    if(s_invoke_context.node->detail)
                        insertAndClearInfo(s_invoke_context.node);
                    s_com_infos.unlock();
                    if(s_invoke_context.is_fin)
                        invokeNode(&(s_invoke_context.node));
                }
                else
                {
                    insertAndClearInfo(NULL);
                    s_com_infos.unlock();
                }

                if(s_invoke_context.is_secure)
                {
                    if(s_invoke_context.is_fin && s_invoke_context.node==NULL)
                    {
                        replySecure(SERCURE_INVOKE_NO_SUCH_FUNCTION,s_invoke_context.topic,"");
                    }

                    if(s_invoke_context.is_fin==0)
                    {
                        replySecure(SERCURE_INVOKE_RECIEVE_FRAME,s_invoke_context.topic,"<u8>",s_frame_context.index.frame_idx);
                    }
                }
            }
            else
            {
                s_com_infos.lock();
                if(s_invoke_context.node)
                {
                    if(s_invoke_context.node->header==s_frame_context.now_buffer)
                    {
                        s_invoke_context.node->header=NULL;
                    }
                    slabFree(s_frame_context.now_buffer);
                }
                s_com_infos.unlock();
            }
            analys_ret = CHAR_RESET; //for reset status
            break;
        default:
            break;
        }
        switch (analys_ret)
        {
        case CHAR_PASS:
        case CHAR_SUCESS:
            s_process_state.inner_status = (s_process_state.inner_status+1)%IN_PROCESS_ALL;
            memset(&(s_process_state.state),0,sizeof(ProcessState));
            break;
        case CHAR_FAIL:
            s_process_state.inner_status = IN_PROCESS_FRAMEHEADER;
            clearAllState();
            break;
        case  CHAR_RESET:
            s_process_state.inner_status = IN_PROCESS_FRAMEHEADER;
            clearAllState();
            break;
        default:
            break;
        }
        index++;
    }
}

int GHRPC_publish(const char* key,const char* format,...)
{
    CARIDFF_EXCEPTION_FUNCTION
    va_list args;
    va_start(args,format);
    error_code = inner_publish(key,format,args,NULL);
    va_end(args);
    return error_code;
}


int GHRPC_call(void* ret_buff, const char* key,const char* format,...)
{
    CARIDFF_EXCEPTION_FUNCTION

    GhQuickPackager packager = {0};
    packager.header = (PackagerHeader)quickContentPack;
    initialFrameData(&packager.info.data,(char*)key);
    packager.info.data.key_header.secure=0;
    packager.info.ret_buff = ret_buff;
    packager.info.max_payload_size = GHRPC_FRAME_SIZE-sizeof(Frame_Header)-strlen(key)-UNSERCURE_LENGTH_KEYHEADER_INDEX_CRC;
    s_com_infos.lock();
    packager.info.send_buff = slabAlloc(NULL);
    s_com_infos.unlock();

    InvokeNode node={0};
    node.detail = NULL;
    node.func = (RpcFunction)normalCallback;
    node.key = (char*)key;
    node.info = &(packager.info);
    TRY_RUN(insertDynamicNode(&node));

    va_list args;
    va_start(args,format);
    error_code = GHRPC_vpackage((PackageBase* )&packager,(char*)format,args);
    va_end(args);

    CARIDFF_EXCEPTION_DEAL;

    TRY_RUN(waitSendComplete((char*)key));

    CARIDFF_EXCEPTION_HANDLE
    s_com_infos.lock();
    slabFree(packager.info.send_buff);
    s_com_infos.unlock();
    removeDynamicNode((char*)key);
    freeFrameData(&packager.info.data);
    return error_code;
}

int GHRPC_send(const char* key,const char* format,...)
{
    va_list args;
    va_start(args,format);
    int error_code = inner_send((char*)key,(char*)format,args,NULL);
    va_end(args);
    return error_code;
}


int GHRPC_sall(void* ret_buff, const char* key, const char* format,...)
{
    CARIDFF_EXCEPTION_FUNCTION
    //package
    GhSecurePackager packager ={0};
    packager.header_pack = (PackagerHeader)secureContentPack;
    initialFrameData(&packager.info.data,(char*)key);
    packager.info.data.key_header.secure=1;
    packager.info.data.com_id = s_send_index++;
    packager.info.max_payload_size = GHRPC_FRAME_SIZE-sizeof(Frame_Header)-strlen(key)-UNSERCURE_LENGTH_KEYHEADER_INDEX_CRC-1;
    packager.info.ret_buff = ret_buff;
    va_list args;
    va_start(args,format);
    InvokeNode node={0};
    error_code = GHRPC_vpackage((PackageBase*)&packager,(char*)format,args);
    va_end(args);

    CARIDFF_EXCEPTION_DEAL;

    //insert function
    node.detail = NULL;
    node.func = (RpcFunction)secureCallback;
    node.info=&packager.info;
    node.key = (char*)key;
    TRY_RUN(insertDynamicNode(&node));
    TRY_RUN(secureTypeSendProcess(node.info));
    TRY_RUN(waitSendComplete((char*)key));
    removeDynamicNode(node.key);

    CARIDFF_EXCEPTION_HANDLE
    freeFrameData(&packager.info.data);
    return error_code;
}

int GHRPC_unpack(void* ret_buff, const char* format,...)
{
    va_list args;
    va_start(args,format);
    int error_code = GHRPC_vunpack(ret_buff,(char*)format,args);
    va_end(args);
    return error_code;
}

