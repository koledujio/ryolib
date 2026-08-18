#include "../private.h"

namespace ryo {
    void _draw_get_target_size(const _InternalRenderer& renderer, int& width, int& height) {
        if (!renderer.currentTarget.is_null() && render_target_is_valid(renderer.currentTarget)) {
            const _InternalRenderTarget& irt = _render_target_pool.items[renderer.currentTarget.index()];

            width = irt.width;
            height = irt.height;
            return;
        }

        width = renderer.width;
        height = renderer.height;
    }
}