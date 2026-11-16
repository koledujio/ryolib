#include "../private.h"

namespace ryo {
    RenderTarget::RenderTarget(const Renderer& renderer, unsigned int width, unsigned int height) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid renderer");

        InternalRenderTarget* intern = new InternalRenderTarget;
        if (!_render_target_create(renderer, width, height, intern)) {
            _render_target_destroy(intern);
            delete intern;
            return;
        }

        internal = intern;
    }

    RenderTarget::~RenderTarget() {
        if (!internal)
            return;

        _render_target_destroy(internal);

        delete internal;
    }

    RenderTarget::RenderTarget(RenderTarget&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    RenderTarget& RenderTarget::operator=(RenderTarget&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            _render_target_destroy(internal);
            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool RenderTarget::valid() const {
        return (internal != nullptr &&
                internal->renderTarget != nullptr &&
                internal->colorTexture != nullptr &&
                internal->colorTexture->valid());
    }

    const Texture* RenderTarget::texture_color_get() const {
        RYO_ENSURE(valid(), nullptr, "Invalid render target");
        return internal->colorTexture;
    }

    const Texture* RenderTarget::texture_depth_get() const {
        return nullptr; // to be removed on all platforms i think
    } 
}