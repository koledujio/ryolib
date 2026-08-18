#pragma once

#include <cstddef>
#include <stdint.h>

#include "templates.h"
#include "graphics.h"

namespace ryo {
    using Font = Handle<struct FontTag>;

    struct FontConfig {
        const char* path; // path to .ttf or .otf file relative to resource directory
        const void* memory; // alternative to path if loading from memory
        size_t memorySize; // size for the memory property above
        float fontSize; // size in pixels
    };

    Font font_create(const FontConfig& config);
    void font_destroy(Font font);
    bool font_is_valid(Font font);

    bool font_add_fallback(Font font, Font fallback);
    void font_remove_fallback(Font font, Font fallback);
    void font_clear_fallbacks(Font font);

    bool text_render_line(Font font, const char* text, const Color& color, int& outWidth, int& outHeight, void*& outPixels);
    void text_free_pixels(void* pixels);
}