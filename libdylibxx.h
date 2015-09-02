#ifndef LIBDYLIBXX_H
#define LIBDYLIBXX_H

#include <string>

#include "libdylib.h"

namespace libdylib {
    class dylib {
    protected:
        dylib_ref *handle;
    public:
        dylib(const char *path = NULL);
        ~dylib();
        bool open(const char *path);
        inline bool open(std::string path) { return open(path.c_str()); }
        bool open_list(const char *path, ...);
        bool close();

        void *lookup(const char *symbol);
        inline void *lookup(std::string symbol) { return lookup(symbol.c_str()); }
        bool find(const char *symbol);
        inline bool find(std::string symbol) { return find(symbol.c_str()); }

        // Note that these functions use the 'handle' member internally regardless
        // of the first argument passed here (which is needed to pass through
        // all remaining arguments properly)
        bool find_any(dylib_ref *unused, ...);
        bool find_all(dylib_ref *unused, ...);

        template<typename T>
        bool bind(const char *symbol, T* &dest) {
            dest = (T*)lookup(symbol);
            return dest != NULL;
        }
        template<typename T>
        bool bind(std::string symbol, T* &dest) {
            return bind<T>(symbol.c_str(), dest);
        }
        #define DYLIB_BINDNAME(lib, name) lib.bind(#name, name)

        inline dylib_ref *get_handle() { return handle; }
        inline bool is_open() { return handle != NULL; }
    };
    class dylib_self : public dylib {
    public:
        dylib_self();
        ~dylib_self();
    private:
        bool open(const char*);
        bool open_list(const char*, ...);
        bool close();
    };
    extern dylib_self self;
}

#endif /* LIBDYLIBXX_H */
