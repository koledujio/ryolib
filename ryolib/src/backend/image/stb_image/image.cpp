#include "private.h"

namespace ryo {
    bool image_load(const char* path, int& outWidth, int& outHeight, void*& outPixels) {
        RYO_ENSURE(path, false, "Null path pointer provided");

        int channels;
        unsigned char* pixels = stbi_load(path, &outWidth, &outHeight, &channels, 4);
        if (!pixels) {
            error_set("Failed to load image '%s': %s", path, stbi_failure_reason());
            outPixels = nullptr;
            return false;
        }

        outPixels = pixels;
        return true;
    }

    void image_free_pixels(void* pixels) {
        RYO_ENSURE_VOID(pixels, "Null pixels pointer provided");

        stbi_image_free(pixels);
    }

    Texture image_load_texture(Renderer renderer, const char* path, int& outWidth, int& outHeight) {
        int width, height;
        void* pixels;

        if (!image_load(path, width, height, pixels)) {
            return {}; // image load prints error already, it has more data about the error
        }

        Texture texture = texture_create(renderer, pixels, width, height);
        if (!texture_is_valid(texture)) {
            error_set("Failed to create texture from image '%s': %s", path, error_get());
            image_free_pixels(pixels);
            return {};
        }

        image_free_pixels(pixels);

        outWidth = width;
        outHeight = height;

        return texture;
    }
}