#pragma once

#include "../private.h"

#include "ryolib/text.h"

#include <SDL3_ttf/SDL_ttf.h>

namespace ryo {
    struct InternalFont {
        TTF_Font* sdlFont = nullptr;
        float size = 0.0f;
    };
}