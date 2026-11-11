#pragma once

#import "ryolib/graphics/RYGraphicsBackend.h"

/**
 * @brief Direct3D 11 graphics backend.
 *
 * Used on Windows. The renderer chooses it by default when the D3D11 backend
 * is compiled in.
 */
@interface RYD3D11GraphicsBackend : RYGraphicsBackend
@end
