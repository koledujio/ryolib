#include "../../private.h"

#include <CoreFoundation/CFDate.h>
#include <QuartzCore/CABase.h>

namespace ryo {
    void Renderer::frame_begin() {
        RYO_ENSURE_VOID(valid(), "Invalid renderer");
        RYO_ENSURE_VOID(!internal->inFrame, "Renderer already in a frame");

        if (internal->autoreleasePool) {
            [internal->autoreleasePool drain];
            internal->autoreleasePool = nil;
        }

        internal->autoreleasePool = [[NSAutoreleasePool alloc] init];
        internal->currentCommandBuffer = [internal->queue commandBuffer];
        internal->currentDrawable = [internal->metalLayer nextDrawable];

        if (!internal->currentCommandBuffer || !internal->currentDrawable) {
            internal->currentCommandBuffer = nil;
            internal->currentDrawable = nil;
        }

        CFTimeInterval now = CACurrentMediaTime();
        if (internal->lastTime > 0.0) {
            internal->deltaTime = (float)(now - internal->lastTime);
        }
        internal->lastTime = now;

        internal->inFrame = true;
    }

    bool Renderer::frame_end() {
        RYO_ENSURE(valid(), false, "Invalid renderer");
        RYO_ENSURE(internal->inFrame, false, "Renderer is not in a frame");

        if (internal->currentCommandBuffer && internal->currentDrawable) {
            [internal->currentCommandBuffer presentDrawable:internal->currentDrawable];
            [internal->currentCommandBuffer commit];
        }
        
        internal->currentCommandBuffer = nil;
        internal->currentDrawable = nil;

        if (internal->autoreleasePool) {
            [internal->autoreleasePool drain];
            internal->autoreleasePool = nil;
        }

        internal->inFrame = false;

        return true;
    }

    float Renderer::frame_time_get() const {
        RYO_ENSURE(valid(), 0.0f, "Invalid renderer");
        return internal->deltaTime;
    }
}
