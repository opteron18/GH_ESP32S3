#include "gh_gsr.h"
#include "gh3x_fifo_analyze.h"
#include "gh3x_drv.h"


#define GSR_AMP_TARGET_VAL      gsr_amp_taeget_val
#define GSR_AMP_UP_THRE      gsr_amp_up_thre
#define GSR_AMP_DOWN_THRE      gsr_amp_down_thre
#define GSR_ABS_UP_THRE      gsr_abs_thre

#define GSR_RAW_IQ_AMP_RATIO      (3.4)

int32_t gsr_amp_taeget_val = (16777216.0*0.72*0.9);
int32_t gsr_amp_up_thre = (16777216.0*0.72);
int32_t gsr_amp_down_thre = (16777216.0*0.72*0.9*0.5);
int32_t gsr_abs_thre = (16777216.0*0.72*0.5);
extern GS32 gnGsrPInfo[];

typedef enum
{
    GSR_OK = 0,             /**< no need to adjust current */
    GSR_AMP_OVER_THRE,      /**< need to adjust down */
    GSR_ABS_OVER_THRE,      /**< need to adjust down */
    GSR_AMP_BELOW_THRE,     /**< need to adjust up */
} gsr_adj_reason_e;

typedef struct
{
    gsr_adj_reason_e reason;     /**< register address */
    int8_t multiple;     /**< range -6 ~ 6 means 2^n*/
    uint8_t exist_rawdata;     /**< exist rawdata */
} gsr_adj_result_t;



typedef struct {
    int32_t* array;
    int32_t front;
    int32_t rear;
    int32_t size; // 添加一个成员来记录当前队列中的数据数量
    int32_t capacity;
} circular_queue_t;


typedef struct
{
    float ftx;     /**< range 0~5 */
    circular_queue_t rawdata_queue;
    int32_t window_up;
    int32_t window_down;
} gsr_adj_rawdata_info_t;

typedef struct
{
    float ftx;     /**< range 0~5 */
    circular_queue_t idata_queue;
    circular_queue_t qdata_queue;
    int32_t window_up;
    int32_t window_down;
} gsr_adj_iqdata_info_t;

#define RAWDATA_BUF_MAX_LEN           (8*8*4)
#define IQ_DATA_BUF_MAX_LEN           (8*4)
static int32_t g_rawdata_buf[RAWDATA_BUF_MAX_LEN + 1];
static int32_t g_idata_buf[IQ_DATA_BUF_MAX_LEN + 1];
static int32_t g_qdata_buf[IQ_DATA_BUF_MAX_LEN + 1];
static gsr_adj_rawdata_info_t g_rawdata_info;
static gsr_adj_iqdata_info_t g_iqdata_info;

void circular_queue_init(circular_queue_t* circular_queue, int32_t* array, int32_t len)
{
    circular_queue->array = array;
    circular_queue->capacity = len;
    circular_queue->front = 0;
    circular_queue->rear = 0;
    circular_queue->size = 0;
}

//static void* GH3X_Memset(void* pDest, int8_t chVal, uint32_t unByteSize)
//{
//    memset(pDest, chVal, unByteSize);
//}
//static int32_t GH3X_u24toint24(uint32_t unU24Val)
//{
//    uint32_t ret = unU24Val;
//    if (unU24Val & 1 << 23)
//    {
//        ret = unU24Val | (0xFF000000);
//    }
//    return (int32_t)ret;
//}
static uint8_t isFull(circular_queue_t* queue) {
    return (queue->rear + 1) % queue->capacity == queue->front;
}

//static uint8_t isEmpty(circular_queue_t* queue) {
//    return queue->front == queue->rear;
//}

//static void enqueue(circular_queue_t* queue, int item) {
//    if (isFull(queue)) {
//        //printf("Queue is full\n");
//        return;
//    }
//    queue->array[queue->rear] = item;
//    queue->rear = (queue->rear + 1) % queue->capacity;
//    queue->size++; // 入队后更新队列中的数据数量

//}

static void force_enqueue(circular_queue_t* queue, int item) {
    if (isFull(queue)) {
        //printf("Queue is full\n");
        queue->front = (queue->front + 1) % queue->capacity;
        queue->size--; // 出队后更新队列中的数据数量
    }
    queue->array[queue->rear] = item;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size++; // 入队后更新队列中的数据数量
}

//static int dequeue(circular_queue_t* queue) {
//    if (isEmpty(queue)) {
//        //printf("Queue is empty\n");
//        return -1;
//    }
//    int item = queue->array[queue->front];
//    queue->front = (queue->front + 1) % queue->capacity;
//    queue->size--; // 出队后更新队列中的数据数量

//    return item;
//}

static int peek(circular_queue_t* queue, int index) {
    if (index < 0 || index >= queue->size) {
        //printf("Index out of range\n");
        return -1;
    }
    int pos = (queue->front + index) % queue->capacity;
    return queue->array[pos];
}

void clear_queue(circular_queue_t* circular_queue)
{
    circular_queue->front = 0;
    circular_queue->rear = 0;
    circular_queue->size = 0;
}

static int gsr_cal_rawdata_amp(circular_queue_t* rawdata_queue, int32_t window_len)
{
    window_len = window_len / 8 * 8;
    int min = 8388607, max = -8388608, sum = 0, num = 0;
    if (rawdata_queue->size >= window_len)
    {
        int idx = rawdata_queue->size - window_len;
        while (idx < rawdata_queue->size)
        {
            int data = peek(rawdata_queue, idx);
            max = (data > max) ? data : max;
            min = (data < min) ? data : min;
            if ((idx - (rawdata_queue->size - window_len)) % 8 == 7)// calculate p-p value every 8 points
            {
                sum += (max - min);
                num++;
                if(sum < (max - min))
                {
                    return INT32_MAX / num;
                }
                min = 8388607;
                max = -8388608;
            }
            idx++;
        }
    }
    return (num > 0) ? (sum / num) : 0;
}


static int gsr_cal_rawdata_abs(circular_queue_t* rawdata_queue, int32_t window_len)
{
    window_len = window_len / 8 * 8;
    int num = 0, max_abs = 0, abs_ov_num = 0;
    if (rawdata_queue->size >= window_len)
    {
        int idx = rawdata_queue->size - window_len;
        while (idx < rawdata_queue->size)
        {
            int data = peek(rawdata_queue, idx);
            data = data > 0 ? data : (0 - data);
            max_abs = (data > max_abs) ? data : max_abs;
            if ((idx - (rawdata_queue->size - window_len)) % 8 == 7)
            {
                abs_ov_num = (max_abs > GSR_ABS_UP_THRE) ? abs_ov_num + 1 : abs_ov_num;
                num++;
                max_abs = 0;
            }
            idx++;
        }
    }
    return (num == abs_ov_num) && (num != 0);
}

static int8_t get_upgrade_multiple(int64_t current_amp, int64_t target_amp)
{
    //int32_t multiple = (int32_t)((float)target_amp / (float)current_amp + 0.5);
    int64_t multiple = target_amp / current_amp;
    int32_t power = 0;
    while (multiple / 2 != 0) {
        multiple /= 2;
        power++;
    }
    GSR_LOG("[%s], current_amp = %lld, target_amp = %lld, pow = %d\r\n", __FUNCTION__, current_amp, target_amp, power);
    return (int8_t)power;
}

static void gsr_rawdata_adj(gsr_adj_rawdata_info_t * rawdata_info, gsr_adj_result_t * rawdata_result) {
    rawdata_info->window_down = rawdata_info->window_down > rawdata_info->rawdata_queue.capacity ? rawdata_info->rawdata_queue.capacity : rawdata_info->window_down;
    rawdata_info->window_up = rawdata_info->window_up > rawdata_info->rawdata_queue.capacity ? rawdata_info->rawdata_queue.capacity : rawdata_info->window_up;

    //1. calculate peak peak value
    if (rawdata_info->rawdata_queue.size >= rawdata_info->window_down)
    {
        int amp = gsr_cal_rawdata_amp(&rawdata_info->rawdata_queue, rawdata_info->window_down);
        if (amp > GSR_AMP_UP_THRE) {
            rawdata_result->reason = GSR_AMP_OVER_THRE;
            rawdata_result->multiple = -1;
        }
    }
    if (rawdata_result->reason == GSR_OK && rawdata_info->rawdata_queue.size >= rawdata_info->window_up)
    {
        int amp = gsr_cal_rawdata_amp(&rawdata_info->rawdata_queue, rawdata_info->window_up);
        if (amp <= GSR_AMP_DOWN_THRE) {
            rawdata_result->reason = GSR_AMP_BELOW_THRE;
            rawdata_result->multiple = get_upgrade_multiple(amp, GSR_AMP_TARGET_VAL);
        }
    }
    //2. calculate abs
    if (rawdata_result->reason == GSR_OK && rawdata_info->rawdata_queue.size >= rawdata_info->window_down)
    {
        int abs_result = gsr_cal_rawdata_abs(&rawdata_info->rawdata_queue, rawdata_info->window_down);
        if (abs_result) {
            rawdata_result->reason = GSR_ABS_OVER_THRE;
            rawdata_result->multiple = -1;
        }
    }
}

static int64_t gsr_cal_iqdata_amp(circular_queue_t* idata_queue, circular_queue_t* qdata_queue, int32_t window_len)
{
    int num = 0;
    int64_t sum = 0;
    int32_t iqdata_size = idata_queue->size < qdata_queue->size ? idata_queue->size : qdata_queue->size;

    if (iqdata_size >= window_len)
    {
        int idx = iqdata_size - window_len;
        while (idx < iqdata_size)
        {
            int idata = peek(idata_queue, idx);
            int qdata = peek(qdata_queue, idx);
            int64_t square = (int64_t)idata * (int64_t)idata + (int64_t)qdata * (int64_t)qdata;
            sum += square;
            num++;
            if(sum < square)
            {
                return (9223372036854775807) / (int64_t)num;
            }
            idx++;
        }
    }
    return num > 0 ? sum / (int64_t)num : 0;
}

static void gsr_iqdata_adj(gsr_adj_iqdata_info_t * iqdata_info, gsr_adj_result_t * result) {
    //1. calculate peak peak value
    int32_t iqdata_size = iqdata_info->idata_queue.size < iqdata_info->qdata_queue.size ? iqdata_info->idata_queue.size : iqdata_info->qdata_queue.size;
    
    if (iqdata_size >= iqdata_info->window_down)
    {
        int64_t amp = gsr_cal_iqdata_amp(&(iqdata_info->idata_queue), &(iqdata_info->qdata_queue), iqdata_info->window_down);
//        GSR_LOG("iq amp1 = %lld \r\n",amp );
        if (amp * GSR_RAW_IQ_AMP_RATIO * GSR_RAW_IQ_AMP_RATIO > (int64_t)GSR_AMP_UP_THRE * (int64_t)GSR_AMP_UP_THRE) {
            result->reason = GSR_AMP_OVER_THRE;
            result->multiple = -1;
        }
    }
    if (result->reason == GSR_OK && iqdata_size >= iqdata_info->window_up)
    {
        int64_t amp = gsr_cal_iqdata_amp(&(iqdata_info->idata_queue), &(iqdata_info->qdata_queue), iqdata_info->window_up);
//        GSR_LOG("iq amp2 = %lld \r\n",amp );
        if (amp * GSR_RAW_IQ_AMP_RATIO * GSR_RAW_IQ_AMP_RATIO  <= (int64_t)GSR_AMP_DOWN_THRE * (int64_t)GSR_AMP_DOWN_THRE) {
            result->reason = GSR_AMP_BELOW_THRE;
            result->multiple = get_upgrade_multiple(amp * GSR_RAW_IQ_AMP_RATIO * GSR_RAW_IQ_AMP_RATIO , (int64_t)GSR_AMP_TARGET_VAL * (int64_t)GSR_AMP_TARGET_VAL);
        }
    }

}

static void gsr_adj_process(uint8_t * fifo, uint16_t size, gsr_adj_result_t * result)
{
    GH3X_Memset(result, 0, sizeof(gsr_adj_result_t));
    GSR_LOG("fifo size = %d \r\n",size );

    for (uint16_t i = 0; i < size; i += GH3X_FIFO_RAWDATA_SIZE)
    {
        uint16_t usTagTemp = GH3X_MAKEUP_WORD(fifo[i + 4], fifo[i + 3]);
        if (CHNLMAP_GET_DATATYPE(usTagTemp) == DATATYPE_GSR &&
            CHNLMAP_GET_GSRTYPE(usTagTemp) == GSR_DATATYPE_GSR)  // gsr rawdata
        {
            uint32_t unRawdata = GH3X_MAKEUP_DWORD(0, fifo[i + 2], fifo[i + 1], fifo[i + 0]);
            unRawdata = GH3X_u24toint24(unRawdata);
            result->exist_rawdata = (result->exist_rawdata == 1) ? 1 : (CHNLMAP_GET_GSRFLAG(usTagTemp) == 0);
            circular_queue_t* data_queues[] = {&(g_rawdata_info.rawdata_queue),&(g_iqdata_info.idata_queue),&(g_iqdata_info.qdata_queue)};
            force_enqueue(data_queues[CHNLMAP_GET_GSRFLAG(usTagTemp)], unRawdata);
        }
    }
    if (result->exist_rawdata == 1)
    {
        gsr_rawdata_adj(&g_rawdata_info, result);
        //GSR_LOG(" result: reason = %d, multiple = %d, exist_rawdata = %d\r\n", result->reason, result->multiple, result->exist_rawdata);
    }
    else 
    {
        gsr_iqdata_adj(&g_iqdata_info, result);
        //GSR_LOG(" result: reason = %d, multiple = %d, exist_rawdata = %d\r\n", result->reason, result->multiple, result->exist_rawdata);
    }
    GSR_LOG(" result: reason = %d, multiple = %d, exist_rawdata = %d\r\n", result->reason, result->multiple, result->exist_rawdata);
    if(result->reason != GSR_OK)
    {
        clear_queue(&(g_rawdata_info.rawdata_queue));
        clear_queue(&(g_iqdata_info.idata_queue));
        clear_queue(&(g_iqdata_info.qdata_queue));
    }
}
static uint8_t gsr_adj_init_flag = 0;
static uint8_t gsr_current_before_adj = 0;
uint8_t get_gsr_current(void)
{
    return gsr_current_before_adj;
}
uint8_t set_gsr_current(uint8_t gsr_current)
{
    gsr_current_before_adj = gsr_current;
    return 0;
}
void gsr_current_adj(uint8_t * fifo, uint16_t size)
{
    if(gsr_adj_init_flag == 0)
    {
        GSR_LOG(" gsr adj not init!!\r\n");
        return;
    }
    gsr_current_before_adj =  GH3X_ReadRegBitField(0x0206,8,10);
    int16_t gsr_current = gsr_current_before_adj;
    gsr_adj_result_t result;
    gsr_adj_process(fifo, size, &result);
#ifndef __cplusplus
 
    if(result.reason != GSR_OK)
    {
        GSR_LOG(" gsr_current1 = %d \r\n", gsr_current);
        if((gsr_current > 0 && result.multiple < 0) || (gsr_current < 6 && result.multiple > 0))
        {
            gsr_current += result.multiple;
            gsr_current = gsr_current > 6 ? 6 : gsr_current;
            gsr_current = gsr_current < 0 ? 0 : gsr_current;
            GH3X_WriteRegBitField(0x0200,0,0,0);
            GH3X_WriteRegBitField(0x0206,8,10,gsr_current);
            GH3X_WriteRegBitField(0x0008,15,15,0);
            GH3X_WriteRegBitField(0x0008,15,15,1);
            GH3X_WriteRegBitField(0x0200,0,0,1);
        }
        GSR_LOG(" gsr_current2 = %d \r\n", gsr_current);
    }
#endif
}

void gsr_current_adj_deinit(void)
{
    gsr_adj_init_flag = 0;
}
void gsr_current_adj_init(void)
{
    circular_queue_init(&(g_rawdata_info.rawdata_queue), g_rawdata_buf, RAWDATA_BUF_MAX_LEN);
    circular_queue_init(&(g_iqdata_info.idata_queue), g_idata_buf, IQ_DATA_BUF_MAX_LEN);
    circular_queue_init(&(g_iqdata_info.qdata_queue), g_qdata_buf, IQ_DATA_BUF_MAX_LEN);
#ifdef __cplusplus
    g_rawdata_info.window_up = 50;
    g_rawdata_info.window_down = 50;
    g_rawdata_info.ftx = 32;

    g_iqdata_info.window_up = 5;
    g_iqdata_info.window_down = 5;
    g_iqdata_info.ftx = 32;
#else
    gsr_adj_init_flag = ((gnGsrPInfo[1]) & 0xFF);
    GSR_LOG("gnGsrPInfo1 = %d \r\n",gnGsrPInfo[1]);

    if(gsr_adj_init_flag == 0)
    {
        return;
    }
    uint8_t adj_up_time = ((gnGsrPInfo[1] >> 8) & 0xFF);
    uint8_t adj_down_time = ((gnGsrPInfo[1] >> 16) & 0xFF);
    
    uint16_t gsr_tx_rate =  GH3X_ReadRegBitField(0x0696,0,2);
    g_rawdata_info.ftx = 125;
    g_iqdata_info.ftx = 125;
    for(int8_t i = 0; i < gsr_tx_rate; i++)
    {
        g_rawdata_info.ftx = g_rawdata_info.ftx / 2.0f;
        g_iqdata_info.ftx = g_iqdata_info.ftx / 2.0f;
    }
    g_rawdata_info.window_up = g_rawdata_info.ftx * 8.0f * adj_up_time + 0.5f;
    g_rawdata_info.window_down = g_rawdata_info.ftx * 8.0f * adj_down_time + 0.5f;

    uint16_t rg_iq_data_rate =  GH3X_ReadRegBitField(0x069a,0,2);
    GSR_LOG("rg_iq_data_rate = %d \r\n",rg_iq_data_rate);

    float iq_data_rate = g_iqdata_info.ftx * 8;
    for(int8_t i = 0; i < rg_iq_data_rate; i++)
    {
        iq_data_rate = iq_data_rate / 2.0f;
    }
    g_iqdata_info.window_up = (iq_data_rate * adj_up_time + 0.5f);
    g_iqdata_info.window_down = (iq_data_rate * adj_down_time + 0.5f);

    gsr_amp_taeget_val = gnGsrPInfo[2];
    gsr_amp_up_thre = gnGsrPInfo[3];
    gsr_amp_down_thre = gnGsrPInfo[4];
    gsr_abs_thre = gnGsrPInfo[5];

    GSR_LOG("gsr_current_adj_init raw = %f,%d,%d, iq = %f,%d,%d,\r\n", g_rawdata_info.ftx,g_rawdata_info.window_up,g_rawdata_info.window_down,
                                                 g_iqdata_info.ftx,g_iqdata_info.window_up,g_iqdata_info.window_down);

    GSR_LOG("gsr_amp = %d,%d,%d,%d\r\n",gsr_amp_taeget_val,gsr_amp_up_thre,gsr_amp_down_thre,gsr_abs_thre);
#endif
}

