#include "../private.h"

namespace ryo {
    void _render_target_release_views(_InternalRenderTarget& renderTarget);
    void _render_target_release(_InternalRenderTarget& renderTarget);
    bool _render_target_create_textures(ID3D11Device* device, int width, int height, _InternalRenderTarget& result);
}