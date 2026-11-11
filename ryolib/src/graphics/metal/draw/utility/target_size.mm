#include "../private.h"

void _draw_get_target_size(const RYInternalRenderer& renderer, int& width, int& height) {
    if (renderer.currentTarget) {
        RYInternalRenderTarget *internTarget = (RYInternalRenderTarget *)renderer.currentTarget.internalRenderTarget;
        if (internTarget) {
            width = internTarget->width;
            height = internTarget->height;
            return;
        }
    }

    width = renderer.width;
    height = renderer.height;
}
