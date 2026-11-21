#ifndef CJ_STATIC_V1_H
#define CJ_STATIC_V1_H

#include <stddef.h>

#ifndef CJ_STATIC_PATH_MAX
#define CJ_STATIC_PATH_MAX 512u
#endif

#ifndef CJ_STATIC_BODY_MAX
#define CJ_STATIC_BODY_MAX 65536u
#endif

int cj_static_is_safe_relpath(const char *relpath);
const char *cj_static_mime_type(const char *path);
int cj_static_resolve_path(const char *root_dir, const char *relpath, char *out_path, size_t out_cap);
int cj_static_read_file(const char *root_dir, const char *relpath, char *out_buf, size_t out_cap, size_t *out_len, const char **out_mime);

#endif
