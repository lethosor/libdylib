#if !defined(LIBDYLIB_UNIX) && (defined(__APPLE__) || defined(__linux__) || defined(__UNIX__))
    #define LIBDYLIB_UNIX
#elif !defined(LIBDYLIB_WINDOWS) && defined(WIN32)
    #define LIBDYLIB_WINDOWS
#else
    #error "Could not detect platform - try defining LIBDYLIB_UNIX or LIBDYLIB_WINDOWS appropriately"
#endif

#ifdef LIBDYLIB_UNIX
    #define LIBDYLIB_EXPORT __attribute__((visibility("default")))
#else
    #define LIBDYLIB_EXPORT __declspec(dllexport)
#endif

#ifdef __cplusplus
    #ifndef LIBDYLIB_C
        #define LIBDYLIB_CXX
    #endif
#else
    #ifdef LIBDYLIB_CXX
        #error "Can't build as C++ with a C compiler"
    #else
        #define LIBDYLIB_C
    #endif
#endif

#ifdef LIBDYLIB_C
    #define LIBDYLIB_NAME(name) libdylib_##name
    #define LIBDYLIB_DECLARE(type, name) LIBDYLIB_EXPORT type libdylib_##name
    #define LIBDYLIB_DEFINE(type, name) type libdylib_##name
#else
    #define LIBDYLIB_NAME(name) libdylib::name
    #define LIBDYLIB_DECLARE(type, name) LIBDYLIB_EXPORT type name
    #define LIBDYLIB_DEFINE(type, name) type libdylib::name
#endif

#ifdef LIBDYLIB_CXX
namespace libdylib {
#endif

    // Empty struct
    typedef struct {} DynamicLibrary;

    // attempt to load a dynamic library from a path
    // return a library handle or NULL
    LIBDYLIB_DECLARE(DynamicLibrary*, open)(const char *path);

    // return a handle to the current executable
    LIBDYLIB_DECLARE(DynamicLibrary*, self)();

    // close the specified dynamic library
    // returns 1 on success, 0 on failure
    LIBDYLIB_DECLARE(short, close)(DynamicLibrary *lib);

    // attempt to load a dynamic library from all paths given
    // return a library handle of the first successfully-loaded library, or NULL if none were successfully loaded
    // NOTE: the last argument must be NULL
    LIBDYLIB_DECLARE(DynamicLibrary*, open_list)(const char *path, ...);

    // return the address of a symbol in a library, or NULL if the symbol does not exist
    LIBDYLIB_DECLARE(void*, lookup)(DynamicLibrary *lib, const char *symbol);

    // set the contents of dest to the result of lookup(lib, symbol) and returns 1,
    // or set dest to NULL and returns 0 if the symbol was not found
    LIBDYLIB_DECLARE(short, bind)(DynamicLibrary *lib, const char *symbol, void **dest);
    // helper macros - note that dest is a simple pointer, NOT a pointer to a pointer
    #define LIBDYLIB_BIND(lib, symbol, dest) LIBDYLIB_NAME(bind)(lib, symbol, (void**)&dest)
    #define LIBDYLIB_BINDNAME(lib, name) LIBDYLIB_BIND(lib, #name, name)

    // check for the existence of a symbol in a library
    LIBDYLIB_DECLARE(short, find)(DynamicLibrary *lib, const char *symbol);

    // check for the existence of any or all specified symbols in a library, respectively
    // NOTE: the last argument must be NULL
    LIBDYLIB_DECLARE(short, find_any)(DynamicLibrary *lib, ...);
    LIBDYLIB_DECLARE(short, find_all)(DynamicLibrary *lib, ...);

    // returns the last error message set by libdylib functions, or NULL
    LIBDYLIB_DECLARE(const char*, last_error)();

#ifdef LIBDYLIB_CXX
}
#endif
