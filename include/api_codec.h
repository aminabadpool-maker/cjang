#ifndef CJANG_API_CODEC_H
#define CJANG_API_CODEC_H

#include <stddef.h>

/* Decodes '+' to space and valid %XX hex sequences into fixed buffer dst.
 * Always NUL-terminates dst if cap > 0. Returns decoded byte length. */
size_t cjang_url_decode(const char *src, size_t src_len, char *dst, size_t dst_cap);

/* Installs a SIG_IGN handler for SIGPIPE so broken pipes return EPIPE instead of killing process. */
int cjang_ignore_sigpipe(void);

/* Sends all len bytes on fd without triggering SIGPIPE, then closes fd.
 * Returns 0 on full write, -1 on error. */
int cjang_send_all_nosig_and_close(int fd, const char *buf, size_t len);

#endif
