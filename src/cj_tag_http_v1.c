#include "cj_tag_http_v1.h"
#include "cj_tag_db_v1.h"
#include "cj_http_body_v1.h"
#include "cj_http_query_v1.h"
#include "cj_http_resp_v1.h"
#include "cj_http_status_v1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_tag_uint(const char *req_buf, const char *key, unsigned int *out_val) {
    if (!req_buf || !key || !out_val) return -1;
    char raw[32];
    raw[0] = '\0';

    size_t blen = 0u;
    const char *body = cj_http_find_body(req_buf, &blen);
    int found = 0;

    if (body && blen > 0u && cj_http_get_form_field(body, key, raw, sizeof(raw)) == 0 && raw[0] != '\0') {
        found = 1;
    } else if (cj_http_get_query_param(req_buf, key, raw, sizeof(raw)) == 0 && raw[0] != '\0') {
        found = 1;
    }

    if (!found) return -1;
    for (size_t i = 0u; raw[i] != '\0'; i++) {
        if (raw[i] < '0' || raw[i] > '9') return -1;
    }

    *out_val = (unsigned int)strtoul(raw, NULL, 10);
    return 0;
}

static void parse_tag_str(const char *req_buf, const char *key, char *dst, size_t dst_cap) {
    if (!dst || dst_cap == 0u) return;
    dst[0] = '\0';
    if (!req_buf || !key) return;

    size_t blen = 0u;
    const char *body = cj_http_find_body(req_buf, &blen);
    if (body && blen > 0u && cj_http_get_form_field(body, key, dst, dst_cap) == 0 && dst[0] != '\0') {
        return;
    }
    (void)cj_http_get_query_param(req_buf, key, dst, dst_cap);
}

size_t cj_tag_http_add_resp(const char *req_buf, char *resp_buf, size_t resp_cap) {
    if (!resp_buf || resp_cap == 0u) return 0u;

    unsigned int note_id = 0u;
    if (parse_tag_uint(req_buf, "note_id", &note_id) != 0) {
        note_id = 1u;
    }

    char name[CJ_TAG_NAME_MAX];
    parse_tag_str(req_buf, "name", name, sizeof(name));

    unsigned int new_id = 0u;
    if (cj_tag_db_add(note_id, name, &new_id) != 0) {
        return cj_http_build_400(resp_buf, resp_cap);
    }

    char item_json[256];
    if (cj_tag_db_format_item_json(new_id, item_json, sizeof(item_json)) == 0u) {
        return cj_http_build_text_resp(
            200,
            "OK",
            "application/json; charset=utf-8",
            "{\"ok\":true}",
            resp_buf,
            resp_cap
        );
    }

    char out[384];
    int n = snprintf(out, sizeof(out), "{\"ok\":true,\"tag\":%s}", item_json);
    if (n <= 0 || (size_t)n >= sizeof(out)) return 0u;

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        out,
        resp_buf,
        resp_cap
    );
}

size_t cj_tag_http_list_resp(const char *req_buf, char *resp_buf, size_t resp_cap) {
    if (!resp_buf || resp_cap == 0u) return 0u;

    unsigned int filter_note = 0u;
    int has_filter = (parse_tag_uint(req_buf, "note_id", &filter_note) == 0 && filter_note > 0u);

    char arr[16384];
    size_t pos = 0u;
    arr[pos++] = '[';
    arr[pos] = '\0';

    size_t total = cj_tag_db_count();
    size_t emitted = 0u;
    unsigned int tid = 1u;
    size_t seen = 0u;

    while (seen < total && tid < 100000u && pos + 8u < sizeof(arr)) {
        cj_tag_record_t rec;
        if (cj_tag_db_get(tid, &rec) == 0) {
            seen++;
            if (!has_filter || rec.note_id == filter_note) {
                char item_json[256];
                if (cj_tag_db_format_item_json(rec.id, item_json, sizeof(item_json)) > 0u) {
                    size_t ilen = strlen(item_json);
                    size_t need = ilen + (emitted > 0u ? 1u : 0u) + 4u;
                    if (pos + need < sizeof(arr)) {
                        if (emitted > 0u) arr[pos++] = ',';
                        memcpy(arr + pos, item_json, ilen);
                        pos += ilen;
                        arr[pos] = '\0';
                        emitted++;
                    }
                }
            }
        }
        tid++;
    }

    arr[pos++] = ']';
    arr[pos] = '\0';

    char out[17000];
    int n = snprintf(out, sizeof(out), "{\"count\":%zu,\"tags\":%s}", emitted, arr);
    if (n <= 0 || (size_t)n >= sizeof(out)) return 0u;

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        out,
        resp_buf,
        resp_cap
    );
}

size_t cj_tag_http_delete_resp(const char *req_buf, char *resp_buf, size_t resp_cap) {
    if (!resp_buf || resp_cap == 0u) return 0u;

    unsigned int id = 0u;
    if (parse_tag_uint(req_buf, "id", &id) != 0) {
        return cj_http_build_400(resp_buf, resp_cap);
    }

    if (cj_tag_db_delete(id) != 0) {
        return cj_http_build_text_resp(
            404,
            "Not Found",
            "application/json; charset=utf-8",
            "{\"ok\":false,\"error\":\"not_found\"}",
            resp_buf,
            resp_cap
        );
    }

    char out[160];
    int n = snprintf(out, sizeof(out), "{\"ok\":true,\"deleted_id\":%u,\"count\":%zu}", id, cj_tag_db_count());
    if (n <= 0 || (size_t)n >= sizeof(out)) return 0u;

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        out,
        resp_buf,
        resp_cap
    );
}
