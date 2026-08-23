#include "../private.h"

namespace ryo {
    void _draw_get_target_size(const InternalRenderer& renderer, int& width, int& height) {
        if (renderer.currentTarget && renderer.currentTarget->valid()) {
            width = renderer.currentTarget->internal->width;
            height = renderer.currentTarget->internal->height;
            return;
        }

        width = renderer.width;
        height = renderer.height;
    }
}