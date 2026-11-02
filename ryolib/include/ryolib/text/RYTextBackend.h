#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYString.h"
#import "../objc/RYError.h"
#import "../objc/RYColor.h"

/**
 * @brief Base class for font rasterization backends.
 * 
 * Subclasses implement font loading, fallback font chains, and line rasterization.
 */
@interface RYTextBackend : RYObject

/**
 * @brief Initializes a text backend instance.
 * 
 * @param error Out parameter for error reporting if initialization fails.
 * @return Initialized backend instance, or nil on error.
 */
- (instancetype)initBackendWithError:(RYError **)error;

/**
 * @brief Creates a backend font handle from a font file path.
 * 
 * @param path Path to font file (.ttf / .otf).
 * @param fontSize Font size in pixels.
 * @param error Out parameter for error reporting.
 * @return Opaque backend font handle, or NULL on error.
 */
- (void *)createFontWithPath:(RYString *)path
                    fontSize:(float)fontSize
                       error:(RYError **)error;

/**
 * @brief Creates a backend font handle from font data in memory.
 * 
 * @param memory Pointer to font data.
 * @param size Size in bytes.
 * @param fontSize Font size in pixels.
 * @param error Out parameter for error reporting.
 * @return Opaque backend font handle, or NULL on error.
 */
- (void *)createFontWithMemory:(const void *)memory
                          size:(unsigned long)size
                      fontSize:(float)fontSize
                         error:(RYError **)error;

/**
 * @brief Destroys a backend font handle.
 * 
 * @param fontHandle Backend font handle previously created by this backend.
 */
- (void)destroyFont:(void *)fontHandle;

/**
 * @brief Adds a fallback font to be queried for missing glyphs.
 * 
 * @param fallbackHandle Font handle to act as fallback.
 * @param fontHandle Base font handle to attach fallback to.
 * @param error Out parameter for error reporting.
 * @return YES on success, NO on failure.
 */
- (BOOL)addFallbackFont:(void *)fallbackHandle
                 toFont:(void *)fontHandle
                  error:(RYError **)error;

/**
 * @brief Removes an attached fallback font from the base font.
 * 
 * @param fallbackHandle Fallback font handle to detach.
 * @param fontHandle Base font handle.
 * @param error Out parameter for error reporting.
 * @return YES on success, NO on failure.
 */
- (BOOL)removeFallbackFont:(void *)fallbackHandle
                  fromFont:(void *)fontHandle
                     error:(RYError **)error;

/**
 * @brief Removes all fallback fonts from the base font.
 * 
 * @param fontHandle Base font handle.
 */
- (void)clearFallbackFonts:(void *)fontHandle;

/**
 * @brief Renders a single line of text into 32-bit RGBA pixels.
 * 
 * @param text The text string to render.
 * @param fontHandle Backend font handle.
 * @param color RGBA color to render text with.
 * @param outWidth Out parameter for rendered width in pixels.
 * @param outHeight Out parameter for rendered height in pixels.
 * @param error Out parameter for error reporting.
 * @return Newly allocated pixel buffer, or NULL on error. Must be freed with freePixels:.
 */
- (void *)renderLine:(RYString *)text
                font:(void *)fontHandle
               color:(RYColor)color
            outWidth:(int *)outWidth
           outHeight:(int *)outHeight
               error:(RYError **)error;

/**
 * @brief Frees a pixel buffer allocated by renderLine:.
 * 
 * @param pixels Pointer to pixel data to free.
 */
- (void)freePixels:(void *)pixels;

@end
