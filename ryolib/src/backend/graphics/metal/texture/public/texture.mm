#include "../../private.h"

namespace ryo {
    ResourcePool<id<MTLTexture>, MAX_TEXTURES> _texture_pool;

    Texture texture_create(Renderer renderer, const void *pixels, int width, int height) {
        RYO_ENSURE(renderer_is_valid(renderer), {}, "Invalid renderer handle");

        if (_texture_pool.active_count >= MAX_TEXTURES) {
            error_set("Maximum number of textures reached");
            return { 0 };
        }

        id<MTLDevice> device = _renderer_pool.items[renderer.index()].device;

        INDEX idx = _texture_pool.allocate_index();
        GENID gen = _texture_pool.generations[idx];

        MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm width:width height:height mipmapped:NO];

        id<MTLTexture> mtlTexture = [device newTextureWithDescriptor:desc];

        MTLRegion region = MTLRegionMake2D(0, 0, width, height);
        NSUInteger bytesPerRow = 4 * width; // assuming 4 bytes per pixel (RGBA)
        [mtlTexture replaceRegion:region mipmapLevel:0 withBytes:pixels bytesPerRow:bytesPerRow];

        _texture_pool.items[idx] = mtlTexture;

        return Texture::pack(idx, gen);
    }

    void texture_destroy(Texture texture) {
        RYO_ENSURE_VOID(texture_is_valid(texture), "Invalid texture handle");

        INDEX idx = texture.index();

        [_texture_pool.items[idx] release];
        _texture_pool.items[idx] = nil;

        _texture_pool.free_index(idx);
    }

    bool texture_is_valid(Texture texture) {
        INDEX idx = texture.index();

        if (idx >= MAX_TEXTURES)
            return false;
        
        if (!_texture_pool.is_valid(idx, texture.gen()))
            return false;

        return _texture_pool.items[idx] != nil;
    }
}
