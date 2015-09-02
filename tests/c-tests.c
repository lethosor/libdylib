#include "libdylib.h"
#include "test.inc.h"

void run_tests()
{
    TEST(!libdylib_last_error());

    dylib_ref lib;
    TEST(lib = libdylib_open(lib_path));
    TEST(libdylib_close(lib));
    TEST(!libdylib_close(lib));
    TEST(lib = libdylib_open(lib_path));
    TEST(!libdylib_open("foo"));
    TEST(libdylib_last_error());

    TEST_STRICT(lib);

    TEST(libdylib_open_list(lib_path, "foo", NULL));
    TEST(libdylib_open_list("foo", lib_path, NULL));
    TEST(!libdylib_open_list("foo", "foo", "bar", "baz", "", NULL));

    TEST(libdylib_lookup(lib, "sym1"));
    TEST(libdylib_lookup(lib, "sym2"));
    TEST(libdylib_lookup(lib, "sym3"));
    TEST(libdylib_find(lib, "sym1"));
    TEST(libdylib_find(lib, "sym2"));
    TEST(libdylib_find(lib, "sym3"));

    TEST(libdylib_find_any(lib, "sym1", "sym2", "sym3", NULL));
    TEST(libdylib_find_all(lib, "sym1", "sym2", "sym3", NULL));
    TEST(libdylib_find_any(lib, "x", "sym1", NULL));
    TEST(libdylib_find_any(lib, "sym1", "x", NULL));
    TEST(!libdylib_find_all(lib, "x", "sym1", NULL));
    TEST(!libdylib_find_all(lib, "sym1", "x", NULL));

    int (*returns_0)(void), (*returns_1)(void);
    TEST(libdylib_bind(lib, "returns_0", (void**)&returns_0));
    TEST(LIBDYLIB_BIND(lib, "returns_0", returns_0));
    TEST(LIBDYLIB_BINDNAME(lib, returns_1));
    TEST(returns_0 && returns_1);
    TEST(returns_0() == 0);
    TEST(returns_1() == 1);

    TEST(libdylib_open_self());
    TEST(libdylib_find(libdylib_open_self(), "main"));
}
