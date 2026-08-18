#pragma once

#include "graphics.h"

namespace ryo {
    bool image_load(const char* path, int& outWidth, int& outHeight, void*& outPixels);
    void image_free_pixels(void* pixels);
    Texture image_load_texture(Renderer renderer, const char* path, int& outWidth, int& outHeight);
}