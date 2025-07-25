#include <stdint.h>
#include "global_config.h"

#if (USE_EVT_ADS1192)
uint8_t ads1192_default_config[11] = {
    0x02,   //01h  config1 0000 0010 500sps
    0x80,   //02h  config2 1000 0000 set to default
    0x10,   //03h  lead-off 0001 0000 set to default 
    0x00,   //04h  channel1 setting 0000 0000 set to default 
    0x00,   //05h  channel2 setting 0000 0000 set to default 
    0x00,   //06h  Right Leg Driver 0000 00000 set to default
    0x00,   //07h  Lead-Off Sense Selection 0000 00000 set to default
    0x00,   //08h  Lead-Off Status 0000 0000 set to default
    0x02,   //09h  MiscellaneousControlRegister1 0000 0010 Bit6  Mustbesetto'1'
    0x00,   //0Ah  MiscellaneousControlRegister2 0000 0000 set to default
    0x0C    //0Bh  General-PurposeI/ORegister 0000 1100 set to default
};
#endif

#if (USE_ADS1292R)
uint8_t ads1292r_default_config[11] = {
    0x04, // CONFIG1: 250SPS (DR2=1 DR1=0 DR0=0)
    0x80, // CONFIG2: PDB_REFBUF = 1
    0x10, // LOFF: 仅保留COMP_EN = 1，其它为0（或根据需要）
    0x10, // CH1SET: GAIN=6, MUX=Normal
    0x10, // CH2SET: 同上
    0x00, // RLD_SENS: RLD相关配置，根据需要启用/禁用
    0x00, // LOFF_SENS: 默认
    0x00, // LOFF_STAT: 只读，写入无效
    0x00, // MISC1: 默认0
    0x02, // MISC2: CALIB_ON使能
    0x0C  // GPIO: 开启 GPIOD1 和 GPIOD2
};
#endif
//EMG-----------------------------------------------
