#include "SDL3/SDL_surface.h"
#include "private.h"

#include <SDL3/SDL_error.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>
#include <cstring>
#include <algorithm>
#include <string>

namespace ryo {
    ResourcePool<_InternalFont, MAX_FONTS> _font_pool;

    Font font_create(const FontConfig& config) {
        RYO_ENSURE(config.path != nullptr || config.memory != nullptr , {}, "FontConfig requires either a path or memory data");
        RYO_ENSURE(config.path != nullptr && config.memory != nullptr, {}, "FontConfig cannot specify both path and memory data");

        if (TTF_WasInit() == 0) {
            if (!TTF_Init()) {
                error_set("Failed to init SDL_TTF, %s", SDL_GetError());
                return {};
            }
        }

        TTF_Font* sdlFont = nullptr;
        if (config.path) {
            sdlFont = TTF_OpenFont(config.path, config.fontSize);
            if (!sdlFont) {
                error_set("Failed to open SDL_TTF font from path '%s': %s", config.path, SDL_GetError());
                return {};
            }
        } else {
            SDL_IOStream* stream = SDL_IOFromConstMem(config.memory, config.memorySize);
            if (!stream) {
                error_set("Failed to create SDL_IOStream from memory, %s", SDL_GetError());
                return {};
            }
            sdlFont = TTF_OpenFontIO(stream, true, config.fontSize);
            if (!sdlFont) {
                error_set("Failed to open SDL_TTF font from memory: %s", SDL_GetError());
                return {};
            }
        }

        INDEX idx = _font_pool.allocate_index();
        GENID gen = _font_pool.generations[idx];

        _InternalFont& internal = _font_pool.items[idx];
        internal.sdlFont = sdlFont;
        internal.size = config.fontSize;

        return Font::pack(idx, gen);
    }

    void font_destroy(Font font) {
        RYO_ENSURE_VOID(font_is_valid(font), "Invalid font handle");

        INDEX idx = font.index();
        _InternalFont& internal = _font_pool.items[idx];
        if (internal.sdlFont) {
            TTF_CloseFont(internal.sdlFont);
            internal.sdlFont = nullptr;
        }
        _font_pool.free_index(idx);
    }

    bool font_is_valid(Font font) {
        INDEX idx = font.index();

        if (idx >= MAX_FONTS || idx == 0)
            return false;

        if (!_font_pool.is_valid(idx, font.gen()))
            return false;

        return _font_pool.items[idx].sdlFont != nullptr;
    }

    bool font_add_fallback(Font font, Font fallback) {
        RYO_ENSURE(font_is_valid(font) && font_is_valid(fallback), false, "Invalid font handle(s)");

        INDEX origIdx = font.index();
        INDEX fallIdx = fallback.index();

        _InternalFont& origInternal = _font_pool.items[origIdx];
        _InternalFont& fallInternal = _font_pool.items[fallIdx];

        TTF_Font* origSdlFont = origInternal.sdlFont;
        TTF_Font* fallSdlFont = fallInternal.sdlFont;

        if (!TTF_AddFallbackFont(origSdlFont, fallSdlFont)) {
            error_set("Failed to add SDL_TTF fallback font: %s", SDL_GetError());
            return false;
        }

        return true;
    }

    void font_remove_fallback(Font font, Font fallback) {
        RYO_ENSURE_VOID(font_is_valid(font) && font_is_valid(fallback), "Invalid font handle(s)");

        INDEX origIdx = font.index();
        INDEX fallIdx = fallback.index();

        _InternalFont& origInternal = _font_pool.items[origIdx];
        _InternalFont& fallInternal = _font_pool.items[fallIdx];

        TTF_Font* origSdlFont = origInternal.sdlFont;
        TTF_Font* fallSdlFont = fallInternal.sdlFont;

        TTF_RemoveFallbackFont(origSdlFont, fallSdlFont);
    }

    void font_clear_fallbacks(Font font) {
        RYO_ENSURE_VOID(font_is_valid(font), "Invalid font handle");

        INDEX origIdx = font.index();
        _InternalFont& origInternal = _font_pool.items[origIdx];
        TTF_Font* origSdlFont = origInternal.sdlFont;

        TTF_ClearFallbackFonts(origSdlFont);
    }

    bool text_render_line(Font font, const char* text, const Color& color, int& outWidth, int& outHeight, void*& outPixels) {
        RYO_ENSURE(font_is_valid(font), false, "Invalid font handle");
        RYO_ENSURE(text, false, "Null text pointer provided");

        INDEX idx = font.index();
        _InternalFont& internal = _font_pool.items[idx];

        SDL_Color fg = {
            static_cast<Uint8>(std::clamp(color.r * 255.0f, 0.0f, 255.0f)),
            static_cast<Uint8>(std::clamp(color.g * 255.0f, 0.0f, 255.0f)),
            static_cast<Uint8>(std::clamp(color.b * 255.0f, 0.0f, 255.0f)),
            static_cast<Uint8>(std::clamp(color.a * 255.0f, 0.0f, 255.0f))
        };

        SDL_Surface* surface = TTF_RenderText_Blended(internal.sdlFont, text, 0, fg);
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

        memcpy(pixels, converted->pixels, outWidth * outHeight * sizeof(uint32_t));
        outPixels = pixels;

        SDL_DestroySurface(converted);
        return true;
    }

    void text_free_pixels(void* pixels) {
        RYO_ENSURE_VOID(pixels != nullptr, "Null pixels pointer provided");

        free(pixels);
    }
}