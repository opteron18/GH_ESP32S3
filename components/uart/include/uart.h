#ifndef _UART_H_
#define _UART_H_

void uart_init(void);
int sendData(const char* logName, const char* data);
void tx_task(void *arg);
void rx_task(void *arg);

#endif 