//EMG-----------------------------------------------
#define USE_ADS1292R 0
#define USE_DVT_ADS1292R 1
#define USE_ADS1292R_TWO_CORE 0
#define USE_EVT_ADS1192 0
#define USE_HOST_DEBUGER 1
//EMG REGS CONFIG-----------------------------------------------
extern uint8_t ads1192_default_config[11];
extern uint8_t ads1292r_default_config[11];
//EMG-----------------------------------------------

//UART-----------------------------------------------
#define USE_UART0 1
#define USE_UART1 0
#define USE_UART2 0
//UART-----------------------------------------------

//GH3300-----------------------------------------------
#define USE_GH3300_LOG 1
#define USE_ALGO_LOG 0
#define USE_GH3300_EVK 1
#define USE_GH3300_EVT 0
#define USE_GH3300_TWO_ESP32S3 0
#define USE_GH3300_DVT 0
//GH3300-----------------------------------------------
//SD CARD-----------------------------------------------
//ESP32S3 B on DVT
#define CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4 1
#define CONFIG_EXAMPLE_SDMMC_SPEED_HS 1
#define CONFIG_EXAMPLE_PIN_CMD 16
#define CONFIG_EXAMPLE_PIN_CLK 17
#define CONFIG_EXAMPLE_PIN_D0 18
#define CONFIG_EXAMPLE_PIN_D1 21
#define CONFIG_EXAMPLE_PIN_D2 14
#define CONFIG_EXAMPLE_PIN_D3 15
//SD CARD-----------------------------------------------