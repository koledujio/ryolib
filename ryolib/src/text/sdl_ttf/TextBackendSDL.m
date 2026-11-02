#if defined(RYO_ENABLE_TEXT_SDL)

#import "TextBackendSDL.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>

static unsigned int g_numTextBackends = 0;

static inline Uint8 clamp_channel(float v) {
    if (v <= 0.0f) return 0;
    if (v >= 1.0f) return 255;
    return (Uint8)(v * 255.0f);
}

@implementation RYTextBackendSDL

- (instancetype)initBackendWithError:(RYError **)error {
    if (error) {
        *error = nil;
    }

    self = [super init];
    if (!self) {
        return nil;
    }

    if (g_numTextBackends == 0) {
        if (TTF_WasInit() == 0) {
            if (!TTF_Init()) {
                if (error) {
                    *error = [[RYError alloc] initWithCode:RYErrorCodeTextBackendInitializationFailed
                                                   message:SDL_GetError()];
                }
                [self release];
                return nil;
            }
        }
    }

    g_numTextBackends++;
    return self;
}

- (void)dealloc {
    if (g_numTextBackends > 0) {
        g_numTextBackends--;
        if (g_numTextBackends == 0) {
            if (TTF_WasInit() != 0) {
                TTF_Quit();
            }
        }
    }
    [super dealloc];
}

- (void *)createFontWithPath:(RYString *)path
                    fontSize:(float)fontSize
                       error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!path || !path.UTF8String) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeTextInvalidFontPath
                                           message:"Font path is null or empty"];
        }
        return NULL;
    }

    if (fontSize <= 0.0f) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeTextInvalidFontSize
                                           message:"Font size must be greater than zero"];
        }
        return NULL;
    }

    TTF_Font *font = TTF_OpenFont(path.UTF8String, fontSize);
    if (!font) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeTextLoadFontFailed
                                           message:SDL_GetError()];
        }
        return NULL;
    }

    return font;
}

- (void *)createFontWithMemory:(const void *)memory
                          size:(unsigned long)size
                      fontSize:(float)fontSize
                         error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!memory || size == 0) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeTextInvalidMemory
                                           message:"Memory buffer is null or size is zero"];
        }
        return NULL;
    }

    if (fontSize <= 0.0f) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeTextInvalidFontSize
                                           message:"Font size must be greater than zero"];
        }
        return NULL;
    }

    SDL_IOStream *stream = SDL_IOFromConstMem(memory, (size_t)size);
    if (!stream) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeTextBackendError
                                           message:SDL_GetError()];
        }
        return NULL;
    }

    TTF_Font *font = TTF_OpenFontIO(stream, true, fontSize);
    if (!font) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeTextLoadFontFailed
                                           message:SDL_GetError()];
        }
        return NULL;
    }

    return font;
}

- (void)destroyFont:(void *)fontHandle {
    if (fontHandle) {
        TTF_CloseFont((TTF_Font *)fontHandle);
    }
}

- (BOOL)addFallbackFont:(void *)fallbackHandle
                 toFont:(void *)fontHandle
                  error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!fontHandle || !fallbackHandle) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralInvalidArgument
                                           message:"Font handle or fallback font handle is null"];
        }
        return NO;
    }

    if (!TTF_AddFallbackFont((TTF_Font *)fontHandle, (TTF_Font *)fallbackHandle)) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeTextFallbackFailed
                                           message:SDL_GetError()];
        }
        return NO;
    }

    return YES;
}

- (BOOL)removeFallbackFont:(void *)fallbackHandle
                  fromFont:(void *)fontHandle
                     error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!fontHandle || !fallbackHandle) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralInvalidArgument
                                           message:"Font handle or fallback font handle is null"];
        }
        return NO;
    }

    TTF_RemoveFallbackFont((TTF_Font *)fontHandle, (TTF_Font *)fallbackHandle);
    return YES;
}

- (void)clearFallbackFonts:(void *)fontHandle {
    if (fontHandle) {
        TTF_ClearFallbackFonts((TTF_Font *)fontHandle);
    }
}

- (void *)renderLine:(RYString *)text
                font:(void *)fontHandle
               color:(RYColor)color
            outWidth:(int *)outWidth
           outHeight:(int *)outHeight
               error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!fontHandle) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralInvalidArgument
                                           message:"Font handle is null"];
        }
        return NULL;
    }

    if (!text || !text.UTF8String) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralInvalidArgument
                                           message:"Text string is null"];
        }
        return NULL;
    }

    SDL_Color fg = {
        clamp_channel(color.r),
        clamp_channel(color.g),
        clamp_channel(color.b),
        clamp_channel(color.a)
    };

    SDL_Surface *surface = TTF_RenderText_Blended((TTF_Font *)fontHandle, text.UTF8String, 0, fg);
    if (!surface) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeTextRenderFailed
                                           message:SDL_GetError()];
        }
        return NULL;
    }

    SDL_Surface *converted = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(surface);

    if (!converted) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeTextRenderFailed
                                           message:SDL_GetError()];
        }
        return NULL;
    }

    int w = converted->w;
    int h = converted->h;

    if (w <= 0 || h <= 0) {
        SDL_DestroySurface(converted);
        if (outWidth) *outWidth = 0;
        if (outHeight) *outHeight = 0;
        return NULL;
    }

    uint32_t *pixels = (uint32_t *)malloc((size_t)w * (size_t)h * sizeof(uint32_t));
    if (!pixels) {
        SDL_DestroySurface(converted);
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGeneralOutOfMemory
                                           message:"Failed to allocate text pixel buffer"];
        }
        return NULL;
    }

    uint8_t *dst = (uint8_t *)pixels;
    uint8_t *src = (uint8_t *)converted->pixels;
    int y;

    for (y = 0; y < h; ++y) {
        memcpy(dst + (size_t)y * (size_t)w * sizeof(uint32_t),
               src + (size_t)y * (size_t)converted->pitch,
               (size_t)w * sizeof(uint32_t));
    }

    SDL_DestroySurface(converted);

    if (outWidth) *outWidth = w;
    if (outHeight) *outHeight = h;

    return pixels;
}

- (void)freePixels:(void *)pixels {
    if (pixels) {
        free(pixels);
    }
}

@end

#endif
