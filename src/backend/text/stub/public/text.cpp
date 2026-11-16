#include "../private.h"

namespace ryo {
    struct InternalFont {};

    Font::Font(const FontConfig& config) {}
    Font::~Font() {}

    Font::Font(Font&&) {}
    Font& Font::operator=(Font&&) {return *this;}

    bool Font::valid() const {return true;}

    bool Font::fallback_add(const Font& fallback) {return true;}
    void Font::fallback_remove(const Font& fallback) {}
    void Font::fallback_clear() {}

    bool Font::pixels_render_line(const char* text, const Color& color, int& outWidth, int& outHeight, void*& outPixels) const {
        RYO_ENSURE(text, false, "Null text pointer provided");

        constexpr int width = 100;
        constexpr int height = 100;

        constexpr unsigned char purple[] = {
            128, 0, 128, 255
        };

        constexpr unsigned char black[] = {
            0, 0, 0, 255
        };

        auto* pixels = new unsigned char[width * height * 4];

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const unsigned char* color = ((x + y) % 20 == 0)
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

    void Font::pixels_free(void* pixels) const {
        RYO_ENSURE_VOID(pixels, "Null pixels pointer provided");

        delete[] static_cast<unsigned char*>(pixels);
    }
}