#include "cj_log_v1.h"
#include <stdio.h>
#include <string.h>

static cj_log_level_t g_min_level = CJ_LOG_INFO;

void cj_log_set_level(cj_log_level_t lvl) {
    g_min_level = lvl;
}

cj_log_level_t cj_log_get_level(void) {
    return g_min_level;
}

const char *cj_log_level_name(cj_log_level_t lvl) {
    switch (lvl) {
        case CJ_LOG_DEBUG: return "DEBUG";
        case CJ_LOG_INFO:  return "INFO";
        case CJ_LOG_WARN:  return "WARN";
        case CJ_LOG_ERROR: return "ERROR";
        default:           return "UNKNOWN";
    }
}

size_t cj_log_format(char *out, size_t cap, cj_log_level_t lvl, const char *tag, const char *msg) {
    if (!out || cap == 0) return 0;
    const char *t = (tag && tag[0] != '\0') ? tag : "core";
    const char *m = msg ? msg : "";
    int n = snprintf(out, cap, "[%s] %s: %s\n", cj_log_level_name(lvl), t, m);
    if (n < 0) {
        out[0] = '\0';
        return 0;
    }
    if ((size_t)n >= cap) {
        out[cap - 1] = '\0';
        return cap - 1;
    }
    return (size_t)n;
}

void cj_log_write(cj_log_level_t lvl, const char *tag, const char *msg) {
    if (lvl < g_min_level) return;
    char buf[CJ_LOG_LINE_MAX];
    size_t len = cj_log_format(buf, sizeof(buf), lvl, tag, msg);
    if (len > 0) {
        FILE *fp = (lvl >= CJ_LOG_WARN) ? stderr : stdout;
        fwrite(buf, 1, len, fp);
        fflush(fp);
    }
}
