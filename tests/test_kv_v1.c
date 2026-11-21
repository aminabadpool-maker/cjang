#include "cj_kv_v1.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char k[CJ_KV_KEY_MAX];
    char v[CJ_KV_VAL_MAX];

    assert(cj_kv_parse_line("# comment", k, sizeof(k), v, sizeof(v)) == 1);
    assert(cj_kv_parse_line("   ", k, sizeof(k), v, sizeof(v)) == 1);
    assert(cj_kv_parse_line("host = 127.0.0.1\r\n", k, sizeof(k), v, sizeof(v)) == 0);
    assert(strcmp(k, "host") == 0);
    assert(strcmp(v, "127.0.0.1") == 0);

    const char *cfg =
        "# cjang config\n"
        "port = 8080\n"
        "doc_root = public\n";

    char port[32];
    char root[64];
    assert(cj_kv_get_from_buf(cfg, "port", port, sizeof(port)) == 0);
    assert(strcmp(port, "8080") == 0);
    assert(cj_kv_get_from_buf(cfg, "doc_root", root, sizeof(root)) == 0);
    assert(strcmp(root, "public") == 0);
    assert(cj_kv_get_from_buf(cfg, "missing", port, sizeof(port)) != 0);

    printf("OK: cj_kv_v1 passed\n");
    return 0;
}
