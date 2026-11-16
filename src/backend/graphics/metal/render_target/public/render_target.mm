#include "../private.h"

namespace ryo {
    RenderTarget::RenderTarget(const Renderer& renderer, unsigned int width, unsigned int height) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid renderer");

        id<MTLDevice> device = renderer.internal->device;

        id<MTLTexture> colorTexture = _render_target_create_color_texture(device, width, height);
        id<MTLTexture> depthTexture = _render_target_create_depth_texture(device, width, height);

        InternalTexture* colorTextureIntern = new InternalTexture;
        colorTextureIntern->texture = colorTexture;
        Texture* colorTextureObject = new Texture(colorTextureIntern);

        InternalTexture* depthTextureIntern = new InternalTexture;
        depthTextureIntern->texture = depthTexture;
        Texture* depthTextureObject = new Texture(depthTextureIntern);

        InternalRenderTarget* intern = new InternalRenderTarget;
        intern->colorTexture = colorTextureObject;
        intern->depthTexture = depthTextureObject;
        intern->width = width;
        intern->height = height;

        internal = intern;
    }

    RenderTarget::~RenderTarget() {
        if (!internal)
            return;
        
        if (internal->colorTexture)
            delete internal->colorTexture;
        
        if (internal->depthTexture)
            delete internal->depthTexture;
        
        delete internal;
    }

    RenderTarget::RenderTarget(RenderTarget&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    RenderTarget& RenderTarget::operator=(RenderTarget&& other) {
        if (this == &other)
            return *this;
        
        if (internal) {
            if (internal->colorTexture)
                delete internal->colorTexture;
            
            if (internal->depthTexture)
                delete internal->depthTexture;
            
            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool RenderTarget::valid() const {
        return internal != nullptr && internal->colorTexture != nullptr && internal->colorTexture->valid() && internal->depthTexture != nullptr && internal->depthTexture->valid();
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
