#include "../private.h"

namespace ryo {
    RenderTarget::RenderTarget(const Renderer& renderer, unsigned int width, unsigned int height) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid renderer");
        RYO_ENSURE_VOID(width > 0 && height > 0, "Invalid render target dimensions");
        RYO_ENSURE_VOID(!renderer.internal->inFrame, "Renderer is in a frame");

        InternalRenderTarget* intern = new InternalRenderTarget;
        if (!_render_target_create_textures(renderer.internal->device, width, height, *intern)) { // error logs are in this function
            delete intern;
            return;
        }

        internal = intern;
    }

    RenderTarget::~RenderTarget() {
        if (!internal)
            return;

        _render_target_release(*internal);

        delete internal;
    }

    RenderTarget::RenderTarget(RenderTarget&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    RenderTarget& RenderTarget::operator=(RenderTarget&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            _render_target_release(*internal);

            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool RenderTarget::valid() const {
        return (internal != nullptr &&
                internal->colorView != nullptr &&
                internal->depthView != nullptr &&
                internal->colorTexture != nullptr &&
                internal->depthTexture != nullptr &&
                internal->width > 0 &&
                internal->height > 0);
    }

    const Texture* RenderTarget::texture_color_get() const {
        RYO_ENSURE(valid(), nullptr, "Invalid render target");

        return internal->colorTexture;
    }

    const Texture* RenderTarget::texture_depth_get() const {
        RYO_ENSURE(valid(), nullptr, "Invalid render target");

        return internal->depthTexture;
    }
} 