#include "../../private.h"

#include <psp2/gxm.h>

namespace ryo {
    void Renderer::frame_begin() {
        RYO_ENSURE_VOID(valid(), "Invalid renderer");

        _DisplayBuffer& buffer = internal->displayBuffers[internal->backBuffer];

        sceGxmPadHeartbeat(&buffer.surface, buffer.sync);

        uint32_t* pixels = static_cast<uint32_t*>(buffer.data);

        for (unsigned int y = 0; y < internal->height; ++y) {
            for (unsigned int x = 0; x < internal->width; ++x) {
                pixels[y * internal->stride + x] = 0x00FF00FF;
            }
        }
    }

    bool Renderer::frame_end() {
        RYO_ENSURE(valid(), false, "Invalid renderer");

        _DisplayBuffer& front = internal->displayBuffers[internal->frontBuffer];
        _DisplayBuffer& back = internal->displayBuffers[internal->backBuffer];

        if (sceGxmDisplayQueueAddEntry(front.sync, back.sync, &back.data) < 0) {
            error_set("Failed to add display queue entry");
            return false;
        }

        internal->frontBuffer = internal->backBuffer;
        internal->backBuffer = (internal->backBuffer + 1) % 3;

        return true;
    }

    float Renderer::frame_time_get() const {return 0.0f;}
}