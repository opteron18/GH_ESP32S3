#ifndef _GH3X_GSR_H_
#define _GH3X_GSR_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include <stdio.h>
 

#ifndef __cplusplus
// #include "slaver_log.h"

#define GSR_LOG(...)  //APP_LOG_INFO(__VA_ARGS__)
#include "gh3x_drv.h"
#include "gh3x_demo_algo_call_inner.h"

#else
#define GSR_LOG(...)  printf(__VA_ARGS__) 
#endif


void gsr_current_adj(uint8_t* fifo, uint16_t size);
void gsr_current_adj_init(void);
void gsr_current_adj_deinit(void);
uint8_t get_gsr_current(void);
uint8_t set_gsr_current(uint8_t gsr_current);

#ifdef __cplusplus
}
#endif
#endif
