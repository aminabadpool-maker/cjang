#include "cj_ringbuf_v1.h"
#include <string.h>

void cj_rb_init(cj_ringbuf_t *rb) {
    if (!rb) return;
    memset(rb, 0, sizeof(*rb));
}

void cj_rb_reset(cj_ringbuf_t *rb) {
    if (!rb) return;
    rb->head = 0;
    rb->tail = 0;
    rb->len = 0;
}

size_t cj_rb_cap(const cj_ringbuf_t *rb) {
    (void)rb;
    return CJ_RINGBUF_CAP;
}

size_t cj_rb_len(const cj_ringbuf_t *rb) {
    return rb ? rb->len : 0;
}

size_t cj_rb_avail(const cj_ringbuf_t *rb) {
    if (!rb || rb->len >= CJ_RINGBUF_CAP) return 0;
    return CJ_RINGBUF_CAP - rb->len;
}

int cj_rb_is_empty(const cj_ringbuf_t *rb) {
    return (!rb || rb->len == 0) ? 1 : 0;
}

int cj_rb_is_full(const cj_ringbuf_t *rb) {
    return (rb && rb->len >= CJ_RINGBUF_CAP) ? 1 : 0;
}

size_t cj_rb_write(cj_ringbuf_t *rb, const void *src, size_t n) {
    if (!rb || !src || n == 0) return 0;
    size_t avail = cj_rb_avail(rb);
    size_t to_write = (n < avail) ? n : avail;
    const uint8_t *p = (const uint8_t *)src;
    size_t written = 0;

    while (written < to_write) {
        size_t contiguous = CJ_RINGBUF_CAP - rb->tail;
        size_t chunk = to_write - written;
        if (chunk > contiguous) chunk = contiguous;
        memcpy(&rb->data[rb->tail], p + written, chunk);
        rb->tail = (rb->tail + chunk) % CJ_RINGBUF_CAP;
        written += chunk;
    }

    rb->len += written;
    return written;
}

size_t cj_rb_read(cj_ringbuf_t *rb, void *dst, size_t n) {
    if (!rb || !dst || n == 0 || rb->len == 0) return 0;
    size_t to_read = (n < rb->len) ? n : rb->len;
    uint8_t *out = (uint8_t *)dst;
    size_t read_bytes = 0;

    while (read_bytes < to_read) {
        size_t contiguous = CJ_RINGBUF_CAP - rb->head;
        size_t chunk = to_read - read_bytes;
        if (chunk > contiguous) chunk = contiguous;
        memcpy(out + read_bytes, &rb->data[rb->head], chunk);
        rb->head = (rb->head + chunk) % CJ_RINGBUF_CAP;
        read_bytes += chunk;
    }

    rb->len -= read_bytes;
    if (rb->len == 0) {
        rb->head = 0;
        rb->tail = 0;
    }
    return read_bytes;
}

size_t cj_rb_peek(const cj_ringbuf_t *rb, void *dst, size_t n) {
    if (!rb || !dst || n == 0 || rb->len == 0) return 0;
    size_t to_peek = (n < rb->len) ? n : rb->len;
    uint8_t *out = (uint8_t *)dst;
    size_t pos = rb->head;
    size_t peeked = 0;

    while (peeked < to_peek) {
        size_t contiguous = CJ_RINGBUF_CAP - pos;
        size_t chunk = to_peek - peeked;
        if (chunk > contiguous) chunk = contiguous;
        memcpy(out + peeked, &rb->data[pos], chunk);
        pos = (pos + chunk) % CJ_RINGBUF_CAP;
        peeked += chunk;
    }

    return peeked;
}

size_t cj_rb_discard(cj_ringbuf_t *rb, size_t n) {
    if (!rb || n == 0 || rb->len == 0) return 0;
    size_t to_drop = (n < rb->len) ? n : rb->len;
    rb->head = (rb->head + to_drop) % CJ_RINGBUF_CAP;
    rb->len -= to_drop;
    if (rb->len == 0) {
        rb->head = 0;
        rb->tail = 0;
    }
    return to_drop;
}
