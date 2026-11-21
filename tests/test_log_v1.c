#include "cj_log_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    cj_log_set_level(CJ_LOG_INFO);
    assert(cj_log_get_level() == CJ_LOG_INFO);

    char line[128];
    size_t n = cj_log_format(line, sizeof(line), CJ_LOG_ERROR, "net", "socket closed");
    assert(n > 0);
    assert(strcmp(line, "[ERROR] net: socket closed\n") == 0);

    cj_log_write(CJ_LOG_INFO, "boot", "cj_log_v1 ready");
    printf("OK: cj_log_v1 passed\n");
    return 0;
}
