//EMG-----------------------------------------------
// #define USE_ADS1292R 1
#define USE_ADS1292R_TWO_CORE 1
// #define USE_EVT_ADS1192 1
// #define USE_HOST_DEBUGER 1
//EMG REGS CONFIG-----------------------------------------------
extern uint8_t ads1192_default_config[11];
extern uint8_t ads1292r_default_config[11];
//EMG-----------------------------------------------

//UART-----------------------------------------------
// #define USE_UART0 1
// #define USE_UART1 1
#define USE_UART2 1
//UART-----------------------------------------------

//GH3300-----------------------------------------------
// #define USE_GH3300_LOG 1

// #define USE_GH3300_EVK 1
// #define USE_GH3300_EVT 1
#define USE_GH3300_TWO_ESP32S3 1
//GH3300-----------------------------------------------
//SD CARD-----------------------------------------------
#define CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4 1
#define CONFIG_EXAMPLE_SDMMC_SPEED_HS 1
#define CONFIG_EXAMPLE_PIN_CMD 7
#define CONFIG_EXAMPLE_PIN_CLK 8
#define CONFIG_EXAMPLE_PIN_D0 1
#define CONFIG_EXAMPLE_PIN_D1 2
#define CONFIG_EXAMPLE_PIN_D2 3
#define CONFIG_EXAMPLE_PIN_D3 4
//SD CARD-----------------------------------------------