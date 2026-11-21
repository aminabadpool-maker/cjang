#include "cj_ringbuf_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_ringbuf_t rb;
    cj_rb_init(&rb);
    assert(cj_rb_is_empty(&rb) == 1);

    const char *msg = "cjang-pure-c-stream";
    size_t w = cj_rb_write(&rb, msg, strlen(msg));
    assert(w == strlen(msg));
    assert(cj_rb_len(&rb) == strlen(msg));

    char peek_buf[32] = {0};
    size_t p = cj_rb_peek(&rb, peek_buf, 5);
    assert(p == 5);
    assert(strncmp(peek_buf, "cjang", 5) == 0);
    assert(cj_rb_len(&rb) == strlen(msg));

    char out_buf[64] = {0};
    size_t r = cj_rb_read(&rb, out_buf, sizeof(out_buf) - 1);
    assert(r == strlen(msg));
    assert(strcmp(out_buf, msg) == 0);
    assert(cj_rb_is_empty(&rb) == 1);

    printf("OK: cj_ringbuf_v1 passed all checks (cap=%zu)\n", cj_rb_cap(&rb));
    return 0;
}
