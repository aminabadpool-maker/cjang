#include "cj_static_v1.h"
#include <stdio.h>
#include <string.h>

int cj_static_is_safe_relpath(const char *relpath) {
    if (!relpath || relpath[0] == '\0' || relpath[0] == '/') return 0;
    if (strstr(relpath, "..") != NULL) return 0;
    if (strchr(relpath, '\\') != NULL) return 0;
    return 1;
}

const char *cj_static_mime_type(const char *path) {
    if (!path) return "application/octet-stream";
    size_t len = strlen(path);
    if (len >= 5 && strcmp(path + len - 5, ".html") == 0) {
        return "text/html; charset=utf-8";
    }
    if (len >= 4 && strcmp(path + len - 4, ".css") == 0) {
        return "text/css; charset=utf-8";
    }
    if (len >= 3 && strcmp(path + len - 3, ".js") == 0) {
        return "application/javascript; charset=utf-8";
    }
    if (len >= 5 && strcmp(path + len - 5, ".json") == 0) {
        return "application/json; charset=utf-8";
    }
    return "application/octet-stream";
}

int cj_static_resolve_path(const char *root_dir, const char *relpath, char *out_path, size_t out_cap) {
    if (!root_dir || root_dir[0] == '\0' || !out_path || out_cap == 0) return -1;
    const char *target = (relpath && relpath[0] != '\0') ? relpath : "index.html";
    if (!cj_static_is_safe_relpath(target)) return -1;

    size_t rlen = strlen(root_dir);
    int need_slash = (root_dir[rlen - 1] == '/') ? 0 : 1;
    int n = need_slash
        ? snprintf(out_path, out_cap, "%s/%s", root_dir, target)
        : snprintf(out_path, out_cap, "%s%s", root_dir, target);

    if (n < 0 || (size_t)n >= out_cap) return -1;
    return 0;
}

int cj_static_read_file(const char *root_dir, const char *relpath, char *out_buf, size_t out_cap, size_t *out_len, const char **out_mime) {
    if (!out_buf || out_cap == 0) return -1;
    char full_path[CJ_STATIC_PATH_MAX];
    if (cj_static_resolve_path(root_dir, relpath, full_path, sizeof(full_path)) != 0) {
        return -1;
    }

    FILE *fp = fopen(full_path, "rb");
    if (!fp) return -1;

    size_t total = 0;
    while (total + 1 < out_cap) {
        size_t n = fread(out_buf + total, 1, (out_cap - 1) - total, fp);
        total += n;
        if (n == 0) break;
    }
    int extra = fgetc(fp);
    fclose(fp);

    if (extra != EOF) return -1;

    out_buf[total] = '\0';
    if (out_len) *out_len = total;
    if (out_mime) *out_mime = cj_static_mime_type(full_path);
    return 0;
}
