#pragma once

#include "../private.h"

#include "ryolib/text.h"

#include <SDL3_ttf/SDL_ttf.h>

#include "ryolib/templates.h"

namespace ryo {
    constexpr unsigned int MAX_FONTS = 64;

    struct _InternalFont {
        TTF_Font* sdlFont;
        float size;
    };

    extern ResourcePool<_InternalFont, MAX_FONTS> _font_pool;
}