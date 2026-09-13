#include "../private.h"

namespace ryo {
    void _draw_active_target_size_get(const InternalRenderer& renderer, int& outWidth, int& outHeight) {
        if (renderer.currentTarget && renderer.currentTarget->valid()) {
            outWidth = renderer.currentTarget->internal->width;
            outHeight = renderer.currentTarget->internal->height;
        } else {
            outWidth = renderer.width;
            outHeight = renderer.height;
        }
    }
}