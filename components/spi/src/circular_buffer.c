#include "circular_buffer.h"
#include <stdlib.h>
#include <string.h>

bool circular_buffer_init(CircularBuffer *cb, uint16_t size) {
    cb->buffer = malloc(size);
    if (!cb->buffer) return false;
    cb->size = size;
    cb->head = cb->tail = cb->count = 0;
    return true;
}

void circular_buffer_free(CircularBuffer *cb) {
    free(cb->buffer);
    cb->buffer = NULL;
    cb->size = cb->head = cb->tail = cb->count = 0;
}

bool circular_buffer_write(CircularBuffer *cb, const uint8_t *data, uint16_t len) {
    if (cb->count + len > cb->size) return false;

    for (uint16_t i = 0; i < len; ++i) {
        cb->buffer[cb->head] = data[i];
        cb->head = (cb->head + 1) % cb->size;
    }
    cb->count += len;
    return true;
}

bool circular_buffer_read(CircularBuffer *cb, uint8_t *data, uint16_t len) {
    if (cb->count < len) return false;

    for (uint16_t i = 0; i < len; ++i) {
        data[i] = cb->buffer[cb->tail];
        cb->tail = (cb->tail + 1) % cb->size;
    }
    cb->count -= len;
    return true;
}

uint16_t circular_buffer_count(CircularBuffer *cb) {
    return cb->count;
}

bool circular_buffer_is_empty(CircularBuffer *cb) {
    return cb->count == 0;
}

bool circular_buffer_is_full(CircularBuffer *cb) {
    return cb->count == cb->size;
}
