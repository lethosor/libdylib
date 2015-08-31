#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include "libdylib.h"

#ifdef LIBDYLIB_CXX
using libdylib::DynamicLibrary;
#endif

#define ERR_MAX_SIZE 2048
static char last_err[ERR_MAX_SIZE];
void set_last_error(const char *s)
{
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

}

LIBDYLIB_DEFINE(DynamicLibrary*, open)(const char *path)
{
    DynamicLibrary *lib = (DynamicLibrary*)dlopen(path, RTLD_LOCAL);
    if (!lib)
        unix_set_last_error();
    return lib;
}

LIBDYLIB_DEFINE(DynamicLibrary*, self)()
{
    return (DynamicLibrary*)RTLD_SELF;
}

LIBDYLIB_DEFINE(short, close)(DynamicLibrary *lib)
{
    check_null_handle(lib, 0);
    int ret = dlclose((void*)lib);
    if (ret != 0)
        unix_set_last_error();
    return ret == 0;
}

LIBDYLIB_DEFINE(void*, lookup)(DynamicLibrary *lib, const char *symbol)
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
    const char *e = NULL;
    if (!code)
        e = "NULL error";
    else
    {
        LPSTR buf = NULL;
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buf, 0, NULL);
        set_last_error(buf);
        LocalFree(buf);
    }
}

LIBDYLIB_DEFINE(DynamicLibrary*, open)(const char *path)
{
    DynamicLibrary *lib = (DynamicLibrary*)LoadLibrary(path);
    if (!lib)
        win_set_last_error();
    return lib;
}

LIBDYLIB_DEFINE(DynamicLibrary*, self)()
{
    return (DynamicLibrary*)GetModuleHandle(NULL);
}

LIBDYLIB_DEFINE(short, close)(DynamicLibrary *lib)
{
    check_null_handle(lib, 0);
    BOOL ret = FreeLibrary((HMODULE)lib);
    if (ret != 0)
        win_set_last_error();
    return ret == 0;
}

LIBDYLIB_DEFINE(void*, lookup)(DynamicLibrary *lib, const char *symbol)
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

LIBDYLIB_DEFINE(DynamicLibrary*, open_list)(const char *path, ...)
{
    const char *curpath = path;
    DynamicLibrary *ret = NULL;
    va_list args;
    va_start(args, path);
    while (curpath)
    {
        ret = LIBDYLIB_NAME(open)(curpath);
        if (ret)
            break;
        curpath = va_arg(args, const char*);
    }
    va_end(args);
    return ret;
}

LIBDYLIB_DEFINE(short, find)(DynamicLibrary *lib, const char *symbol)
{
    return LIBDYLIB_NAME(lookup)(lib, symbol) != NULL;
}


LIBDYLIB_DEFINE(short, find_any)(DynamicLibrary *lib, ...)
{
    const char *cursym = NULL;
    short ret = 0;
    va_list args;
    va_start(args, lib);
    while (!ret && (cursym = va_arg(args, const char*)))
    {
        if (LIBDYLIB_NAME(lookup)(lib, cursym))
            ret = 1;
    }
    va_end(args);
    return ret;
}

LIBDYLIB_DEFINE(short, find_all)(DynamicLibrary *lib, ...)
{
    const char *cursym = NULL;
    short ret = 1;
    va_list args;
    va_start(args, lib);
    while (ret && (cursym = va_arg(args, const char*)))
    {
        if (LIBDYLIB_NAME(lookup)(lib, cursym))
            ret = 0;
    }
    va_end(args);
    return ret;
}

LIBDYLIB_DEFINE(const char*, last_error)()
{
    return last_err;
}
