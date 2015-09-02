#include "libdylibxx.h"
#include "libdylib.c"

using libdylib::dylib;
using libdylib::dylib_self;

dylib_self libdylib::self;

dylib::dylib(const char *path) : handle(NULL)
{
    if (path)
        open(path);
}

dylib::~dylib()
{
    if (handle)
        close();
}

bool dylib::open(const char *path)
{
    if (handle)
        return false;
    handle = libdylib::open(path);
    return handle;
}

bool dylib::open_list(const char *path, ...)
{
    va_list args;
    va_start(args, path);
    handle = libdylib::va_open_list(path, args);
    va_end(args);
    return handle;
}

bool dylib::close()
{
    bool ret = libdylib::close(handle);
    handle = NULL;
    return ret;
}

void *dylib::lookup(const char *symbol)
{
    return libdylib::lookup(handle, symbol);
}

bool dylib::find(const char *symbol)
{
    return libdylib::find(handle, symbol);
}

bool dylib::find_any(dylib_ref unused, ...)
{
    va_list args;
    va_start(args, unused);
    bool ret = libdylib::va_find_any(handle, args);
    va_end(args);
    return ret;
}

bool dylib::find_all(dylib_ref unused, ...)
{
    va_list args;
    va_start(args, unused);
    bool ret = libdylib::va_find_all(handle, args);
    va_end(args);
    return ret;
}

dylib_self::dylib_self()
{
    handle = libdylib::open_self();
}

dylib_self::~dylib_self() {}

bool dylib_self::open(const char*) { return false; }
bool dylib_self::open_list(const char*, ...) { return false; }
bool dylib_self::close() { return false; }
