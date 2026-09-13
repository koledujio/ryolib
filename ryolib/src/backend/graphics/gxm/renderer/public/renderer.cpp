#include "../private.h"

#include <stdlib.h>
#include <cstdio>
#include <cstddef>
#include <cstdlib>

namespace ryo {
    bool g_gxmInitialized = false;

    Renderer::Renderer(const RendererConfig& config) {
        if (g_gxmInitialized) {
            error_set("Cannot create more than one renderer on the PS Vita");
            return;
        }

        InternalRenderer* intern = new InternalRenderer();
        intern->width = DISPLAY_WIDTH;
        intern->height = DISPLAY_HEIGHT;
        intern->stride = 1024;

        SceGxmInitializeParams params = {};
        params.flags = 0;
        params.displayQueueMaxPendingCount = 2;
        params.displayQueueCallback = _display_callback;
        params.displayQueueCallbackDataSize = sizeof(void *);
        params.parameterBufferSize = SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE;

        SceInt32 result = sceGxmInitialize(&params);

        if (result < 0) {
            delete intern;
            error_set("Failed to initialize GXM: 0x%08X", result);
            return;
        }
        g_gxmInitialized = true;
        
        if (!_initialize_display_buffers(intern) ||
            !_create_context(intern) ||
            !_create_shader_patcher(intern) ||
            !_create_default_render_target(intern) ||
            !_create_default_depth_stencil(intern)
        ) {
            _renderer_destroy(intern);
            return;
        }

        internal = intern;

        _create_default_pipelines(internal, *this);
        _create_default_buffers(internal, *this);
    }

    Renderer::Renderer(const RendererConfig& config, const Renderer& renderer) {
        error_set("Cannot create more than one renderer on the PS Vita");
    }

    Renderer::~Renderer() {
        _renderer_destroy(internal);
    }

    Renderer::Renderer(Renderer&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Renderer& Renderer::operator=(Renderer&& other) {
        if (this == &other)
            return *this;

        _renderer_destroy(internal);

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Renderer::valid() const {
        return (
            internal != nullptr &&

            internal->context != nullptr &&
            internal->contextHostMem != nullptr &&
            internal->contextHostMemId >= 0 &&

            internal->shaderPatcher != nullptr &&
            internal->shaderPatcherBuffer != nullptr &&
            internal->shaderPatcherBufferId >= 0 &&

            internal->displayBuffers[0].data != nullptr &&
            internal->displayBuffers[0].memId >= 0 &&
            internal->displayBuffers[0].sync != nullptr &&

            internal->displayBuffers[1].data != nullptr &&
            internal->displayBuffers[1].memId >= 0 &&
            internal->displayBuffers[1].sync != nullptr &&

            internal->displayBuffers[2].data != nullptr &&
            internal->displayBuffers[2].memId >= 0 &&
            internal->displayBuffers[2].sync != nullptr
        ); // TODO: Improve this later, but it's fine enough for now
    }

    bool Renderer::resize(unsigned int width, unsigned int height) {
        // TODO: figure out what to do with this function, for now nothing
        return true;
    }
}