#include "cj_note_thread_v1.h"
#include "cj_note_db_v1.h"
#include "cj_comment_db_v1.h"
#include "cj_http_body_v1.h"
#include "cj_http_query_v1.h"
#include "cj_http_resp_v1.h"
#include "cj_http_status_v1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_thread_id(const char *req_buf, unsigned int *out_id) {
    if (!req_buf || !out_id) return -1;
    char raw[32];
    raw[0] = '\0';

    size_t blen = 0;
    const char *body = cj_http_find_body(req_buf, &blen);
    int found = 0;

    if (cj_http_get_query_param(req_buf, "id", raw, sizeof(raw)) == 0 && raw[0] != '\0') {
        found = 1;
    } else if (cj_http_get_query_param(req_buf, "note_id", raw, sizeof(raw)) == 0 && raw[0] != '\0') {
        found = 1;
    } else if (body && blen > 0 && cj_http_get_form_field(body, "id", raw, sizeof(raw)) == 0 && raw[0] != '\0') {
        found = 1;
    }

    if (!found) return -1;
    for (size_t i = 0; raw[i] != '\0'; i++) {
        if (raw[i] < '0' || raw[i] > '9') return -1;
    }

    *out_id = (unsigned int)strtoul(raw, NULL, 10);
    return (*out_id > 0) ? 0 : -1;
}

size_t cj_note_thread_count_comments(unsigned int note_id) {
    return cj_comment_db_count_by_note(note_id);
}

size_t cj_note_thread_format_json(unsigned int note_id, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0) return 0;
    out_buf[0] = '\0';

    cj_note_record_t note;
    if (cj_note_db_get(note_id, &note) != 0) return 0;

    char note_json[1536];
    if (cj_note_db_format_item_json(note_id, note_json, sizeof(note_json)) == 0) return 0;

    char arr_buf[16384];
    size_t pos = 0;
    arr_buf[pos++] = '[';
    arr_buf[pos] = '\0';

    size_t total_comments = cj_comment_db_count();
    size_t matched = 0;
    unsigned int cid = 1u;
    size_t seen = 0;

    while (seen < total_comments && cid < 100000u && pos + 8u < sizeof(arr_buf)) {
        cj_comment_record_t cmt;
        if (cj_comment_db_get(cid, &cmt) == 0) {
            seen++;
            if (cmt.note_id == note_id) {
                char cmt_json[1024];
                if (cj_comment_db_format_item_json(cmt.id, cmt_json, sizeof(cmt_json)) > 0) {
                    size_t clen = strlen(cmt_json);
                    size_t need = clen + (matched > 0 ? 1u : 0u) + 4u;
                    if (pos + need < sizeof(arr_buf)) {
                        if (matched > 0) arr_buf[pos++] = ',';
                        memcpy(arr_buf + pos, cmt_json, clen);
                        pos += clen;
                        arr_buf[pos] = '\0';
                        matched++;
                    }
                }
            }
        }
        cid++;
    }

    arr_buf[pos++] = ']';
    arr_buf[pos] = '\0';

    int n = snprintf(
        out_buf,
        out_cap,
        "{\"note\":%s,\"comment_count\":%zu,\"comments\":%s}",
        note_json,
        matched,
        arr_buf
    );
    if (n <= 0 || (size_t)n >= out_cap) {
        out_buf[0] = '\0';
        return 0;
    }
    return (size_t)n;
}

size_t cj_note_thread_http_resp(const char *req_buf, char *resp_buf, size_t resp_cap) {
    if (!resp_buf || resp_cap == 0) return 0;

    unsigned int id = 0;
    if (parse_thread_id(req_buf, &id) != 0) {
        return cj_http_build_400(resp_buf, resp_cap);
    }

    char thread_json[18500];
    if (cj_note_thread_format_json(id, thread_json, sizeof(thread_json)) == 0) {
        return cj_http_build_text_resp(
            404,
            "Not Found",
            "application/json; charset=utf-8",
            "{\"ok\":false,\"error\":\"note_not_found\"}",
            resp_buf,
            resp_cap
        );
    }

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        thread_json,
        resp_buf,
        resp_cap
    );
}
