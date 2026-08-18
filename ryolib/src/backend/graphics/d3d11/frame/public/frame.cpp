#include "../../private.h"

#include <chrono>

namespace ryo {
    void frame_begin(Renderer renderer) {
        // optional safety flags like ir.inFrame can be added here
        // in d3d11, immediate commands execute directly so frame_begin is not needed as much

        RYO_ENSURE_VOID(renderer_is_valid(renderer), "Invalid renderer handle");

        INDEX idx = renderer.index();
        _InternalRenderer& ir = _renderer_pool.items[idx];

        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        if (ir.lastTime.time_since_epoch().count() != 0) {
            std::chrono::duration<float> elapsed = now - ir.lastTime;
            ir.deltaTime = elapsed.count();
        } else {
            ir.deltaTime = 0.0f;
        }

        ir.lastTime = now;
    }

    bool frame_end(Renderer renderer) {
        RYO_ENSURE(renderer_is_valid(renderer), false, "Invalid renderer handle");

        INDEX idx = renderer.index();
        _InternalRenderer& ir = _renderer_pool.items[idx];

        RYO_ASSERT(ir.swapChain, false, "Renderer has no swapchain");

        HRESULT hr = ir.swapChain->Present(1, 0);
        if (FAILED(hr)) {
            error_set("Swapchain present failed HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        return true;
    }

    float frame_get_delta_time(Renderer renderer) {
        RYO_ENSURE(renderer_is_valid(renderer), 0.0f, "Invalid renderer handle");

        INDEX idx = renderer.index();
        _InternalRenderer& ir = _renderer_pool.items[idx];
        return ir.deltaTime;
    }
}