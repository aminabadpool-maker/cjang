#ifndef CJ_KV_V1_H
#define CJ_KV_V1_H

#include <stddef.h>

#ifndef CJ_KV_KEY_MAX
#define CJ_KV_KEY_MAX 64u
#endif

#ifndef CJ_KV_VAL_MAX
#define CJ_KV_VAL_MAX 256u
#endif

int cj_kv_parse_line(const char *line, char *key_out, size_t key_cap, char *val_out, size_t val_cap);
int cj_kv_get_from_buf(const char *buf, const char *key, char *val_out, size_t val_cap);

#endif
