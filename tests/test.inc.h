#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int sig_max = 32;
int test_total = 0, test_passed = 0, test_failed = 0;
int last_line = 0;

void print_report(int finished)
{
    printf("%i tests: %i passed, %i failed [%.1f%% passed]%s\n",
        test_total, test_passed, test_failed, (float)test_passed / (float)test_total * 100.0,
        finished ? "" : " [INCOMPLETE]");
}

char **signal_names;
const char *safe_strsignal (int sig)
{
    if (sig >= 0 && sig <= sig_max && signal_names[sig])
        return signal_names[sig];
    return "Unknown signal";
}

void handler (int sig) {
    fprintf(stderr, "\n*** test at line %i crashed: %s\n", last_line, safe_strsignal(sig));
    ++test_failed;
    print_report(0);
    exit(1);
}

void install_handlers() {
    signal_names = (char**)calloc(sig_max, sizeof(const char*));
    int i;
    for (i = 0; i < sig_max; ++i)
    {
        signal_names[i] = strdup(strsignal(i));
        signal(i, handler);
    }
}

#define TEST_AUX(expr, on_fail) do {                                           \
    ++test_total;                                                              \
    last_line = __LINE__;                                                      \
    if (!(expr)) {                                                             \
        ++test_failed;                                                         \
        fprintf(stderr, "line %i: test failed (%s)\n", __LINE__, #expr);       \
        const char *err = LIBDYLIB_NAME(last_error)();                         \
        if (err) fprintf(stderr, "    last error: %s\n", err);                 \
        on_fail;                                                               \
    } else                                                                     \
        ++test_passed;                                                         \
    } while(0)

#define TEST(expr) TEST_AUX(expr, 0)

#define TEST_STRICT(expr) TEST_AUX(expr, {print_report(0); exit(1); })

void run_tests();
const char *lib_path = "testlib.dylib";

int main(int argc, const char **argv)
{
    install_handlers();
    printf("header version: %s (0x%08x)\n", LIBDYLIB_VERSION_STR, LIBDYLIB_VERSION);
    printf("library version: %s (0x%08x)\n", LIBDYLIB_NAME(get_version_str)(), LIBDYLIB_NAME(get_version)());
    run_tests();
    print_report(1);
    return 0;
}
