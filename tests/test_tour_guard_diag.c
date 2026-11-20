#include "cjang_guard.h"
#include "cjang_tour.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    const cjang_tour_stop_t stops[] = {
        {"routes", "Request Routing & Handlers", "src/api_tcp.c", "Route dispatch"},
        {"agent", "Agent Execution & KV Parser", "src/api_agent.c", "KV parsing"}
    };

    char idx_buf[512];
    assert(cjang_tour_format_index(stops, 2, idx_buf, sizeof(idx_buf)) == CJANG_TOUR_OK);
    assert(strstr(idx_buf, "routes") != NULL);
    assert(strstr(idx_buf, "agent") != NULL);

    char tiny_idx[10];
    assert(cjang_tour_format_index(stops, 2, tiny_idx, sizeof(tiny_idx)) == CJANG_TOUR_ERR_NOSPACE);
    assert(tiny_idx[0] == '\0');

    char diag_buf[160];
    assert(cjang_guard_format_diag("cmd", CJANG_GUARD_ERR_NOT_FOUND, diag_buf, sizeof(diag_buf)) == CJANG_GUARD_OK);
    assert(strcmp(diag_buf, "key=cmd status=-5 reason=key_not_found\n") == 0);

    char tiny_diag[8];
    assert(cjang_guard_format_diag("cmd", CJANG_GUARD_ERR_NOT_FOUND, tiny_diag, sizeof(tiny_diag)) == CJANG_GUARD_ERR_TOO_LONG);
    assert(tiny_diag[0] == '\0');

    printf("[OK] tour/guard diag helpers passed all checks.\n");
    return 0;
}
