#include "../../private.h"

#include <psp2/gxm.h>
#include <psp2/kernel/processmgr.h>
#include <chrono>

namespace ryo {
    void Renderer::frame_begin() {
        RYO_ENSURE_VOID(valid(), "Invalid renderer");
        RYO_ENSURE_VOID(!internal->inFrame, "Renderer already in a frame");

        SceUInt64 now = sceKernelGetProcessTimeWide();
        if (internal->lastTime != 0) {
            internal->deltaTime = static_cast<float>(now - internal->lastTime) / 1000000.0f;
        } else {
            internal->deltaTime = 1.0f / 60.0f;
        }
        internal->lastTime = now;

        _DisplayBuffer& buffer = internal->displayBuffers[internal->backBuffer];
        sceGxmPadHeartbeat(&buffer.surface, buffer.sync);

        internal->inFrame = true;
    }

    bool Renderer::frame_end() {
        RYO_ENSURE(valid(), false, "Invalid renderer");
        RYO_ENSURE(internal->inFrame, false, "Renderer is not in a frame");

        _DisplayBuffer& front = internal->displayBuffers[internal->frontBuffer];
        _DisplayBuffer& back = internal->displayBuffers[internal->backBuffer];

        if (sceGxmDisplayQueueAddEntry(front.sync, back.sync, &back.data) < 0) {
            error_set("Failed to add display queue entry");
            return false;
        }

        internal->frontBuffer = internal->backBuffer;
        internal->backBuffer = (internal->backBuffer + 1) % 3;

        internal->inFrame = false;

        return true;
    }

    float Renderer::frame_time_get() const {
        RYO_ENSURE(valid(), 0.0f, "Invalid renderer");

        return internal->deltaTime;
    }
}