#include "cjang_bridge.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int cjang_bridge_cli_main(int argc, char **argv);

int main(void) {
    /* 1. Direct match from extracted key value */
    char *argv_ok[] = {
        "cjang_bridge_cli",
        "--raw", "{\"target\":\"guard_\"}",
        "--key", "target"
    };
    assert(cjang_bridge_cli_main(5, argv_ok) == 0);

    /* 2. Multi-prefix fallback selects matching candidate */
    char *argv_cand[] = {
        "cjang_bridge_cli",
        "--raw", "{\"target\":\"missing_stop\"}",
        "--key", "target",
        "--prefix", "none_",
        "--prefix", "bridge_"
    };
    assert(cjang_bridge_cli_main(9, argv_cand) == 0);

    /* 3. Missing required --key returns exit code 2 */
    char *argv_bad[] = {
        "cjang_bridge_cli",
        "--raw", "{\"target\":\"guard_\"}"
    };
    assert(cjang_bridge_cli_main(3, argv_bad) == 2);

    /* 4. Unmatched prefixes return exit code 3 */
    char *argv_fail[] = {
        "cjang_bridge_cli",
        "--raw", "{\"target\":\"missing_stop\"}",
        "--key", "target",
        "--prefix", "nope_"
    };
    assert(cjang_bridge_cli_main(7, argv_fail) == 3);

    /* 5. Read payload from stdin via --raw - using a temporary stream */
    const char *sample_stdin = "{\"target\":\"tour_\"}";
    FILE *mem_in = tmpfile();
    assert(mem_in != NULL);
    size_t written = fwrite(sample_stdin, 1u, strlen(sample_stdin), mem_in);
    assert(written == strlen(sample_stdin));
    rewind(mem_in);

    FILE *orig_stdin = stdin;
    stdin = mem_in;

    char *argv_stdin[] = {
        "cjang_bridge_cli",
        "--raw", "-",
        "--key", "target"
    };
    int rc_stdin = cjang_bridge_cli_main(5, argv_stdin);

    stdin = orig_stdin;
    fclose(mem_in);
    assert(rc_stdin == 0);

    /* 6. Quiet mode (-q) succeeds and suppresses [DIAG] output */
    char *argv_quiet[] = {
        "cjang_bridge_cli",
        "--raw", "{\"target\":\"bridge_\"}",
        "--key", "target",
        "-q"
    };
    assert(cjang_bridge_cli_main(6, argv_quiet) == 0);

    /* 7. Count-only mode (-q -c) outputs only the matched count */
    char *argv_count[] = {
        "cjang_bridge_cli",
        "--raw", "{\"target\":\"guard_\"}",
        "--key", "target",
        "-q",
        "-c"
    };
    assert(cjang_bridge_cli_main(7, argv_count) == 0);

    /* 8. First-only mode (-q -f) returns top stop */
    char *argv_first[] = {
        "cjang_bridge_cli",
        "--raw", "{\"target\":\"guard_\"}",
        "--key", "target",
        "-q",
        "-f"
    };
    assert(cjang_bridge_cli_main(7, argv_first) == 0);

    /* 9. KV format (-q -o kv -f) emits clean key-value line */
    char *argv_kv[] = {
        "cjang_bridge_cli",
        "--raw", "{\"target\":\"tour_\"}",
        "--key", "target",
        "-q",
        "-o", "kv",
        "-f"
    };
    assert(cjang_bridge_cli_main(9, argv_kv) == 0);

    /* 10. Invalid format returns exit code 2 */
    char *argv_bad_fmt[] = {
        "cjang_bridge_cli",
        "--raw", "{\"target\":\"guard_\"}",
        "--key", "target",
        "-o", "xml"
    };
    assert(cjang_bridge_cli_main(7, argv_bad_fmt) == 2);

    /* 11. Strict mode (-s) rejects oversized stdin exceeding static buffer */
    FILE *big_in = tmpfile();
    assert(big_in != NULL);
    const char *prefix_part = "{\"target\":\"guard_\",\"pad\":\"";
    assert(fwrite(prefix_part, 1u, strlen(prefix_part), big_in) == strlen(prefix_part));
    for (size_t i = 0u; i < 2100u; i++) {
        assert(fputc('x', big_in) == 'x');
    }
    const char *suffix_part = "\"}";
    assert(fwrite(suffix_part, 1u, strlen(suffix_part), big_in) == strlen(suffix_part));
    rewind(big_in);

    stdin = big_in;
    char *argv_strict[] = {
        "cjang_bridge_cli",
        "--raw", "-",
        "--key", "target",
        "-s"
    };
    int rc_strict = cjang_bridge_cli_main(6, argv_strict);
    stdin = orig_stdin;
    fclose(big_in);
    assert(rc_strict == 2);

    printf("[OK] cjang_bridge_cli integration tests passed cleanly.\n");
    return 0;
}
