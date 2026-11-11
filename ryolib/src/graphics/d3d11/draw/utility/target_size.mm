#include "../private.h"

void _draw_get_target_size(const RYInternalRenderer& renderer, int& width, int& height) {
    if (renderer.currentTarget) {
        RYInternalRenderTarget* targetIntern = (RYInternalRenderTarget*)renderer.currentTarget.internalRenderTarget;
        width = targetIntern->width;
        height = targetIntern->height;
        return;
    }

    width = renderer.width;
    height = renderer.height;
}
