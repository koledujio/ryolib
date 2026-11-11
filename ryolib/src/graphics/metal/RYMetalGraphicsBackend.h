#pragma once

#import "ryolib/graphics/RYGraphicsBackend.h"

/**
 * @brief Metal graphics backend.
 *
 * Used on macOS and iOS. The renderer chooses it by default when the Metal
 * backend is compiled in.
 */
@interface RYMetalGraphicsBackend : RYGraphicsBackend
@end
