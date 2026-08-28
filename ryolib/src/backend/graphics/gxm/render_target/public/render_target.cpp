#include "../../private.h"

namespace ryo {
    RenderTarget::RenderTarget(const Renderer& renderer, unsigned int width, unsigned int height) {}
    RenderTarget::~RenderTarget() {}

    RenderTarget::RenderTarget(RenderTarget&&) {}
    RenderTarget& RenderTarget::operator=(RenderTarget&&) {return *this;}

    bool RenderTarget::valid() const {return true;}

    const Texture* RenderTarget::texture_color_get() const {return nullptr;}
    const Texture* RenderTarget::texture_depth_get() const {return nullptr;} 
}