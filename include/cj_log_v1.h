#ifndef CJ_LOG_V1_H
#define CJ_LOG_V1_H

#include <stddef.h>

#ifndef CJ_LOG_LINE_MAX
#define CJ_LOG_LINE_MAX 512u
#endif

typedef enum {
    CJ_LOG_DEBUG = 0,
    CJ_LOG_INFO  = 1,
    CJ_LOG_WARN  = 2,
    CJ_LOG_ERROR = 3
} cj_log_level_t;

void cj_log_set_level(cj_log_level_t lvl);
cj_log_level_t cj_log_get_level(void);
const char *cj_log_level_name(cj_log_level_t lvl);
size_t cj_log_format(char *out, size_t cap, cj_log_level_t lvl, const char *tag, const char *msg);
void cj_log_write(cj_log_level_t lvl, const char *tag, const char *msg);

#endif
