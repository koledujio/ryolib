#include "../private.h"

namespace ryo {
    ResourcePool<_InternalRenderTarget, MAX_RENDER_TARGETS> _render_target_pool;

    RenderTarget render_target_create(Renderer renderer, int width, int height) {
        RYO_ENSURE(renderer_is_valid(renderer), {}, "Invalid renderer handle");
        RYO_ENSURE(width > 0 && height > 0, {}, "Invalid render target dimensions");

        if (_render_target_pool.active_count >= MAX_RENDER_TARGETS) {
            error_set("Maximum number of renderers reached");
            return {};
        }

        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];

        _InternalRenderTarget renderTarget = {};
        if (!_render_target_create_textures(ir.device, width, height, renderTarget)) { // error logs are in this function
            return {};
        }

        INDEX idx = _render_target_pool.allocate_index();
        GENID gen = _render_target_pool.generations[idx];

        _render_target_pool.items[idx] = renderTarget;

        return RenderTarget::pack(idx, gen);
    }

    void render_target_destroy(RenderTarget target) {
        RYO_ENSURE_VOID(render_target_is_valid(target), "Invalid render target handle");

        INDEX idx = target.index();

        _render_target_release(_render_target_pool.items[idx]);

        _render_target_pool.free_index(idx);
    }

    bool render_target_is_valid(RenderTarget target) {
        if (target.is_null()) return false;

        INDEX idx = target.index();
        GENID gen = target.gen();

        if (idx >= MAX_RENDER_TARGETS || idx == 0)
            return false;

        if (!_render_target_pool.is_valid(idx, gen)) return false;

        _InternalRenderTarget& irt = _render_target_pool.items[idx];

        return irt.colorTexture != nullptr && irt.colorView != nullptr && irt.depthTexture != nullptr && irt.depthView != nullptr;
    }

    Texture render_target_get_color_texture(RenderTarget target) {
        RYO_ENSURE(render_target_is_valid(target), {}, "Invalid render target handle");

        return _render_target_pool.items[target.index()].colorTextureHandle;
    }

    Texture render_target_get_depth_texture(RenderTarget target) {
        RYO_ENSURE(render_target_is_valid(target), {}, "Invalid render target handle");

        return _render_target_pool.items[target.index()].depthTextureHandle;
    }
} 