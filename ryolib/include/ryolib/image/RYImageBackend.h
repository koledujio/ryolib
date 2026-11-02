#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYString.h"
#import "../objc/RYError.h"

/**
 * @brief Base class for image decoding backends.
 * 
 * Subclasses implement decoding of image data from files or memory buffers into raw RGBA pixels.
 */
@interface RYImageBackend : RYObject

/**
 * Initializes an image backend instance.
 */
- (instancetype)initBackendWithError:(RYError **)error;

/**
 * @brief Loads and decodes an image from a file path into raw RGBA pixel data.
 * 
 * @param path Path to the image file relative to the resource or current directory.
 * @param width Out parameter for decoded image width in pixels.
 * @param height Out parameter for decoded image height in pixels.
 * @param channels Out parameter for channel count (always 4 for RGBA).
 * @param error Out parameter for error reporting.
 * @return Pointer to decoded pixel buffer, or NULL on error. Must be freed with freePixels:.
 */
- (void *)loadImageWithPath:(RYString *)path
                   outWidth:(int *)width
                  outHeight:(int *)height
                outChannels:(int *)channels
                      error:(RYError **)error;

/**
 * @brief Loads and decodes an image from memory into raw RGBA pixel data.
 * 
 * @param memory Pointer to encoded image data in memory.
 * @param size Size of memory buffer in bytes.
 * @param width Out parameter for decoded image width in pixels.
 * @param height Out parameter for decoded image height in pixels.
 * @param channels Out parameter for channel count (always 4 for RGBA).
 * @param error Out parameter for error reporting.
 * @return Pointer to decoded pixel buffer, or NULL on error. Must be freed with freePixels:.
 */
- (void *)loadImageWithMemory:(const void *)memory
                         size:(unsigned long)size
                     outWidth:(int *)width
                    outHeight:(int *)height
                  outChannels:(int *)channels
                        error:(RYError **)error;

/**
 * @brief Frees a pixel buffer allocated by this backend.
 * 
 * @param pixels Pointer to pixel data previously returned by this backend.
 */
- (void)freePixels:(void *)pixels;

@end
