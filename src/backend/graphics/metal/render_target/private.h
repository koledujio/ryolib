#include "../private.h"

namespace ryo {
    id<MTLTexture> _render_target_create_color_texture(id<MTLDevice> device, int width, int height);
    id<MTLTexture> _render_target_create_depth_texture(id<MTLDevice> device, int width, int height);
}