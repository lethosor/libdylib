#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libdylib.h"

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
char *safe_strsignal (int sig)
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
        const char *err = libdylib_last_error();                               \
        if (err) fprintf(stderr, "    last error: %s\n", err);                 \
        on_fail;                                                               \
    } else                                                                     \
        ++test_passed;                                                         \
    } while(0)

#define TEST(expr) TEST_AUX(expr, 0)

#define TEST_STRICT(expr) TEST_AUX(expr, {print_report(0); exit(1); })

int main(int argc, const char **argv)
{
    const char *lib_path = "testlib.dylib";
    install_handlers();
    TEST(!libdylib_last_error());

    DynamicLibrary *lib;
    TEST(lib = libdylib_open(lib_path));
    TEST(libdylib_close(lib));
    TEST(!libdylib_close(lib));
    TEST(lib = libdylib_open(lib_path));
    TEST(!libdylib_open("foo"));
    TEST(libdylib_last_error());

    TEST_STRICT(lib);

    TEST(libdylib_open_list(lib_path, "foo", 0));
    TEST(libdylib_open_list("foo", lib_path, 0));
    TEST(!libdylib_open_list("foo", "foo", "bar", "baz", "", 0));

    TEST(libdylib_lookup(lib, "sym1"));
    TEST(libdylib_lookup(lib, "sym2"));
    TEST(libdylib_lookup(lib, "sym3"));
    TEST(libdylib_find(lib, "sym1"));
    TEST(libdylib_find(lib, "sym2"));
    TEST(libdylib_find(lib, "sym3"));
    TEST(libdylib_find_any(lib, "sym1", "sym2", "sym3", 0));
    TEST(libdylib_find_all(lib, "sym1", "sym2", "sym3", 0));
    TEST(libdylib_find_any(lib, "x", "sym1", 0));
    TEST(libdylib_find_any(lib, "sym1", "x", 0));
    TEST(!libdylib_find_all(lib, "x", "sym1", 0));
    TEST(!libdylib_find_all(lib, "sym1", "x", 0));

    int (*returns_0)(void), (*returns_1)(void);
    TEST(LIBDYLIB_BIND(lib, "returns_0", returns_0));
    TEST(LIBDYLIB_BINDNAME(lib, returns_1));
    TEST(returns_0 && returns_1);
    TEST(returns_0() == 0);
    TEST(returns_1() == 1);

    TEST(libdylib_self());
    TEST(libdylib_find(libdylib_self(), "main"));

    print_report(1);
    return 0;
}
