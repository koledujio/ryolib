#include "../../private.h"

#include <psp2/gxm.h>
#include <psp2/display.h>
#include <psp2/kernel/sysmem.h>

#include <stdlib.h>

#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 544
#define DISPLAY_STRIDE_IN_PIXELS 1024

namespace ryo {
    static bool g_gxmInitialized = false;
    static void _renderer_destroy(InternalRenderer*& intern) {
        if (!intern)
            return;

        if (intern->defaultColorPipeline) {
            delete intern->defaultColorPipeline;
            intern->defaultColorPipeline = nullptr;
        }

        if (intern->defaultTexturePipeline) {
            delete intern->defaultTexturePipeline;
            intern->defaultTexturePipeline = nullptr;
        }

        if (intern->context)
            sceGxmFinish(intern->context);

        if (intern->shaderPatcher) {
            sceGxmShaderPatcherDestroy(intern->shaderPatcher);
            intern->shaderPatcher = nullptr;
        }

        if (intern->context) {
            sceDisplayWaitVblankStart();

            sceGxmDestroyContext(intern->context);
            intern->context = nullptr;
        }

        for (_DisplayBuffer& buffer : intern->displayBuffers) {
            if (buffer.sync) {
                sceGxmSyncObjectDestroy(buffer.sync);
                buffer.sync = nullptr;
            }

            _memory_free_gpu(buffer.data, buffer.memId);
            buffer.data = nullptr;
            buffer.memId = -1;
        }

        _memory_free(intern->contextHostMemId);
        intern->contextHostMemId = -1;
        intern->contextHostMem = nullptr;

        _memory_free_gpu(intern->vdmRingBuffer, intern->vdmRingBufferId);
        intern->vdmRingBuffer = nullptr;
        intern->vdmRingBufferId = -1;

        _memory_free_gpu(intern->vertexRingBuffer, intern->vertexRingBufferId);
        intern->vertexRingBuffer = nullptr;
        intern->vertexRingBufferId = -1;

        _memory_free_gpu(intern->fragmentRingBuffer, intern->fragmentRingBufferId);
        intern->fragmentRingBuffer = nullptr;
        intern->fragmentRingBufferId = -1;

        _memory_free_usse(_USSE_MEMORY_TYPE_FRAGMENT, intern->fragmentUsseRingBuffer, intern->fragmentUsseRingBufferId);
        intern->fragmentUsseRingBuffer = nullptr;
        intern->fragmentUsseRingBufferId = -1;
        intern->fragmentUsseRingOffset = 0;

        _memory_free_gpu(intern->shaderPatcherBuffer, intern->shaderPatcherBufferId);
        intern->shaderPatcherBuffer = nullptr;
        intern->shaderPatcherBufferId = -1;

        _memory_free_usse(_USSE_MEMORY_TYPE_VERTEX, intern->vertexUsse, intern->vertexUsseId);
        intern->vertexUsse = nullptr;
        intern->vertexUsseId = -1;
        intern->vertexUsseOffset = 0;

        _memory_free_usse(_USSE_MEMORY_TYPE_FRAGMENT, intern->fragmentUsse, intern->fragmentUsseId);
        intern->fragmentUsse = nullptr;
        intern->fragmentUsseId = -1;
        intern->fragmentUsseOffset = 0;

        if (g_gxmInitialized) {
            sceGxmTerminate();
            g_gxmInitialized = false;
        }

        delete intern;
        intern = nullptr;
    }

    void display_callback(const void* callback_data) {
        SceDisplayFrameBuf framebuf {};
        framebuf.size = sizeof(SceDisplayFrameBuf);
        framebuf.base = *((void **)callback_data);
        framebuf.pitch = DISPLAY_STRIDE_IN_PIXELS;
        framebuf.pixelformat = SCE_DISPLAY_PIXELFORMAT_A8B8G8R8;
        framebuf.width = DISPLAY_WIDTH;
        framebuf.height = DISPLAY_HEIGHT;

        sceDisplaySetFrameBuf(&framebuf, SCE_DISPLAY_SETBUF_NEXTFRAME);
    }

    static bool _create_display_buffers(InternalRenderer* intern, _DisplayBuffer& buffer) {
        const size_t size = 4 * intern->stride * intern->height;
        return _memory_alloc_gpu_rw("ryo_display", size, _MEMORY_TYPE_CDRAM, buffer.data, buffer.memId);
    }

    Renderer::Renderer(const RendererConfig& config) {
        if (g_gxmInitialized) {
            error_set("Cannot create more than one renderer on the PS Vita");
            return;
        }

        // TODO: Validate config values(as in width and height, nwh is ignored, but can be checked to be nullptr)

        // Allocate InternalRenderer
        InternalRenderer* intern = new InternalRenderer();
        intern->width = config.width;
        intern->height = config.height;
        intern->stride = 1024;

        // Initialize GXM
        SceGxmInitializeParams params = {};
        params.flags = 0;
        params.displayQueueMaxPendingCount = 2; // use 2 for barely better performance later
        params.displayQueueCallback = display_callback; // display_callback contains stuff like which framebuffer to show
        params.displayQueueCallbackDataSize = sizeof(void *);
        params.parameterBufferSize = SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE;

        SceInt32 result = sceGxmInitialize(&params);

        if (result < 0) {
            delete intern;
            error_set("Failed to initialize GXM: 0x%08X", result);
            return;
        }
        g_gxmInitialized = true;
        
        // Allocate display framebuffer memory
        // Create sync objects
        // Initialize color surfaces
        for (int i = 0; i < 3; ++i) {
            if (!_create_display_buffers(intern, intern->displayBuffers[i])) {
                _renderer_destroy(intern);
                error_set("Failed to create display framebuffer %d", i);
                return;
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
                _renderer_destroy(intern);
                error_set("Failed to initialize display framebuffer surface %d", i);
                return;
            }

            if (sceGxmSyncObjectCreate(&buffer.sync) < 0) {
                _renderer_destroy(intern);
                error_set("Failed to create sync object for display framebuffer %d", i);
                return;
            }
        }

        // Create GXM context
        constexpr size_t HOST_MEM_SIZE = 16 * 1024;
        constexpr size_t VDM_RING_BUFFER_SIZE = 64 * 1024;
        constexpr size_t VERTEX_RING_BUFFER_SIZE = 256 * 1024;
        constexpr size_t FRAGMENT_RING_BUFFER_SIZE = 256 * 1024;
        constexpr size_t FRAGMENT_USSE_SIZE = 64 * 1024;

        // Host memory (CPU only)
        if (!_memory_alloc("ryo_gxm_context", HOST_MEM_SIZE, _MEMORY_TYPE_MAIN, intern->contextHostMem, intern->contextHostMemId)) {
            _renderer_destroy(intern);
            error_set("Failed to allocate GXM context memory");
            return;
        }

        // GPU ring buffers
        if (!_memory_alloc_gpu_rw("ryo_vdm_ring", VDM_RING_BUFFER_SIZE, _MEMORY_TYPE_MAIN, intern->vdmRingBuffer, intern->vdmRingBufferId)) {
            _renderer_destroy(intern);
            error_set("Failed to allocate VDM ring buffer");
            return;
        }
        if (!_memory_alloc_gpu_rw("ryo_vertex_ring", VERTEX_RING_BUFFER_SIZE, _MEMORY_TYPE_MAIN, intern->vertexRingBuffer, intern->vertexRingBufferId)) {
            _renderer_destroy(intern);
            error_set("Failed to allocate vertex ring buffer");
            return;
        }
        if (!_memory_alloc_gpu_rw("ryo_fragment_ring", FRAGMENT_RING_BUFFER_SIZE, _MEMORY_TYPE_MAIN, intern->fragmentRingBuffer, intern->fragmentRingBufferId)) {
            _renderer_destroy(intern);
            error_set("Failed to allocate fragment ring buffer");
            return;
        }

        // Fragment USSE
        if (!_memory_alloc_usse("ryo_fragment_ring_usse", FRAGMENT_USSE_SIZE, _USSE_MEMORY_TYPE_FRAGMENT, intern->fragmentUsseRingBuffer, intern->fragmentUsseRingBufferId, intern->fragmentUsseRingOffset)) {
            _renderer_destroy(intern);
            error_set("Failed to allocate fragment USSE ring buffer");
            return;
        }

        SceGxmContextParams contextParams = {};

        contextParams.hostMem = intern->contextHostMem;
        contextParams.hostMemSize = HOST_MEM_SIZE;

        contextParams.vdmRingBufferMem = intern->vdmRingBuffer;
        contextParams.vdmRingBufferMemSize = VDM_RING_BUFFER_SIZE;

        contextParams.vertexRingBufferMem = intern->vertexRingBuffer;
        contextParams.vertexRingBufferMemSize = VERTEX_RING_BUFFER_SIZE;

        contextParams.fragmentRingBufferMem = intern->fragmentRingBuffer;
        contextParams.fragmentRingBufferMemSize = FRAGMENT_RING_BUFFER_SIZE;

        contextParams.fragmentUsseRingBufferMem = intern->fragmentUsseRingBuffer;
        contextParams.fragmentUsseRingBufferMemSize = FRAGMENT_USSE_SIZE;
        contextParams.fragmentUsseRingBufferOffset = intern->fragmentUsseRingOffset;

        if (sceGxmCreateContext(&contextParams, &intern->context) < 0) {
            _renderer_destroy(intern);
            error_set("Failed to create GXM context");
            return;
        }

        // Shader patcher
        constexpr size_t PATCHER_BUFFER_SIZE = 64 * 1024;
        constexpr size_t PATCHER_VERTEX_USSE_SIZE = 64 * 1024;
        constexpr size_t PATCHER_FRAGMENT_USSE_SIZE = 64 * 1024;

        if (!_memory_alloc_gpu_rw("ryo_shader_patcher", PATCHER_BUFFER_SIZE, _MEMORY_TYPE_MAIN, intern->shaderPatcherBuffer, intern->shaderPatcherBufferId)) {
            _renderer_destroy(intern);
            error_set("Failed to allocate shader patcher buffer");
            return;
        }
        if (!_memory_alloc_usse("ryo_vertex_usse", PATCHER_VERTEX_USSE_SIZE, _USSE_MEMORY_TYPE_VERTEX, intern->vertexUsse, intern->vertexUsseId, intern->vertexUsseOffset)) {
            _renderer_destroy(intern);
            error_set("Failed to allocate vertex USSE buffer");
            return;
        }
        if (!_memory_alloc_usse("ryo_fragment_usse", PATCHER_FRAGMENT_USSE_SIZE, _USSE_MEMORY_TYPE_FRAGMENT, intern->fragmentUsse, intern->fragmentUsseId, intern->fragmentUsseOffset)) {
            _renderer_destroy(intern);
            error_set("Failed to allocate fragment USSE buffer");
            return;
        }

        SceGxmShaderPatcherParams patcherParams = {};
        patcherParams.userData = nullptr;

        patcherParams.hostAllocCallback = _memory_patcher_alloc;
        patcherParams.hostFreeCallback = _memory_patcher_free;

        // patcherParams.bufferAllocCallback = _memory_patcher_buffer_alloc;
        // patcherParams.bufferFreeCallback = _memory_patcher_buffer_free;
        patcherParams.bufferMem = intern->shaderPatcherBuffer;
        patcherParams.bufferMemSize = PATCHER_BUFFER_SIZE;

        // patcherParams.vertexUsseAllocCallback = _memory_patcher_vertex_usse_alloc;
        // patcherParams.vertexUsseFreeCallback = _memory_patcher_vertex_usse_free;
        patcherParams.vertexUsseMem = intern->vertexUsse;
        patcherParams.vertexUsseMemSize = PATCHER_VERTEX_USSE_SIZE;
        patcherParams.vertexUsseOffset = intern->vertexUsseOffset;

        // patcherParams.fragmentUsseAllocCallback = _memory_patcher_fragment_usse_alloc;
        // patcherParams.fragmentUsseFreeCallback = _memory_patcher_fragment_usse_free;
        patcherParams.fragmentUsseMem = intern->fragmentUsse;
        patcherParams.fragmentUsseMemSize = PATCHER_FRAGMENT_USSE_SIZE;
        patcherParams.fragmentUsseOffset = intern->fragmentUsseOffset;

        
        int shaderPatcherCreateResult = sceGxmShaderPatcherCreate(&patcherParams, &intern->shaderPatcher);
        if (shaderPatcherCreateResult < 0) {
            _renderer_destroy(intern);
            error_set("Failed to create shader patcher %d", shaderPatcherCreateResult);
            return;
        }

        internal = intern;

        PipelineDesc defaultColorPipelineDesc = {};
        defaultColorPipelineDesc.shader = BUILTIN_SHADER_COLOR;
        defaultColorPipelineDesc.vertexLayout = VERTEX_LAYOUT_POSITION;
        defaultColorPipelineDesc.blendMode = BLEND_MODE_OPAQUE;
        internal->defaultColorPipeline = new Pipeline(*this, defaultColorPipelineDesc);
        if (!internal->defaultColorPipeline->valid()) {
            RYO_ASSERT_VOID(false, "Failed to create default color pipeline");
            delete internal->defaultColorPipeline;
            internal->defaultColorPipeline = nullptr;
        }

        PipelineDesc defaultTexturePipelineDesc = {};
        defaultTexturePipelineDesc.shader = BUILTIN_SHADER_TEXTURE;
        defaultTexturePipelineDesc.vertexLayout = VERTEX_LAYOUT_POSITION_UV;
        defaultTexturePipelineDesc.blendMode = BLEND_MODE_OPAQUE;
        internal->defaultTexturePipeline = new Pipeline(*this, defaultTexturePipelineDesc);
        if (!internal->defaultTexturePipeline->valid()) {
            RYO_ASSERT_VOID(false, "Failed to create default texture pipeline");
            delete internal->defaultTexturePipeline;
            internal->defaultTexturePipeline = nullptr;
        }

        static const PositionVertex quadVertices[] = {
            {-0.5f, -0.5f, 0.0f},
            { 0.5f, -0.5f, 0.0f},
            {-0.5f,  0.5f, 0.0f},
            { 0.5f,  0.5f, 0.0f}
        };

        static const uint32_t quadIndices[] = {
            0, 1, 2,
            2, 1, 3
        };

        Buffer* quadVertexBuffer = new Buffer(*this, quadVertices, sizeof(quadVertices), BUFFER_TYPE_VERTEX);
        if (!quadVertexBuffer->valid()) {
            RYO_ASSERT_VOID(false, "Failed to create quad vertex buffer");
            delete quadVertexBuffer;
        }
        internal->quadVertexBuffer = quadVertexBuffer;

        Buffer* quadIndexBuffer = new Buffer(*this, quadIndices, sizeof(quadIndices), BUFFER_TYPE_INDEX);
        if (!quadIndexBuffer->valid()) {
            RYO_ASSERT_VOID(false, "Failed to create quad index buffer");
            delete quadIndexBuffer;
        }
        internal->quadIndexBuffer = quadIndexBuffer;
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