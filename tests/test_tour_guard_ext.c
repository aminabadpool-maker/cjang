#include "cjang_guard.h"
#include "cjang_tour.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char entry[256];
    assert(cjang_guard_format_entry("mode", "fast", CJANG_GUARD_OK, entry, sizeof(entry)) == CJANG_GUARD_OK);
    assert(strstr(entry, "key=mode") != NULL);
    assert(strstr(entry, "val=fast") != NULL);
    assert(strstr(entry, "val_len=4") != NULL);

    char small_entry[12];
    assert(cjang_guard_format_entry("mode", "very_long_value", CJANG_GUARD_ERR_TOO_LONG, small_entry, sizeof(small_entry)) == CJANG_GUARD_ERR_TOO_LONG);
    assert(small_entry[0] == '\0');

    const cjang_tour_stop_t stops[] = {
        {"api_tcp", "TCP Transport", "src/api_tcp.c", "Socket loop"},
        {"api_agent", "Agent Handler", "src/api_agent.c", "KV execution"},
        {"core_router", "Router", "src/router.c", "Path matching"}
    };

    char filtered[512];
    assert(cjang_tour_render_prefix(stops, 3, "api_", filtered, sizeof(filtered)) == CJANG_TOUR_OK);
    assert(strstr(filtered, "api_tcp") != NULL);
    assert(strstr(filtered, "api_agent") != NULL);
    assert(strstr(filtered, "core_router") == NULL);

    assert(cjang_tour_render_prefix(stops, 3, "missing_", filtered, sizeof(filtered)) == CJANG_TOUR_ERR_ARG);
    assert(filtered[0] == '\0');

    printf("[OK] extended tour/guard capabilities passed.\n");
    return 0;
}
