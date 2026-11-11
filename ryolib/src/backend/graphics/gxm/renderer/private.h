#include "../private.h"

#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 544
#define DISPLAY_STRIDE_IN_PIXELS 1024

namespace ryo {
    extern bool g_gxmInitialized;

    void _renderer_destroy(InternalRenderer*& intern);

    void _display_callback(const void* callback_data);

    bool _initialize_display_buffers(InternalRenderer* intern);

    bool _create_context(InternalRenderer* intern);

    bool _create_shader_patcher(InternalRenderer* intern);

    bool _create_default_render_target(InternalRenderer* intern);

    bool _create_default_depth_stencil(InternalRenderer* intern);

    bool _create_default_pipelines(InternalRenderer* intern, Renderer& renderer);

    bool _create_default_buffers(InternalRenderer* intern, Renderer& renderer);
}