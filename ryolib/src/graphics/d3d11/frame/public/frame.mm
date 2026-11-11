#include "../../private.h"

#include <chrono>

@implementation RYD3D11GraphicsBackend (Frame)

- (void)frameBegin {
    if (!_internal) return;

    RYInternalRenderer* intern = (RYInternalRenderer*)_internal;
    if (intern->inFrame) return;

    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    if (intern->lastTime.time_since_epoch().count() != 0) {
        std::chrono::duration<float> elapsed = now - intern->lastTime;
        intern->deltaTime = elapsed.count();
    } else {
        intern->deltaTime = 0.0f;
    }

    intern->lastTime = now;
    intern->inFrame = true;
}

- (BOOL)frameEnd {
    if (!_internal) return NO;

    RYInternalRenderer* intern = (RYInternalRenderer*)_internal;
    if (!intern->inFrame) return NO;
    if (!intern->swapChain) return NO;

    HRESULT hr = intern->swapChain->Present(1, 0);
    if (FAILED(hr)) return NO;

    intern->inFrame = false;
    return YES;
}

- (float)frameTime {
    if (!_internal) return 0.0f;

    RYInternalRenderer* intern = (RYInternalRenderer*)_internal;
    return intern->deltaTime;
}

@end
