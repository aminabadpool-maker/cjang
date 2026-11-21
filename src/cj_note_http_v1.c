#include "cj_note_http_v1.h"
#include "cj_note_db_v1.h"
#include "cj_http_body_v1.h"
#include "cj_http_query_v1.h"
#include "cj_http_resp_v1.h"
#include "cj_http_status_v1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_note_uint(const char *req_buf, const char *key, unsigned int *out_val) {
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

static void parse_note_str(const char *req_buf, const char *key, char *dst, size_t dst_cap) {
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

size_t cj_note_http_create_resp(const char *req_buf, char *resp_buf, size_t resp_cap) {
    if (!resp_buf || resp_cap == 0) return 0;

    char title[CJ_NOTE_TITLE_MAX];
    char body_txt[CJ_NOTE_BODY_MAX];
    parse_note_str(req_buf, "title", title, sizeof(title));
    parse_note_str(req_buf, "body", body_txt, sizeof(body_txt));

    unsigned int new_id = 0;
    if (cj_note_db_create(title, body_txt, &new_id) != 0) {
        return cj_http_build_400(resp_buf, resp_cap);
    }

    char item_json[1536];
    if (cj_note_db_format_item_json(new_id, item_json, sizeof(item_json)) == 0) {
        return cj_http_build_text_resp(
            200,
            "OK",
            "application/json; charset=utf-8",
            "{\"ok\":true}",
            resp_buf,
            resp_cap
        );
    }

    char out[1664];
    int n = snprintf(out, sizeof(out), "{\"ok\":true,\"note\":%s}", item_json);
    if (n <= 0 || (size_t)n >= sizeof(out)) return 0;

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        out,
        resp_buf,
        resp_cap
    );
}

size_t cj_note_http_list_resp(char *resp_buf, size_t resp_cap) {
    if (!resp_buf || resp_cap == 0) return 0;

    char list_buf[16384];
    size_t pos = 0;
    list_buf[pos++] = '[';
    list_buf[pos] = '\0';

    size_t total = cj_note_db_count();
    size_t emitted = 0;
    unsigned int next_id = 1u;

    while (emitted < total && next_id < 100000u && pos + 8u < sizeof(list_buf)) {
        char item_json[1536];
        if (cj_note_db_format_item_json(next_id, item_json, sizeof(item_json)) > 0) {
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
        next_id++;
    }

    list_buf[pos++] = ']';
    list_buf[pos] = '\0';

    char out[17000];
    int n = snprintf(out, sizeof(out), "{\"count\":%zu,\"notes\":%s}", emitted, list_buf);
    if (n <= 0 || (size_t)n >= sizeof(out)) return 0;

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        out,
        resp_buf,
        resp_cap
    );
}

size_t cj_note_http_update_resp(const char *req_buf, char *resp_buf, size_t resp_cap) {
    if (!resp_buf || resp_cap == 0) return 0;

    unsigned int id = 0;
    if (parse_note_uint(req_buf, "id", &id) != 0) {
        return cj_http_build_400(resp_buf, resp_cap);
    }

    char title[CJ_NOTE_TITLE_MAX];
    char body_txt[CJ_NOTE_BODY_MAX];
    parse_note_str(req_buf, "title", title, sizeof(title));
    parse_note_str(req_buf, "body", body_txt, sizeof(body_txt));

    int rc = cj_note_db_update(id, title, body_txt);
    if (rc == -2) {
        return cj_http_build_400(resp_buf, resp_cap);
    }
    if (rc != 0) {
        return cj_http_build_text_resp(
            404,
            "Not Found",
            "application/json; charset=utf-8",
            "{\"ok\":false,\"error\":\"not_found\"}",
            resp_buf,
            resp_cap
        );
    }

    char item_json[1536];
    if (cj_note_db_format_item_json(id, item_json, sizeof(item_json)) == 0) {
        return cj_http_build_text_resp(
            200,
            "OK",
            "application/json; charset=utf-8",
            "{\"ok\":true}",
            resp_buf,
            resp_cap
        );
    }

    char out[1664];
    int n = snprintf(out, sizeof(out), "{\"ok\":true,\"note\":%s}", item_json);
    if (n <= 0 || (size_t)n >= sizeof(out)) return 0;

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        out,
        resp_buf,
        resp_cap
    );
}

size_t cj_note_http_delete_resp(const char *req_buf, char *resp_buf, size_t resp_cap) {
    if (!resp_buf || resp_cap == 0) return 0;

    unsigned int id = 0;
    if (parse_note_uint(req_buf, "id", &id) != 0) {
        return cj_http_build_400(resp_buf, resp_cap);
    }

    if (cj_note_db_delete(id) != 0) {
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
    int n = snprintf(out, sizeof(out), "{\"ok\":true,\"deleted_id\":%u,\"count\":%zu}", id, cj_note_db_count());
    if (n <= 0 || (size_t)n >= sizeof(out)) return 0;

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        out,
        resp_buf,
        resp_cap
    );
}
