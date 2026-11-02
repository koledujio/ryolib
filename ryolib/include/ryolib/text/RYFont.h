#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYString.h"
#import "../objc/RYArray.h"
#import "../objc/RYError.h"
#import "../objc/RYColor.h"
#import "RYTextBackend.h"

@class RYImage;

/**
 * Identifies the backend implementation used for font rasterization.
 */
typedef enum {
    RYTextBackendTypeDefault, /**< Default platform-recommended backend (e.g. SDL3_ttf). */
    RYTextBackendTypeStub,    /**< Headless/stub backend returning dummy pixels. */
    RYTextBackendTypeSDL3TTF, /**< SDL3_ttf based font rendering backend. */
    RYTextBackendTypeCustom   /**< Custom user-provided text backend. */
} RYTextBackendType;

/**
 * @brief Represents a loaded font with support for text rasterization and font fallback chains.
 * 
 * An RYFont instance manages a rasterization backend, font size, fallback font list,
 * and renders text lines into raw pixel buffers or RYImage instances.
 */
@interface RYFont : RYObject {
@protected
    RYString *_path;
    float _fontSize;
    RYArray *_fallbackFonts;
    RYTextBackend *_backendInstance;
    void *_backendFont;
}

/** File path from which the font was loaded, or nil if created from memory. */
@property (nonatomic, readonly) RYString *path;

/** Font point/pixel size. */
@property (nonatomic, readonly) float fontSize;

/** Array of fallback fonts attached to this font. */
@property (nonatomic, readonly) RYArray *fallbackFonts;

/** Text backend instance managing this font. */
@property (nonatomic, readonly) RYTextBackend *backendInstance;

/** Opaque handle to backend font resource. */
@property (nonatomic, readonly) void *backendFont;

/** Creates a new font from a file path using the default backend. */
+ (instancetype)newFontWithPath:(RYString *)path size:(float)fontSize;

/** Creates a new font from a file path with error reporting. */
+ (instancetype)newFontWithPath:(RYString *)path size:(float)fontSize error:(RYError **)error;

/** Creates a new font from memory using the default backend. */
+ (instancetype)newFontWithMemory:(const void *)memory size:(unsigned long)size fontSize:(float)fontSize;

/** Creates a new font from memory with error reporting. */
+ (instancetype)newFontWithMemory:(const void *)memory size:(unsigned long)size fontSize:(float)fontSize error:(RYError **)error;

/**
 * Creates a new font from a file path using the specified backend type.
 * 
 * @param path Path to font file.
 * @param fontSize Font size in pixels.
 * @param backendType Backend type to use.
 * @param error Out parameter for error reporting.
 * @return A new RYFont instance, or nil on failure.
 */
+ (instancetype)newFontWithPath:(RYString *)path
                           size:(float)fontSize
                        backend:(RYTextBackendType)backendType
                          error:(RYError **)error;

/**
 * Creates a new font from memory using the specified backend type.
 * 
 * @param memory Pointer to font data in memory.
 * @param size Size of memory buffer in bytes.
 * @param fontSize Font size in pixels.
 * @param backendType Backend type to use.
 * @param error Out parameter for error reporting.
 * @return A new RYFont instance, or nil on failure.
 */
+ (instancetype)newFontWithMemory:(const void *)memory
                             size:(unsigned long)size
                         fontSize:(float)fontSize
                          backend:(RYTextBackendType)backendType
                            error:(RYError **)error;

/**
 * Initializes a new font instance with backend details.
 */
- (instancetype)initWithPath:(RYString *)path
                    fontSize:(float)fontSize
             backendInstance:(RYTextBackend *)backendInstance
                 backendFont:(void *)backendFont;

/**
 * Attaches a fallback font used when characters/glyphs are missing in this font.
 * 
 * @param fallback Fallback font instance.
 * @return YES if successfully added, NO otherwise.
 */
- (BOOL)addFallbackFont:(RYFont *)fallback;

/**
 * Attaches a fallback font with error reporting.
 * 
 * @param fallback Fallback font instance.
 * @param error Out parameter for error reporting.
 * @return YES if successfully added, NO otherwise.
 */
- (BOOL)addFallbackFont:(RYFont *)fallback error:(RYError **)error;

/**
 * Removes an attached fallback font.
 * 
 * @param fallback Fallback font to detach.
 * @return YES if removed, NO otherwise.
 */
- (BOOL)removeFallbackFont:(RYFont *)fallback;

/**
 * Removes an attached fallback font with error reporting.
 * 
 * @param fallback Fallback font to detach.
 * @param error Out parameter for error reporting.
 * @return YES if removed, NO otherwise.
 */
- (BOOL)removeFallbackFont:(RYFont *)fallback error:(RYError **)error;

/**
 * Clears all fallback fonts attached to this font.
 */
- (void)clearFallbackFonts;

/**
 * Renders a single line of text into a newly allocated 32-bit RGBA pixel buffer.
 * 
 * @param text The text string to render.
 * @param color Color to render the text with.
 * @param outWidth Out parameter for rendered width.
 * @param outHeight Out parameter for rendered height.
 * @param error Out parameter for error reporting.
 * @return Pointer to pixel buffer (must be freed with freePixels: or free()), or NULL on error.
 */
- (void *)renderLine:(RYString *)text
               color:(RYColor)color
            outWidth:(int *)outWidth
           outHeight:(int *)outHeight
               error:(RYError **)error;

/**
 * Renders a single line of text directly into an RYImage instance.
 * 
 * @param text The text string to render.
 * @param color Color to render the text with.
 * @param error Out parameter for error reporting.
 * @return An RYImage wrapping the rendered line, or nil on error.
 */
- (RYImage *)renderLineToImage:(RYString *)text
                         color:(RYColor)color
                         error:(RYError **)error;

/**
 * Frees a pixel buffer returned by renderLine:.
 * 
 * @param pixels Pointer to pixel buffer.
 */
- (void)freePixels:(void *)pixels;

@end
