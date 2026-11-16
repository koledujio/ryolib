#include "private.h"

#include "ryolib/debug.h"

#include <cstdio>
#include <cstdarg>
#include <cstring>

namespace ryo {
#if defined(__vita__)
    // The Vita toolchain emits emulated TLS relocations for thread_local
    // storage, which vita-elf-create cannot handle. Use a plain static buffer.
    static char _error_buffer[1024] = {};
#else
    static thread_local char _error_buffer[1024] = {};
#endif

    void error_set(const char* format, ...) {
        va_list args;
        va_start(args, format);

        vsnprintf(_error_buffer, sizeof(_error_buffer), format, args);

        va_end(args);
    }

    const char* error_get() {
        return _error_buffer;
    }

    void error_clear() {
        _error_buffer[0] = '\0';
    }
}