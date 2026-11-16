#pragma once

namespace ryo {
    void error_set(const char* format, ...);
    const char* error_get();
    void error_clear();
}