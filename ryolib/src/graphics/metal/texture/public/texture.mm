#include "../../private.h"

@implementation RYMetalGraphicsBackend (Texture)

- (void *)createTextureWithPixels:(const void *)pixels
                            width:(unsigned int)width
                           height:(unsigned int)height
                            error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_internal) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInitializationFailed
                                           message:"Metal backend is not initialized"];
        }
        return NULL;
    }

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;

    MTLTextureDescriptor *desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                   width:width
                                                                                  height:height
                                                                               mipmapped:NO];
    id<MTLTexture> mtlTexture = [intern->device newTextureWithDescriptor:desc];

    if (!mtlTexture) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsTextureCreationFailed
                                           message:"Failed to create MTLTexture"];
        }
        return NULL;
    }

    if (pixels) {
        MTLRegion region = MTLRegionMake2D(0, 0, width, height);
        NSUInteger bytesPerRow = 4 * width; // RGBA 4 bytes per pixel
        [mtlTexture replaceRegion:region mipmapLevel:0 withBytes:pixels bytesPerRow:bytesPerRow];
    }

    RYInternalTexture *texture = new RYInternalTexture;
    texture->texture = mtlTexture;

    return texture;
}

- (void)destroyTexture:(void *)textureHandle {
    if (!textureHandle) return;

    RYInternalTexture *texture = (RYInternalTexture *)textureHandle;
    if (texture->texture) {
        [texture->texture release];
    }
    delete texture;
}

@end
