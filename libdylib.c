#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include "libdylib.h"

#ifdef LIBDYLIB_CXX
using libdylib::dylib_ref;
#endif

#define ERR_MAX_SIZE 2048
static char last_err[ERR_MAX_SIZE];
static short last_err_set = 0;
void set_last_error(const char *s)
{
    last_err_set = 1;
    strncpy(last_err, s, ERR_MAX_SIZE);
}

#define check_null_handle(handle, ret) if (!handle) {set_last_error("NULL library handle"); return ret; }

#if defined(LIBDYLIB_UNIX)
#include <dlfcn.h>

void unix_set_last_error()
{
    const char *e = dlerror();
    if (!e)
        e = "NULL error";
    set_last_error(e);
}

LIBDYLIB_DEFINE(dylib_ref*, open)(const char *path)
{
    dylib_ref *lib = (dylib_ref*)dlopen(path, RTLD_LOCAL);
    if (!lib)
        unix_set_last_error();
    return lib;
}

LIBDYLIB_DEFINE(dylib_ref*, open_self)()
{
    return (dylib_ref*)RTLD_SELF;
}

LIBDYLIB_DEFINE(short, close)(dylib_ref *lib)
{
    check_null_handle(lib, 0);
    int ret = dlclose((void*)lib);
    if (ret != 0)
        unix_set_last_error();
    return ret == 0;
}

LIBDYLIB_DEFINE(void*, lookup)(dylib_ref *lib, const char *symbol)
{
    check_null_handle(lib, NULL);
    void *ret = dlsym((void*)lib, symbol);
    if (!ret)
        unix_set_last_error();
    return ret;
}

// end LIBDYLIB_UNIX
#elif defined(LIBDYLIB_WINDOWS)
#include <Windows.h>

void win_set_last_error()
{
    // Based on http://stackoverflow.com/questions/1387064
    DWORD code = GetLastError();
    if (!code)
        set_last_error("NULL error");
    else
    {
        LPSTR buf = NULL;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buf, 0, NULL);
        set_last_error(buf);
        LocalFree(buf);
    }
}

LIBDYLIB_DEFINE(dylib_ref*, open)(const char *path)
{
    dylib_ref *lib = (dylib_ref*)LoadLibrary(path);
    if (!lib)
        win_set_last_error();
    return lib;
}

LIBDYLIB_DEFINE(dylib_ref*, open_self)()
{
    return (dylib_ref*)GetModuleHandle(NULL);
}

LIBDYLIB_DEFINE(short, close)(dylib_ref *lib)
{
    check_null_handle(lib, 0);
    BOOL ret = FreeLibrary((HMODULE)lib);
    if (ret != 0)
        win_set_last_error();
    return ret == 0;
}

LIBDYLIB_DEFINE(void*, lookup)(dylib_ref *lib, const char *symbol)
{
    check_null_handle(lib, NULL);
    void *ret = (void*)GetProcAddress((HMODULE*)lib, symbol);
    if (!ret)
        win_set_last_error();
    return ret;
}

// end LIBDYLIB_WINDOWS
#else
#error "unrecognized platform"
#endif

// All platforms

LIBDYLIB_DEFINE(dylib_ref*, open_list)(const char *path, ...)
{
    va_list args;
    va_start(args, path);
    dylib_ref *ret = LIBDYLIB_NAME(va_open_list)(path, args);
    va_end(args);
    return ret;
}

LIBDYLIB_DEFINE(dylib_ref*, va_open_list)(const char *path, va_list args)
{
    const char *curpath = path;
    dylib_ref *ret = NULL;
    while (curpath)
    {
        ret = LIBDYLIB_NAME(open)(curpath);
        if (ret)
            break;
        curpath = va_arg(args, const char*);
    }
    return ret;
}

LIBDYLIB_DEFINE(short, bind)(dylib_ref *lib, const char *symbol, void **dest)
{
    *dest = LIBDYLIB_NAME(lookup)(lib, symbol);
    return *dest != 0;
}

LIBDYLIB_DEFINE(short, find)(dylib_ref *lib, const char *symbol)
{
    return LIBDYLIB_NAME(lookup)(lib, symbol) != NULL;
}

LIBDYLIB_DEFINE(short, find_any)(dylib_ref *lib, ...)
{
    va_list args;
    va_start(args, lib);
    short ret = LIBDYLIB_NAME(va_find_any)(lib, args);
    va_end(args);
    return ret;
}
LIBDYLIB_DEFINE(short, va_find_any)(dylib_ref *lib, va_list args)
{
    const char *cursym = NULL;
    short ret = 0;
    while (!ret && (cursym = va_arg(args, const char*)))
    {
        if (LIBDYLIB_NAME(lookup)(lib, cursym))
            ret = 1;
    }
    return ret;
}
LIBDYLIB_DEFINE(short, find_all)(dylib_ref *lib, ...)
{
    va_list args;
    va_start(args, lib);
    short ret = LIBDYLIB_NAME(va_find_all)(lib, args);
    va_end(args);
    return ret;
}
LIBDYLIB_DEFINE(short, va_find_all)(dylib_ref *lib, va_list args)
{
    const char *cursym = NULL;
    short ret = 1;
    while (ret && (cursym = va_arg(args, const char*)))
    {
        if (!LIBDYLIB_NAME(lookup)(lib, cursym))
            ret = 0;
    }
    return ret;
}

LIBDYLIB_DEFINE(const char*, last_error)()
{
    if (!last_err_set)
        return NULL;
    return last_err;
}

LIBDYLIB_DEFINE(int, get_version)()
{
    return LIBDYLIB_VERSION;
}

LIBDYLIB_DEFINE(const char*, get_version_str())
{
    return LIBDYLIB_VERSION_STR;
}
