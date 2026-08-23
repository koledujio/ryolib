#include "../../private.h"

namespace ryo {
    Texture::Texture(const Renderer& renderer, const void *pixels, unsigned int width, unsigned int height) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid renderer handle");
        RYO_ENSURE_VOID(!renderer.internal->inFrame, "Renderer is in a frame");

        MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm width:width height:height mipmapped:NO];
        id<MTLTexture> mtlTexture = [renderer.internal->device newTextureWithDescriptor:desc];

        MTLRegion region = MTLRegionMake2D(0, 0, width, height);
        NSUInteger bytesPerRow = 4 * width; // assuming 4 bytes per pixel (RGBA)
        [mtlTexture replaceRegion:region mipmapLevel:0 withBytes:pixels bytesPerRow:bytesPerRow];

        InternalTexture* intern = new InternalTexture;
        intern->texture = mtlTexture;

        internal = intern;
    }

    Texture::Texture(InternalTexture* internal) {
        this->internal = internal;
    }

    Texture::~Texture() {
        if (!internal)
            return;
        
        if (internal->texture)
            [internal->texture release];
        
        delete internal;
    }

    Texture::Texture(Texture&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Texture& Texture::operator=(Texture&& other) {
        if (this == &other)
            return *this;
        
        if (internal) {
            if (internal->texture)
                [internal->texture release];
            
            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Texture::valid() const {
        return internal != nullptr && internal->texture != nil;
    }
}
