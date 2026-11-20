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
    int rc_ok = cjang_bridge_cli_main(5, argv_ok);
    assert(rc_ok == 0);

    /* 2. Multi-prefix fallback selects matching candidate */
    char *argv_cand[] = {
        "cjang_bridge_cli",
        "--raw", "{\"target\":\"missing_stop\"}",
        "--key", "target",
        "--prefix", "none_",
        "--prefix", "bridge_"
    };
    int rc_cand = cjang_bridge_cli_main(9, argv_cand);
    assert(rc_cand == 0);

    /* 3. Missing required --key returns exit code 2 */
    char *argv_bad[] = {
        "cjang_bridge_cli",
        "--raw", "{\"target\":\"guard_\"}"
    };
    int rc_bad = cjang_bridge_cli_main(3, argv_bad);
    assert(rc_bad == 2);

    /* 4. Unmatched prefixes return exit code 3 */
    char *argv_fail[] = {
        "cjang_bridge_cli",
        "--raw", "{\"target\":\"missing_stop\"}",
        "--key", "target",
        "--prefix", "nope_"
    };
    int rc_fail = cjang_bridge_cli_main(7, argv_fail);
    assert(rc_fail == 3);

    printf("[OK] cjang_bridge_cli integration tests passed cleanly.\n");
    return 0;
}
