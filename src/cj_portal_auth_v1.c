#include "cj_portal_auth_v1.h"
#include "cj_http_cookie_v1.h"
#include <string.h>

int cj_portal_has_cookie_val(
    const char *req_buf,
    const char *cookie_name,
    const char *expected_val
) {
    if (!req_buf || !cookie_name || cookie_name[0] == '\0' || !expected_val) {
        return 0;
    }

    char actual[256];
    if (cj_http_get_cookie(req_buf, cookie_name, actual, sizeof(actual)) != 0) {
        return 0;
    }

    return (strcmp(actual, expected_val) == 0) ? 1 : 0;
}
