#pragma once

#import "ryolib/graphics/RYGraphicsBackend.h"

/**
 * @brief Stub graphics backend.
 *
 * Present on every platform so the renderer always has a backend class to
 * fall back to. It cannot create a renderer; the renderer factories report a
 * feature-not-supported error instead.
 */
@interface RYStubGraphicsBackend : RYGraphicsBackend
@end
