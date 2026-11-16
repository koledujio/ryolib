#include "private.h"

#include <SDL3/SDL_error.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>

namespace ryo {
    Font::Font(const FontConfig& config) {
        RYO_ENSURE_VOID(config.path != nullptr || config.memory != nullptr, "FontConfig requires either a path or memory data");
        RYO_ENSURE_VOID(config.path == nullptr || config.memory == nullptr, "FontConfig cannot specify both path and memory data");

        if (TTF_WasInit() == 0) {
            if (!TTF_Init()) {
                error_set("Failed to init SDL_TTF, %s", SDL_GetError());
                return;
            }
        }

        TTF_Font* sdlFont = nullptr;
        if (config.path) {
            sdlFont = TTF_OpenFont(config.path, config.fontSize);
            if (!sdlFont) {
                error_set("Failed to open SDL_TTF font from path '%s': %s", config.path, SDL_GetError());
                return;
            }
        } else {
            SDL_IOStream* stream = SDL_IOFromConstMem(config.memory, config.memorySize);
            if (!stream) {
                error_set("Failed to create SDL_IOStream from memory, %s", SDL_GetError());
                return;
            }
            sdlFont = TTF_OpenFontIO(stream, true, config.fontSize);
            if (!sdlFont) {
                error_set("Failed to open SDL_TTF font from memory: %s", SDL_GetError());
                return;
            }
        }

        InternalFont* intern = new InternalFont;
        intern->sdlFont = sdlFont;
        intern->size = config.fontSize;

        internal = intern;
    }

    Font::~Font() {
        if (!internal)
            return;

        if (internal->sdlFont)
            TTF_CloseFont(internal->sdlFont);
        
        delete internal;
    }

    Font::Font(Font&& otherFont) : internal(otherFont.internal) {
        otherFont.internal = nullptr;
    }

    Font& Font::operator=(Font&& otherFont) {
        if (this == &otherFont)
            return *this;

        if (internal) {
            if (internal->sdlFont)
                TTF_CloseFont(internal->sdlFont);

            delete internal;
        }

        internal = otherFont.internal;
        otherFont.internal = nullptr;

        return *this;
    }

    bool Font::valid() const {
        return internal != nullptr && internal->sdlFont != nullptr;
    }

    bool Font::fallback_add(const Font& fallback) {
        RYO_ENSURE(valid() && fallback.valid(), false, "Invalid font handle(s)");

        TTF_Font* origSdlFont = internal->sdlFont;
        TTF_Font* fallSdlFont = fallback.internal->sdlFont;

        if (!TTF_AddFallbackFont(origSdlFont, fallSdlFont)) {
            error_set("Failed to add SDL_TTF fallback font: %s", SDL_GetError());
            return false;
        }

        return true;
    }

    void Font::fallback_remove(const Font& fallback) {
        RYO_ENSURE_VOID(valid() && fallback.valid(), "Invalid font handle(s)");

        TTF_Font* origSdlFont = internal->sdlFont;
        TTF_Font* fallSdlFont = fallback.internal->sdlFont;

        TTF_RemoveFallbackFont(origSdlFont, fallSdlFont);
    }

    void Font::fallback_clear() {
        RYO_ENSURE_VOID(valid(), "Invalid font handle");

        TTF_ClearFallbackFonts(internal->sdlFont);
    }

    bool Font::pixels_render_line(const char* text, const Color& color, int& outWidth, int& outHeight, void*& outPixels) const {
        RYO_ENSURE(valid(), false, "Invalid font handle");
        RYO_ENSURE(text, false, "Null text pointer provided");

        SDL_Color fg = {
            static_cast<Uint8>(std::clamp(color.r * 255.0f, 0.0f, 255.0f)),
            static_cast<Uint8>(std::clamp(color.g * 255.0f, 0.0f, 255.0f)),
            static_cast<Uint8>(std::clamp(color.b * 255.0f, 0.0f, 255.0f)),
            static_cast<Uint8>(std::clamp(color.a * 255.0f, 0.0f, 255.0f))
        };

        SDL_Surface* surface = TTF_RenderText_Blended(internal->sdlFont, text, 0, fg);
        if (!surface) {
            error_set("Failed to render text line '%s': %s", text, SDL_GetError());
            return false;
        }

        SDL_Surface* converted = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(surface);

        if (!converted) {
            error_set("Failed to convert texture to RGBA8888: %s", SDL_GetError());
            return false;
        }

        outWidth = converted->w;
        outHeight = converted->h;

        if (outWidth <= 0 || outHeight <= 0) {
            outWidth = 0;
            outHeight = 0;
            outPixels = nullptr;
            SDL_DestroySurface(converted);
            return false;
        }

        uint32_t* pixels = static_cast<uint32_t*>(malloc(outWidth * outHeight * sizeof(uint32_t)));

        if (!pixels) {
            SDL_DestroySurface(converted);
            error_set("Failed to allocate text pixel buffer");
            return false;
        }

        uint8_t* dst = static_cast<uint8_t*>(static_cast<void*>(pixels));
        uint8_t* src = static_cast<uint8_t*>(converted->pixels);

        for (int y = 0; y < outHeight; ++y) {
            memcpy(
                dst + y * outWidth * sizeof(uint32_t),
                src + y * converted->pitch,
                outWidth * sizeof(uint32_t)
            );
        }
        outPixels = pixels;

        SDL_DestroySurface(converted);
        return true;
    }

    void Font::pixels_free(void* pixels) const {
        RYO_ENSURE_VOID(pixels != nullptr, "Null pixels pointer provided");

        free(pixels);
    }
}