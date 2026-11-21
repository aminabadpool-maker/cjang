#include "cj_comment_http_v1.h"
#include "cj_comment_db_v1.h"
#include "cj_http_body_v1.h"
#include "cj_http_query_v1.h"
#include "cj_http_resp_v1.h"
#include "cj_http_status_v1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CJ_COMMENT_DB_DEFAULT_PATH
#define CJ_COMMENT_DB_DEFAULT_PATH "data/cj_comments.db"
#endif

static int parse_uint_field(const char *req_buf, const char *key, unsigned int *out_val) {
    if (!req_buf || !key || !out_val) return -1;
    char raw[32];
    raw[0] = '\0';

    size_t blen = 0;
    const char *body = cj_http_find_body(req_buf, &blen);
    int found = 0;

    if (body && blen > 0 && cj_http_get_form_field(body, key, raw, sizeof(raw)) == 0 && raw[0] != '\0') {
        found = 1;
    } else if (cj_http_get_query_param(req_buf, key, raw, sizeof(raw)) == 0 && raw[0] != '\0') {
        found = 1;
    }

    if (!found) return -1;
    for (size_t i = 0; raw[i] != '\0'; i++) {
        if (raw[i] < '0' || raw[i] > '9') return -1;
    }

    *out_val = (unsigned int)strtoul(raw, NULL, 10);
    return 0;
}

static void parse_str_field(const char *req_buf, const char *key, char *dst, size_t dst_cap) {
    if (!dst || dst_cap == 0) return;
    dst[0] = '\0';
    if (!req_buf || !key) return;

    size_t blen = 0;
    const char *body = cj_http_find_body(req_buf, &blen);
    if (body && blen > 0 && cj_http_get_form_field(body, key, dst, dst_cap) == 0 && dst[0] != '\0') {
        return;
    }
    (void)cj_http_get_query_param(req_buf, key, dst, dst_cap);
}

size_t cj_comment_http_add_resp(const char *req_buf, char *resp_buf, size_t resp_cap) {
    if (!resp_buf || resp_cap == 0) return 0;

    unsigned int note_id = 0;
    if (parse_uint_field(req_buf, "note_id", &note_id) != 0) {
        note_id = 1u;
    }

    char author[CJ_COMMENT_AUTHOR_MAX];
    char text[CJ_COMMENT_TEXT_MAX];
    parse_str_field(req_buf, "author", author, sizeof(author));
    parse_str_field(req_buf, "text", text, sizeof(text));

    unsigned int new_id = 0;
    if (cj_comment_db_add(note_id, author, text, &new_id) != 0) {
        return cj_http_build_400(resp_buf, resp_cap);
    }

    char item_json[1024];
    if (cj_comment_db_format_item_json(new_id, item_json, sizeof(item_json)) == 0) {
        return cj_http_build_text_resp(
            200,
            "OK",
            "application/json; charset=utf-8",
            "{\"ok\":true}",
            resp_buf,
            resp_cap
        );
    }

    char body[1152];
    int n = snprintf(body, sizeof(body), "{\"ok\":true,\"comment\":%s}", item_json);
    if (n <= 0 || (size_t)n >= sizeof(body)) return 0;

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        body,
        resp_buf,
        resp_cap
    );
}

size_t cj_comment_http_list_resp(const char *req_buf, char *resp_buf, size_t resp_cap) {
    if (!resp_buf || resp_cap == 0) return 0;

    unsigned int filter_note = 0;
    int has_filter = (parse_uint_field(req_buf, "note_id", &filter_note) == 0 && filter_note > 0);

    char list_buf[16384];
    size_t pos = 0;
    list_buf[pos++] = '[';
    list_buf[pos] = '\0';

    size_t total = cj_comment_db_count();
    size_t emitted = 0;
    unsigned int next_id = 1u;
    size_t found_recs = 0;

    while (found_recs < total && next_id < 100000u && pos + 8u < sizeof(list_buf)) {
        cj_comment_record_t rec;
        if (cj_comment_db_get(next_id, &rec) == 0) {
            found_recs++;
            if (!has_filter || rec.note_id == filter_note) {
                char item_json[1024];
                if (cj_comment_db_format_item_json(rec.id, item_json, sizeof(item_json)) > 0) {
                    size_t ilen = strlen(item_json);
                    size_t need = ilen + (emitted > 0 ? 1u : 0u) + 4u;
                    if (pos + need < sizeof(list_buf)) {
                        if (emitted > 0) list_buf[pos++] = ',';
                        memcpy(list_buf + pos, item_json, ilen);
                        pos += ilen;
                        list_buf[pos] = '\0';
                        emitted++;
                    }
                }
            }
        }
        next_id++;
    }

    list_buf[pos++] = ']';
    list_buf[pos] = '\0';

    char body[17000];
    int n = snprintf(body, sizeof(body), "{\"count\":%zu,\"comments\":%s}", emitted, list_buf);
    if (n <= 0 || (size_t)n >= sizeof(body)) return 0;

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        body,
        resp_buf,
        resp_cap
    );
}

size_t cj_comment_http_moderate_resp(const char *req_buf, char *resp_buf, size_t resp_cap) {
    if (!resp_buf || resp_cap == 0) return 0;

    unsigned int id = 0;
    unsigned int status_val = 0;
    if (parse_uint_field(req_buf, "id", &id) != 0 ||
        parse_uint_field(req_buf, "status", &status_val) != 0) {
        return cj_http_build_400(resp_buf, resp_cap);
    }

    if (cj_comment_db_set_status(id, (int)status_val) != 0) {
        return cj_http_build_text_resp(
            404,
            "Not Found",
            "application/json; charset=utf-8",
            "{\"ok\":false,\"error\":\"not_found_or_invalid_status\"}",
            resp_buf,
            resp_cap
        );
    }

    char body[160];
    int n = snprintf(body, sizeof(body), "{\"ok\":true,\"id\":%u,\"status\":%u}", id, status_val);
    if (n <= 0 || (size_t)n >= sizeof(body)) return 0;

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        body,
        resp_buf,
        resp_cap
    );
}

size_t cj_comment_http_delete_resp(const char *req_buf, char *resp_buf, size_t resp_cap) {
    if (!resp_buf || resp_cap == 0) return 0;

    unsigned int id = 0;
    if (parse_uint_field(req_buf, "id", &id) != 0) {
        return cj_http_build_400(resp_buf, resp_cap);
    }

    if (cj_comment_db_delete(id) != 0) {
        return cj_http_build_text_resp(
            404,
            "Not Found",
            "application/json; charset=utf-8",
            "{\"ok\":false,\"error\":\"not_found\"}",
            resp_buf,
            resp_cap
        );
    }

    char body[160];
    int n = snprintf(body, sizeof(body), "{\"ok\":true,\"deleted_id\":%u,\"count\":%zu}", id, cj_comment_db_count());
    if (n <= 0 || (size_t)n >= sizeof(body)) return 0;

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        body,
        resp_buf,
        resp_cap
    );
}
