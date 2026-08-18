#include "../private.h"

namespace ryo {
    ResourcePool<_InternalRenderTarget, MAX_RENDER_TARGETS> _render_target_pool;
 
    RenderTarget render_target_create(Renderer renderer, int width, int height) {
        RYO_ENSURE(renderer_is_valid(renderer), {}, "Invalid renderer handle");

        if (_render_target_pool.active_count >= MAX_RENDER_TARGETS) {
            error_set("Maximum number of render targets reached");
            return {};
        }

        if (_texture_pool.active_count >= (MAX_TEXTURES - 1)) { // 2 textures will be created
            error_set("Maximum number of textures reached");
            return {};
        }

        id<MTLDevice> device = _renderer_pool.items[renderer.index()].device;

        INDEX idx = _render_target_pool.allocate_index();
        GENID gen = _render_target_pool.generations[idx];

        id<MTLTexture> colorTexture = _render_target_create_color_texture(device, width, height);
        id<MTLTexture> depthTexture = _render_target_create_depth_texture(device, width, height);

        uint32_t colorTextureId = _texture_pool.allocate_index();
        _texture_pool.items[colorTextureId] = colorTexture;

        uint32_t depthTextureId = _texture_pool.allocate_index();
        _texture_pool.items[depthTextureId] = depthTexture;

        _InternalRenderTarget& irt = _render_target_pool.items[idx];
        irt.colorTexture = colorTexture;
        irt.depthTexture = depthTexture;
        irt.width = width;
        irt.height = height;
        irt.colorTextureId = colorTextureId;
        irt.depthTextureId = depthTextureId;

        return RenderTarget::pack(idx, gen);
    }

    void render_target_destroy(RenderTarget target) {
        RYO_ENSURE_VOID(render_target_is_valid(target), "Invalid render target handle");

        INDEX idx = target.index();
        _InternalRenderTarget& irt = _render_target_pool.items[idx];
        
        id<MTLTexture> colorTexture = irt.colorTexture;
        id<MTLTexture> depthTexture = irt.depthTexture;

        if (_texture_pool.items[irt.colorTextureId] == colorTexture) {
            [_texture_pool.items[irt.colorTextureId] release];
            _texture_pool.items[irt.colorTextureId] = nil;
            _texture_pool.free_index(irt.colorTextureId);
        }

        if (_texture_pool.items[irt.depthTextureId] == depthTexture) {
            [_texture_pool.items[irt.depthTextureId] release];
            _texture_pool.items[irt.depthTextureId] = nil;
            _texture_pool.free_index(irt.depthTextureId);
        }

        irt.colorTexture = nil;
        irt.colorTextureId = 0;
        irt.depthTexture = nil;
        irt.depthTextureId = 0;

        _render_target_pool.free_index(idx);
    }

    bool render_target_is_valid(RenderTarget target) {
        INDEX idx = target.index();

        if (idx >= MAX_RENDER_TARGETS)
            return false;
        
        if (!_render_target_pool.is_valid(idx, target.gen()))
            return false;

        return _render_target_pool.items[idx].colorTextureId != 0 && _render_target_pool.items[idx].depthTextureId != 0;
    }

    Texture render_target_get_color_texture(RenderTarget target) {
        RYO_ENSURE(render_target_is_valid(target), {}, "Invalid render target handle");

        INDEX idx = target.index();
        _InternalRenderTarget& irt = _render_target_pool.items[idx];

        id<MTLTexture> colorTexture = irt.colorTexture;
        id<MTLTexture> supposedColorTexture = _texture_pool.items[irt.colorTextureId];

        if (colorTexture == supposedColorTexture) {
            return Texture::pack(irt.colorTextureId, _texture_pool.generations[irt.colorTextureId]);
        }

        error_set("Failed to find render target color texture");
        return { 0 };
    }

    Texture render_target_get_depth_texture(RenderTarget target) {
        RYO_ENSURE(render_target_is_valid(target), {}, "Invalid render target handle");

        INDEX idx = target.index();
        _InternalRenderTarget& irt = _render_target_pool.items[idx];

        id<MTLTexture> depthTexture = irt.depthTexture;
        id<MTLTexture> supposedDepthTexture = _texture_pool.items[irt.depthTextureId];

        if (depthTexture == supposedDepthTexture) {
            return Texture::pack(irt.depthTextureId, _texture_pool.generations[irt.depthTextureId]);
        }

        error_set("Failed to find render target depth texture");
        return { 0 };
    }
}
