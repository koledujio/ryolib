#include "../../private.h"

#include <chrono>

namespace ryo {
    void Renderer::frame_begin() {
        // optional safety flags like ir.inFrame can be added here
        // in d3d11, immediate commands execute directly so frame_begin is not needed as much

        RYO_ENSURE_VOID(valid(), "Invalid renderer");
        RYO_ENSURE_VOID(!internal->inFrame, "Renderer is in a frame already");

        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        if (internal->lastTime.time_since_epoch().count() != 0) {
            std::chrono::duration<float> elapsed = now - internal->lastTime;
            internal->deltaTime = elapsed.count();
        } else {
            internal->deltaTime = 0.0f;
        }

        internal->lastTime = now;

        internal->inFrame = true;
    }

    bool Renderer::frame_end() {
        RYO_ENSURE(valid(), false, "Invalid renderer");
        RYO_ENSURE(internal->inFrame, false, "Renderer is not in a frame");
        RYO_ASSERT(internal->swapChain, false, "Renderer has no swapchain");

        HRESULT hr = internal->swapChain->Present(1, 0);
        if (FAILED(hr)) {
            error_set("Swapchain present failed HRESULT: 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        internal->inFrame = false;

        return true;
    }

    float Renderer::frame_time_get() const {
        RYO_ENSURE(valid(), 0.0f, "Invalid renderer");

        return internal->deltaTime;
    }
}