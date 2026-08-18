#include "../../private.h"

#include <CoreFoundation/CFDate.h>
#include <QuartzCore/CABase.h>

namespace ryo {
    void frame_begin(Renderer renderer) {
        RYO_ENSURE_VOID(renderer_is_valid(renderer), "Invalid renderer handle");
        
        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];

        if (ir.autoreleasePool) {
            [ir.autoreleasePool drain];
            ir.autoreleasePool = nil;
        }

        ir.autoreleasePool = [[NSAutoreleasePool alloc] init];
        ir.currentCommandBuffer = [ir.queue commandBuffer];
        ir.currentDrawable = [ir.metalLayer nextDrawable];

        if (!ir.currentCommandBuffer || !ir.currentDrawable) {
            ir.currentCommandBuffer = nil;
            ir.currentDrawable = nil;
        }

        CFTimeInterval now = CACurrentMediaTime();
        if (ir.lastTime > 0.0) {
            ir.deltaTime = (float)(now - ir.lastTime);
        }
        ir.lastTime = now;
    }

    bool frame_end(Renderer renderer) {
        RYO_ENSURE(renderer_is_valid(renderer), false, "Invalid renderer handle");

        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];

        if (ir.currentCommandBuffer && ir.currentDrawable) {
            [ir.currentCommandBuffer presentDrawable:ir.currentDrawable];
            [ir.currentCommandBuffer commit];
        }
        
        ir.currentCommandBuffer = nil;
        ir.currentDrawable = nil;

        if (ir.autoreleasePool) {
            [ir.autoreleasePool drain];
            ir.autoreleasePool = nil;
        }

        return true;
    }

    float frame_get_delta_time(Renderer renderer) {
        RYO_ENSURE(renderer_is_valid(renderer), 0.0f, "Invalid renderer handle");

        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];

        return ir.deltaTime;
    }
}
