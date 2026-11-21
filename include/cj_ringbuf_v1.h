#ifndef CJ_RINGBUF_V1_H
#define CJ_RINGBUF_V1_H

#include <stddef.h>
#include <stdint.h>

#ifndef CJ_RINGBUF_CAP
#define CJ_RINGBUF_CAP 65536u
#endif

typedef struct {
    uint8_t data[CJ_RINGBUF_CAP];
    size_t head;
    size_t tail;
    size_t len;
} cj_ringbuf_t;

void cj_rb_init(cj_ringbuf_t *rb);
void cj_rb_reset(cj_ringbuf_t *rb);
size_t cj_rb_cap(const cj_ringbuf_t *rb);
size_t cj_rb_len(const cj_ringbuf_t *rb);
size_t cj_rb_avail(const cj_ringbuf_t *rb);
int cj_rb_is_empty(const cj_ringbuf_t *rb);
int cj_rb_is_full(const cj_ringbuf_t *rb);

size_t cj_rb_write(cj_ringbuf_t *rb, const void *src, size_t n);
size_t cj_rb_read(cj_ringbuf_t *rb, void *dst, size_t n);
size_t cj_rb_peek(const cj_ringbuf_t *rb, void *dst, size_t n);
size_t cj_rb_discard(cj_ringbuf_t *rb, size_t n);

#endif
