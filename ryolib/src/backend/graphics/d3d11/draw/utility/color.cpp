#include "../private.h"

namespace ryo {
    bool _draw_bind_color(_InternalRenderer& renderer, const Color& color) {
        ID3D11Buffer* buffer = nullptr;

        if (!_draw_create_dynamic_buffer(renderer.device, &color, sizeof(color), D3D11_BIND_CONSTANT_BUFFER, &buffer)) {
            return false;
        }

        renderer.context->PSSetConstantBuffers(0, 1, &buffer);

        buffer->Release();
        return true;
    }
}