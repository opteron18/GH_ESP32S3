#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t *buffer;
    uint16_t size;
    uint16_t head;
    uint16_t tail;
    uint16_t count;
} CircularBuffer;

bool circular_buffer_init(CircularBuffer *cb, uint16_t size);
void circular_buffer_free(CircularBuffer *cb);
bool circular_buffer_write(CircularBuffer *cb, const uint8_t *data, uint16_t len);
bool circular_buffer_read(CircularBuffer *cb, uint8_t *data, uint16_t len);
uint16_t circular_buffer_count(CircularBuffer *cb);
bool circular_buffer_is_empty(CircularBuffer *cb);
bool circular_buffer_is_full(CircularBuffer *cb);

#endif
