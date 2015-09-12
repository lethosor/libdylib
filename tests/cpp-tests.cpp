#include "libdylibxx.h"
#include "test.inc.h"

using namespace libdylib;

void run_tests()
{
    TEST(!libdylib::last_error());

    dylib lib(lib_path);
    TEST(lib.is_open());
    TEST(lib.get_handle());

    TEST(!lib.open("foo"));
    TEST(lib.close());
    TEST(!lib.close());
    TEST(!lib.open("foo"));
    TEST(lib.open(lib_path));

    TEST(lib.close());
    TEST(lib.open_list(lib_path, "foo", NULL));
    TEST(lib.close());
    TEST(lib.open_list("foo", lib_path, NULL));
    TEST(lib.close());
    TEST(!lib.open_list("foo", "foo", "bar", "baz", "", NULL));

    dylib plib(plib_path, true);
    TEST(plib.is_open());
    TEST(dylib(lib_path, true).is_open());
    TEST(!dylib("foo", true).is_open());

    lib.open(lib_path);
    TEST_STRICT(lib.get_handle() && lib.is_open());

    TEST(lib.lookup("sym1"));
    TEST(lib.lookup("sym2"));
    TEST(lib.lookup("sym3"));
    TEST(lib.find("sym1"));
    TEST(lib.find("sym2"));
    TEST(lib.find("sym3"));

    TEST(lib.find_any(NULL, "sym1", "sym2", "sym3", NULL));
    TEST(lib.find_all(NULL, "sym1", "sym2", "sym3", NULL));
    TEST(lib.find_any(NULL, "x", "sym1", NULL));
    TEST(lib.find_any(NULL, "sym1", "x", NULL));
    TEST(!lib.find_all(NULL, "x", "sym1", NULL));
    TEST(!lib.find_all(NULL, "sym1", "x", NULL));

    int (*returns_0)(void), (*returns_1)(void);
    TEST(lib.bind("returns_0", returns_0));
    TEST(returns_0() == 0);
    TEST(DYLIB_BINDNAME(lib, returns_1));
    TEST(returns_1() == 1);

    TEST(libdylib::self.is_open());
    TEST(libdylib::self.get_handle() == libdylib::open_self());
    TEST(libdylib::self.find("main"));

    {
        dylib_ref *handle = NULL;
        {
            dylib lib2(lib_path);
            handle = &(lib2.get_handle());
            TEST(handle && *handle);
        }
        TEST(handle && !*handle);
    }

    {
        dylib_ref *handle = NULL;
        dylib lib2("foo");
        handle = &(lib2.get_handle());
        TEST(handle && !*handle);
    }

}
