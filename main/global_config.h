//EMG-----------------------------------------------
// #define USE_ADS1292R 1
#define USE_EVT_ADS1192 1

//EMG REGS CONFIG-----------------------------------------------
// 0x01 = 0X51 0101 0001

extern uint8_t ads1192_default_config[11];
extern uint8_t ads1292r_default_config[11];
//EMG-----------------------------------------------

//UART-----------------------------------------------
#define USE_UART0 1
// #define USE_UART1 1
// #define USE_UART2 1
//UART-----------------------------------------------