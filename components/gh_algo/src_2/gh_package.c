/**
 * @file gh_package.c
 * @author wangjinxin (wangjinxin@domain.com)
 * @brief struct serialize & invoke function
 * @version 0.1
 * @date 2023-03-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "gh_package.h"
#include "gh3x_drv.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>

//===============static global var======================
static int s_mode = 0;

//==============implement strcture(represent protocol)===============

typedef struct{
    uint8_t pack_type : 2; // ProPackType
    uint8_t is_array: 1; // always 1  
    uint8_t width : 3; // unit lenth 2^x
    uint8_t end:1;//0-0-...-1
    uint8_t split:1; //split bit
} TypeArray;  // type array mark

typedef struct{
    TypeArray header;
    uint8_t length;
    uint8_t datas[FLEXIBLE_ARRAY];
} ArrayPack; 

typedef struct{
    uint8_t pack_type : 2; // ProPackType
    uint8_t is_array: 1; // always 0
    uint8_t width : 3; // unit lenth 2^x
    uint8_t end:1;//0-0-...-1
    uint8_t split:1; //split bit
} TypeData;  // single data mark 

typedef struct{
    TypeData header;
    uint8_t datas[FLEXIBLE_ARRAY];
} DataPack; 
//***************analysis ***************
enum
{
    FORMAT_INFO_NO_DATA = 0x0,
    FORMAT_INFO_DATA = 0x1,
    FORMAT_INFO_STRUCT = 0x2,
    FORMAT_INFO_MASK = 0x3,
    FORMAT_INFO_ARRAY = 0x1<<2,
};

typedef struct 
{
    uint8_t header[MAX_PARAM_NUMBER];
    uint8_t size;
    uint8_t type;
    uint8_t data_size;
    uint8_t array_num;
} FormatInfo;

static int getType(char type)
{
    switch (type){
    case 'u':return GH_PRO_TYPE_UNSIGNED;
    case 'f':return GH_PRO_TYPE_DOUBLE;
    case 'd':return GH_PRO_TYPE_SIGNED;
    default:break;
    }
    return -1;
}

static int gh_strtok(char* base, char split)
{
    int index = 0;
    while(*(base+index)!='\0')
    {
        if(*(base+index) == split )
        {
            *(base+index)='\0';
            return index+1;
        }
        index++;
    }
    return 0;
}

int analysisFormatString(char* format,FormatInfo* head_line)
{
    head_line->size=0;
    head_line->type =0;
    char analysis_buffer[MAX_SUPPORT_FORMAT_LENGTH];
    strcpy(analysis_buffer,format);
    char* token = analysis_buffer;
    head_line->data_size = 0;
    head_line->array_num =0;
    int next_offset = gh_strtok(analysis_buffer,'<');
    while(next_offset)
    {
        token+=next_offset;
        if(head_line->size>MAX_PARAM_NUMBER)
        {
            return GHRPC_ERROR_PARAM_TOO_MUCH;
        }
        next_offset =gh_strtok(token,'>');
        if(next_offset==0)
        {
            return GHRPC_ERROR_DETAIL_FORMAT;
        }
        int now_size = strlen(token);
        if(now_size<2)
        {
            return GHRPC_ERROR_DETAIL_FORMAT;
        }
        int type = getType(token[0]);
        *(head_line->header+head_line->size)=0;
        if(token[now_size-1]=='*') //array
        {
            head_line->type |= FORMAT_INFO_ARRAY;
            TypeArray* array=(TypeArray*)(head_line->header+head_line->size++);
            array->is_array=1;
            token[now_size-1]='\0';
            array->width = log2(atoi(token+1));
            array->pack_type = type;
            head_line->array_num++;
        }
        else
        {
            head_line->type |= head_line->data_size? FORMAT_INFO_STRUCT:FORMAT_INFO_DATA;
            TypeData* data_header = (TypeData*)(head_line->header+head_line->size++);
            data_header->is_array = 0;
            int this_size = atoi(token+1);
            data_header->width = log2(this_size);
            data_header->pack_type = type;
            head_line->data_size += this_size/8;
        }  
        token += next_offset;
        next_offset= gh_strtok(token,'<');
    }
    if(head_line->size > 0)
        ((TypeHeader*)(head_line->header+head_line->size-1))->end = 1;
    return 0;
}


//static uint8_t getNextSortHeader(FormatInfo* info,int* index)
//{
//    while((++*index) < info->size)
//    {
//        TypeHeader* header = (TypeHeader*)(info->header+*index);
//        if(header->is_array)
//        {
//            return 1;
//        }
//    }
//    while(--*index)
//    {
//        TypeHeader* header = (TypeHeader*)(info->header+*index);
//        if(header->is_array==0)
//        {
//            break;
//        }
//    }
//    return 0;
//}

uint8_t GHRPC_headType(uint8_t* head)
{
    TypeHeader* now_head = (TypeHeader*)head;
    if(now_head->pack_type==GH_PRO_TYPE_PACK)
    {
        return GHRPC_HEAD_ERROR;
    }
    if(now_head->is_array)
    {
        return GHRPC_HEAD_ARRAY;
    }
    return GHRPC_HEAD_DATA;
}

//***************package****************
static void transEndian(uint8_t* base, int unit_size,int size)
{
    int sub_size = unit_size/2;
    while(size--)
    {
        base += unit_size;
        for(int i=0;i<sub_size;i++)
        {
            *(base+i) += *(base+unit_size-i);
            *(base+unit_size-i) = *(base+i) - *(base+unit_size-i);
            *(base+i) = *(base+i) - *(base+unit_size-i);
        }
    }
}

//represent a independ package process
//todo: endian
static int packageData(PackageBase* tool,TypeData* header,va_list* args)
{
    /*CARIDFF_EXCEPTION_FUNCTION*/

    tool->move_to_buffer(tool->info,header,1);
    int width = (int)pow(2,header->width);
    switch(width)
    {
        case sizeof(uint64_t)*8:
        {
            uint64_t data = va_arg(*args,uint64_t);
            if(s_mode)
            {
                transEndian((uint8_t*)(&data),8,1);
            }
            return tool->move_to_buffer(tool->info,&data,sizeof(uint64_t));
        }
        case sizeof(uint32_t)*8:
        {
            uint32_t data = va_arg(*args,uint32_t);
            if(s_mode)
            {
                transEndian((uint8_t*)&data,4,1);
            }
            return tool->move_to_buffer(tool->info,&data,sizeof(uint32_t));
        }
        case sizeof(uint16_t)*8:
        {
            uint16_t data = va_arg(*args,uint32_t);
            if(s_mode)
            {
                transEndian((uint8_t*)&data,2,1);
            }
            return tool->move_to_buffer(tool->info,&data,sizeof(uint16_t));
        }
        case sizeof(uint8_t)*8:
        {
            uint8_t data = va_arg(*args,uint32_t);
            return tool->move_to_buffer(tool->info,&data,sizeof(uint8_t));
        }
        default:
            break;
    }
    return 0;
}

//todo: compress
static int packageArray(PackageBase* tool,TypeArray* header,va_list* args)
{
    int width =(int)pow(2,header->width);
    RPCPoint point  = va_arg(*args,RPCPoint);
    if(point.size==0)
        return 0;
    if(!width||width>128)
        return 0;
    if((width>=8&&width%8) || (width<8 && (8%width)))
        return 0;
    width /= 8;
    int size = point.size;
    uint8_t* now_point= point.point;
    while(size>255) //only one byte to repesent
    {
        header->split=1;
        tool->move_to_buffer(tool->info,header,1);
        uint8_t length = 255;
        tool->move_to_buffer(tool->info,&length,1);
        if(s_mode)
        {
            uint8_t trans_buff[16];
            for(int i=0;i<255;i++)
            {
                GH3X_Memcpy(trans_buff,now_point+i*width,width);
                transEndian(trans_buff,width,1);
                tool->move_to_buffer(tool->info,trans_buff,width);
            }
        }
        else
        {
            tool->move_to_buffer(tool->info,now_point,255*width);
        }
        now_point+=255;
        size -= 255;
    }
    header->split=0;
    tool->move_to_buffer(tool->info,header,1);
    uint8_t length = size;
    tool->move_to_buffer(tool->info,&length,1);
    if(s_mode)
    {
        uint8_t trans_buff[16];
        for(int i=0;i<255;i++)
        {
            GH3X_Memcpy(trans_buff,now_point+i*width,width);
            transEndian(trans_buff,width,1);
            tool->move_to_buffer(tool->info,trans_buff,width);
        }
    }
    else
    {
        tool->move_to_buffer(tool->info,now_point,size*width);
    }

    return 0;
}

//todo: stuct 
int GHRPC_vpackage(PackageBase* package_info,char* format,va_list args)
{
    CARIDFF_EXCEPTION_FUNCTION
    FormatInfo format_info;
    TRY_RUN(analysisFormatString(format,&format_info));

    for(int idx = 0;idx < format_info.size;idx++)
    {
        TypeHeader* typed_header = (TypeHeader*)&(format_info.header[idx]);
        if(typed_header->is_array==0)
        {
            TRY_RUN(packageData(package_info,(TypeData*)(format_info.header+idx),&args));
        }
        else
        {
            TRY_RUN(packageArray(package_info,(TypeArray*)(format_info.header+idx),&args));
        }
    }
    package_info->move_to_buffer(package_info->info,NULL,0);

    CARIDFF_EXCEPTION_HANDLE
    return error_code;
}

int GHRPC_package(PackageBase* package_info,char* format,...)
{
    va_list args;
    va_start(args,format);
    int ret=  GHRPC_vpackage(package_info,format,args);
    va_end(args);
    return ret;
}


//***************unpackage****************
static uint8_t compareWithoutFlag(uint8_t* a,uint8_t*b)
{
    HeaderBase mask = {0};
    mask.flag=1;
    return (*a|T2(uint8_t,mask)) == (*b|T2(uint8_t,mask));
}

static uint8_t receive_trans_buff[16];//recrive only support one thread
static int unpackData(Unpacker* datas,TypeData* head,void* base)
{
    if(s_mode)
    {
        int width = pow(2,head->width)/8;

        datas->base.getBytes(datas->info,receive_trans_buff,width);
        transEndian(receive_trans_buff,width,1);
        GH3X_Memcpy(base,receive_trans_buff,width);
        return width;
    }
    return datas->base.getBytes(datas->info,base,pow(2,head->width)/8);
}

static int unpackArray(Unpacker* datas,TypeArray* head, uint8_t* base,void* data_base)
{
    RPCPoint point;
    point.point = base;
    point.size = 0;
    int width = pow(2,head->width)/8;
    do{
        if(point.size)
        {
            TypeArray* now_head = (TypeArray*)(datas->base.getByte(datas->info));
            if(now_head==NULL || compareWithoutFlag((uint8_t*)head,(uint8_t*)now_head)==0)
            {
                return 0;
            }
            head = now_head;
        }
        int length = *datas->base.getByte(datas->info);
        point.size +=length;
        length *= width;
        if(s_mode)
        {
            while(length--)
            {
                datas->base.getBytes(datas->info,receive_trans_buff,width);
                transEndian(receive_trans_buff,width,1);
                GH3X_Memcpy(base,receive_trans_buff,width);
                base += width;
            }
        }
        else
        {
            if(datas->base.getBytes(datas->info,base,length)!=length)
            {
                return 0;
            }
            base += length;
        }
    }while(head->split);
    GH3X_Memcpy(data_base,&point,sizeof(RPCPoint));
    return point.size * width;
}

//todo:struct
int GHRPC_unpackage(Unpacker* datas, uint8_t* struct_pointer,int* totel_size)
{
    CARIDFF_EXCEPTION_FUNCTION
    FormatInfo format_info;
    int now_data_index = 0;
    int now_array_index = 0;
    int ret = 0;
    TRY_RUN(analysisFormatString(datas->base.detail,&format_info));
    now_array_index = format_info.data_size + sizeof(RPCPoint)*format_info.array_num;
    for(int check_index=0; check_index< format_info.size;check_index++)
    {
        uint8_t* last_base = datas->base.getByte(datas->info);
        if(compareWithoutFlag(last_base,format_info.header+check_index)==0)
        {
            GH_EXCEPTION_RETURN(GHRPC_ERROR_UNPACKAGE);
        }
        switch(GHRPC_headType(last_base))
        {
            case GHRPC_HEAD_DATA:
                ret = unpackData(datas,(TypeData*)last_base,struct_pointer+now_data_index);
                now_data_index += ret;
                break;
            case GHRPC_HEAD_ARRAY:
                ret = unpackArray(datas,(TypeArray*)last_base,struct_pointer+now_array_index,struct_pointer+now_data_index);
                now_array_index += ret;
                now_data_index+=sizeof (RPCPoint);
                break;
            default:
                GH_EXCEPTION_RETURN(GHRPC_ERROR_UNPACKAGE);
        }
        if(ret==0)
        {
            GH_EXCEPTION_RETURN(GHRPC_ERROR_UNPACKAGE);
        }
    }
    if(totel_size)
    {
        *totel_size = now_array_index;
    }

    CARIDFF_EXCEPTION_HANDLE
    return error_code;
}

int GHRPC_vunpack(uint8_t* struct_pointer, char* detail, va_list args)
{
    CARIDFF_EXCEPTION_FUNCTION
    FormatInfo format_info;
    int now_index=0;
    TypeHeader* this_header=NULL;
    TRY_RUN(analysisFormatString(detail,&format_info));
    for(int index = 0; index < format_info.size; index++)
    {
        TypeHeader* header = (TypeHeader*)(format_info.header+index);
        int width = pow(2,header->width)/8;
        if(header->is_array)
        {
            RPCPoint* point = va_arg(args,void*);
            uint8_t* this_base = point->point;
            point->size = 0;
            do{
                this_header = (TypeHeader*)(struct_pointer+now_index++);
                int this_size = *(struct_pointer+now_index++);
                GH3X_Memcpy(this_base,struct_pointer+now_index,this_size*width);
                this_base+= this_size*width;
                point->size+=this_size;

            }while (this_header->split);
        }
        else
        {
            this_header = (TypeHeader*)(struct_pointer+now_index++);
            GH3X_Memcpy(va_arg(args,void*),struct_pointer+now_index,width);
            now_index+=width;
        }
    }
    CARIDFF_EXCEPTION_HANDLE
    return error_code;
}

uint8_t GHRPC_setMode(int mode)
{
    s_mode = mode;
    return 0;
}
