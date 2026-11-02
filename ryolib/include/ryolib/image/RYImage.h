#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYString.h"
#import "../objc/RYError.h"
#import "RYImageBackend.h"

/**
 * Identifies the backend implementation used to decode images.
 */
typedef enum {
    RYImageBackendTypeDefault,
    RYImageBackendTypeStub,
    RYImageBackendTypeStb,
    RYImageBackendTypeCustom
} RYImageBackendType;

/**
 * @brief Represents decoded 2D image pixel data.
 * 
 * An RYImage manages an RGBA pixel buffer, dimensions, channel count,
 * and handles memory deallocation when released.
 */
@interface RYImage : RYObject {
@protected
    RYString *_path;
    int _width;
    int _height;
    int _channels;
    const void *_pixels;
    BOOL _ownsPixels;
    RYImageBackend *_backendInstance;
}

/** File path from which the image was loaded, or nil if created from memory or raw pixels. */
@property (nonatomic, readonly) RYString *path;

/** Width of the image in pixels. */
@property (nonatomic, readonly) int width;

/** Height of the image in pixels. */
@property (nonatomic, readonly) int height;

/** Channel count of the image (typically 4 for RGBA). */
@property (nonatomic, readonly) int channels;

/** Direct pointer to the raw pixel buffer. */
@property (nonatomic, readonly) const void *pixels;

/** Backend instance used to load and free the pixel buffer. */
@property (nonatomic, readonly) RYImageBackend *backendInstance;

/** Creates a new image from a file path using the default backend. */
+ (instancetype)newImageWithPath:(RYString *)path;

/** Creates a new image from a file path using the default backend with error reporting. */
+ (instancetype)newImageWithPath:(RYString *)path error:(RYError **)error;

/** Creates a new image from an encoded memory buffer using the default backend. */
+ (instancetype)newImageWithMemory:(const void *)memory size:(unsigned long)size;

/** Creates a new image from an encoded memory buffer using the default backend with error reporting. */
+ (instancetype)newImageWithMemory:(const void *)memory size:(unsigned long)size error:(RYError **)error;

/** Creates a new image from a file path using the specified backend type with error reporting. */
+ (instancetype)newImageWithPath:(RYString *)path backend:(RYImageBackendType)backendType error:(RYError **)error;

/** Creates a new image from an encoded memory buffer using the specified backend type. */
+ (instancetype)newImageWithMemory:(const void *)memory
                              size:(unsigned long)size
                           backend:(RYImageBackendType)backendType
                             error:(RYError **)error;

/**
 * Initializes an image wrapping pre-allocated pixel data.
 * 
 * @param pixels Pointer to raw pixel buffer.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param channels Number of color channels.
 * @param freeWhenDone If YES, the pixel buffer is freed upon image deallocation.
 */
- (instancetype)initWithPixels:(void *)pixels
                         width:(int)width
                        height:(int)height
                      channels:(int)channels
                  freeWhenDone:(BOOL)freeWhenDone;

/**
 * Initializes an image wrapping pixel data owned by a specific backend instance.
 */
- (instancetype)initWithPixels:(void *)pixels
                         width:(int)width
                        height:(int)height
                      channels:(int)channels
                  freeWhenDone:(BOOL)freeWhenDone
               backendInstance:(RYImageBackend *)backendInstance;

@end

#if defined(__cplusplus)
namespace ryo {
    struct Renderer;
    struct Texture;
}

/**
 * @brief Category bridging RYImage with the graphics renderer for texture creation.
 */
@interface RYImage (Texture)

/**
 * Creates a GPU texture from this image's pixel data.
 * 
 * @param renderer The renderer to create the GPU texture on.
 * @return A new ryo::Texture instance, or nullptr on failure.
 */
- (ryo::Texture *)createTextureWithRenderer:(const ryo::Renderer &)renderer;

/**
 * Creates a GPU texture from this image's pixel data, reporting errors.
 * 
 * @param renderer The renderer to create the GPU texture on.
 * @param error Out parameter for error reporting.
 * @return A new ryo::Texture instance, or nullptr on failure.
 */
- (ryo::Texture *)createTextureWithRenderer:(const ryo::Renderer &)renderer error:(RYError **)error;

@end
#endif
