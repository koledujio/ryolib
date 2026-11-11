#include "../private.h"

namespace ryo {
    bool _render_target_create(const Renderer& renderer, unsigned int width, unsigned int height, InternalRenderTarget* target);
    void _render_target_destroy(InternalRenderTarget* target);
}