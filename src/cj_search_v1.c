#include "cj_search_v1.h"
#include "cj_note_db_v1.h"
#include "cj_comment_db_v1.h"
#include "cj_http_body_v1.h"
#include "cj_http_query_v1.h"
#include "cj_http_resp_v1.h"
#include "cj_http_status_v1.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static int contains_ci_sub(const char *haystack, const char *needle) {
    if (!haystack || !needle || needle[0] == '\0') return 0;
    size_t hlen = strlen(haystack);
    size_t nlen = strlen(needle);
    if (nlen > hlen) return 0;

    for (size_t i = 0u; i + nlen <= hlen; i++) {
        size_t j = 0u;
        while (j < nlen) {
            unsigned char ch = (unsigned char)haystack[i + j];
            unsigned char cn = (unsigned char)needle[j];
            if (tolower(ch) != tolower(cn)) break;
            j++;
        }
        if (j == nlen) return 1;
    }
    return 0;
}

static void extract_search_query(const char *req_buf, char *dst, size_t cap) {
    if (!dst || cap == 0u) return;
    dst[0] = '\0';
    if (!req_buf) return;

    size_t blen = 0u;
    const char *body = cj_http_find_body(req_buf, &blen);
    if (cj_http_get_query_param(req_buf, "q", dst, cap) == 0 && dst[0] != '\0') {
        return;
    }
    if (body && blen > 0u) {
        (void)cj_http_get_form_field(body, "q", dst, cap);
    }
}

size_t cj_search_notes_json(const char *query, char *out_buf, size_t out_cap, size_t *out_count) {
    if (out_count) *out_count = 0u;
    if (!out_buf || out_cap < 3u) return 0u;
    out_buf[0] = '\0';

    size_t pos = 0u;
    out_buf[pos++] = '[';
    out_buf[pos] = '\0';

    size_t total = cj_note_db_count();
    size_t matched = 0u;
    unsigned int nid = 1u;
    size_t seen = 0u;

    while (seen < total && nid < 100000u && pos + 8u < out_cap) {
        cj_note_record_t rec;
        if (cj_note_db_get(nid, &rec) == 0) {
            seen++;
            if (contains_ci_sub(rec.title, query) || contains_ci_sub(rec.body, query)) {
                char item[1536];
                if (cj_note_db_format_item_json(rec.id, item, sizeof(item)) > 0u) {
                    size_t ilen = strlen(item);
                    size_t need = ilen + (matched > 0u ? 1u : 0u) + 4u;
                    if (pos + need < out_cap) {
                        if (matched > 0u) out_buf[pos++] = ',';
                        memcpy(out_buf + pos, item, ilen);
                        pos += ilen;
                        out_buf[pos] = '\0';
                        matched++;
                    }
                }
            }
        }
        nid++;
    }

    out_buf[pos++] = ']';
    out_buf[pos] = '\0';
    if (out_count) *out_count = matched;
    return pos;
}

size_t cj_search_comments_json(const char *query, char *out_buf, size_t out_cap, size_t *out_count) {
    if (out_count) *out_count = 0u;
    if (!out_buf || out_cap < 3u) return 0u;
    out_buf[0] = '\0';

    size_t pos = 0u;
    out_buf[pos++] = '[';
    out_buf[pos] = '\0';

    size_t total = cj_comment_db_count();
    size_t matched = 0u;
    unsigned int cid = 1u;
    size_t seen = 0u;

    while (seen < total && cid < 100000u && pos + 8u < out_cap) {
        cj_comment_record_t rec;
        if (cj_comment_db_get(cid, &rec) == 0) {
            seen++;
            if (contains_ci_sub(rec.text, query) || contains_ci_sub(rec.author, query)) {
                char item[1024];
                if (cj_comment_db_format_item_json(rec.id, item, sizeof(item)) > 0u) {
                    size_t ilen = strlen(item);
                    size_t need = ilen + (matched > 0u ? 1u : 0u) + 4u;
                    if (pos + need < out_cap) {
                        if (matched > 0u) out_buf[pos++] = ',';
                        memcpy(out_buf + pos, item, ilen);
                        pos += ilen;
                        out_buf[pos] = '\0';
                        matched++;
                    }
                }
            }
        }
        cid++;
    }

    out_buf[pos++] = ']';
    out_buf[pos] = '\0';
    if (out_count) *out_count = matched;
    return pos;
}

size_t cj_search_all_json(const char *query, char *out_buf, size_t out_cap) {
    if (!out_buf || out_cap == 0u) return 0u;
    out_buf[0] = '\0';

    char notes_arr[16384];
    char cmts_arr[16384];
    size_t n_cnt = 0u;
    size_t c_cnt = 0u;

    (void)cj_search_notes_json(query, notes_arr, sizeof(notes_arr), &n_cnt);
    (void)cj_search_comments_json(query, cmts_arr, sizeof(cmts_arr), &c_cnt);

    int n = snprintf(
        out_buf,
        out_cap,
        "{\"note_count\":%zu,\"comment_count\":%zu,\"notes\":%s,\"comments\":%s}",
        n_cnt,
        c_cnt,
        notes_arr,
        cmts_arr
    );
    if (n <= 0 || (size_t)n >= out_cap) {
        out_buf[0] = '\0';
        return 0u;
    }
    return (size_t)n;
}

size_t cj_search_http_resp(const char *req_buf, char *resp_buf, size_t resp_cap) {
    if (!resp_buf || resp_cap == 0u) return 0u;

    char q[256];
    extract_search_query(req_buf, q, sizeof(q));
    if (q[0] == '\0') {
        return cj_http_build_400(resp_buf, resp_cap);
    }

    char body[34000];
    if (cj_search_all_json(q, body, sizeof(body)) == 0u) {
        return cj_http_build_400(resp_buf, resp_cap);
    }

    return cj_http_build_text_resp(
        200,
        "OK",
        "application/json; charset=utf-8",
        body,
        resp_buf,
        resp_cap
    );
}
