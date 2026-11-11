#include "../private.h"

namespace ryo {
    static bool _create_display_buffers(InternalRenderer* intern, _DisplayBuffer& buffer) {
        const size_t size = 4 * intern->stride * intern->height;
        return _memory_alloc_gpu_rw("ryo_display", size, _MEMORY_TYPE_CDRAM, buffer.data, buffer.memId);
    }

    bool _initialize_display_buffers(InternalRenderer* intern) {
        for (int i = 0; i < 3; ++i) {
            if (!_create_display_buffers(intern, intern->displayBuffers[i])) {
                error_set("Failed to create display framebuffer %d", i);
                return false;
            }

            _DisplayBuffer& buffer = intern->displayBuffers[i];

            if (sceGxmColorSurfaceInit(
                &buffer.surface,
                SCE_GXM_COLOR_FORMAT_A8B8G8R8,
                SCE_GXM_COLOR_SURFACE_LINEAR,
                SCE_GXM_COLOR_SURFACE_SCALE_NONE,
                SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
                intern->width,
                intern->height,
                intern->stride,
                buffer.data) < 0
            ) {
                error_set("Failed to initialize display framebuffer surface %d", i);
                return false;
            }

            if (sceGxmSyncObjectCreate(&buffer.sync) < 0) {
                error_set("Failed to create sync object for display framebuffer %d", i);
                return false;
            }
        }

        return true;
    }
}