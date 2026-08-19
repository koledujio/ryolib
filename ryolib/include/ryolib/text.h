#pragma once

#include <cstddef>
#include <stdint.h>

#include "templates.h"
#include "graphics.h"

namespace ryo {
    struct InternalFont;

    struct FontConfig {
        const char* path = nullptr; // path to .ttf or .otf file relative to resource directory

        const void* memory = nullptr; // alternative to path if loading from memory
        size_t memorySize = 0; // size for the memory property above

        float fontSize = 48.0f; // size in pixels
    };

    struct Font {
        InternalFont* internal = nullptr;

        Font(const FontConfig& config);
        ~Font();

        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;

        Font(Font&&);
        Font& operator=(Font&&);

        bool valid() const;

        bool fallback_add(const Font& fallback);
        void fallback_remove(const Font& fallback);
        void fallback_clear();

        bool pixels_render_line(const char* text, const Color& color, int& outWidth, int& outHeight, void*& outPixels) const;
        void pixels_free(void* pixels) const;
    };
}