#include "../../private.h"

#include "ryolib/image.h"

namespace ryo {
    bool image_load(const char* path, int& outWidth, int& outHeight, void*& outPixels) {
        RYO_ENSURE(path, false, "Null path pointer provided");

        constexpr int width = 4;
        constexpr int height = 4;

        constexpr unsigned char purple[] = {
            128, 0, 128, 255
        };

        constexpr unsigned char black[] = {
            0, 0, 0, 255
        };

        auto* pixels = new unsigned char[width * height * 4];

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const unsigned char* color = ((x + y) % 2 == 0)
                    ? purple
                    : black;

                const int offset = (y * width + x) * 4;

                pixels[offset + 0] = color[0];
                pixels[offset + 1] = color[1];
                pixels[offset + 2] = color[2];
                pixels[offset + 3] = color[3];
            }
        }

        outWidth = width;
        outHeight = height;
        outPixels = pixels;

        return true;
    }

    void image_free_pixels(void* pixels) {
        RYO_ENSURE_VOID(pixels, "Null pixels pointer provided");

        delete[] static_cast<unsigned char*>(pixels);
    }

    Texture* image_load_texture(const Renderer& renderer, const char* path, int& outWidth, int& outHeight) {
        int width, height;
        void* pixels;

        if (!image_load(path, width, height, pixels)) {
            return nullptr;
        }

        Texture* texture = new Texture(renderer, pixels, width, height);
        if (!texture->valid()) {
            delete texture;
            error_set("Failed to create texture from image '%s': %s", path, error_get());
            delete[] static_cast<unsigned char*>(pixels);
            return nullptr;
        }

        delete[] static_cast<unsigned char *>(pixels);

        outWidth = width;
        outHeight = height;

        return texture;
    }
}