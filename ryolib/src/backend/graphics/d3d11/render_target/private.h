#include "../private.h"

namespace ryo {
    void _render_target_release_views(InternalRenderTarget& renderTarget);
    void _render_target_release(InternalRenderTarget& renderTarget);
    bool _render_target_create_textures(ID3D11Device* device, int width, int height, InternalRenderTarget& result);
}