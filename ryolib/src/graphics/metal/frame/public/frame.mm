#include "../../private.h"

#include <CoreFoundation/CFDate.h>
#include <QuartzCore/CABase.h>

@implementation RYMetalGraphicsBackend (Frame)

- (void)frameBegin {
    if (!_internal) return;

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    if (intern->inFrame) return;

    _renderer_sync_layer_size(*intern);

    if (intern->autoreleasePool) {
        [intern->autoreleasePool drain];
        intern->autoreleasePool = nil;
    }

    intern->autoreleasePool = [[NSAutoreleasePool alloc] init];
    intern->currentCommandBuffer = [intern->queue commandBuffer];
    intern->currentDrawable = [intern->metalLayer nextDrawable];

    if (!intern->currentCommandBuffer || !intern->currentDrawable) {
        intern->currentCommandBuffer = nil;
        intern->currentDrawable = nil;
    }

    CFTimeInterval now = CACurrentMediaTime();
    if (intern->lastTime > 0.0) {
        intern->deltaTime = (float)(now - intern->lastTime);
    }
    intern->lastTime = now;

    intern->inFrame = true;
}

- (BOOL)frameEnd {
    if (!_internal) return NO;

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    if (!intern->inFrame) return NO;

    if (intern->currentCommandBuffer && intern->currentDrawable) {
        [intern->currentCommandBuffer presentDrawable:intern->currentDrawable];
        [intern->currentCommandBuffer commit];
    }

    intern->currentCommandBuffer = nil;
    intern->currentDrawable = nil;

    if (intern->autoreleasePool) {
        [intern->autoreleasePool drain];
        intern->autoreleasePool = nil;
    }

    intern->inFrame = false;

    return YES;
}

- (float)frameTime {
    if (!_internal) return 0.0f;

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    return intern->deltaTime;
}

@end
