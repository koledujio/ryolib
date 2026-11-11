#include "../private.h"

namespace ryo {
    bool _create_context(InternalRenderer* intern) {
        constexpr size_t HOST_MEM_SIZE = 16 * 1024;
        constexpr size_t VDM_RING_BUFFER_SIZE = SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE;
        constexpr size_t VERTEX_RING_BUFFER_SIZE = SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE;
        constexpr size_t FRAGMENT_RING_BUFFER_SIZE = SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE;
        constexpr size_t FRAGMENT_USSE_SIZE = SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE;

        // Host memory (CPU only)
        if (!_memory_alloc("ryo_gxm_context", HOST_MEM_SIZE, _MEMORY_TYPE_MAIN, intern->contextHostMem, intern->contextHostMemId)) {
            error_set("Failed to allocate GXM context memory");
            return false;
        }

        // GPU ring buffers
        if (!_memory_alloc_gpu_rw("ryo_vdm_ring", VDM_RING_BUFFER_SIZE, _MEMORY_TYPE_MAIN, intern->vdmRingBuffer, intern->vdmRingBufferId)) {
            error_set("Failed to allocate VDM ring buffer");
            return false;
        }
        if (!_memory_alloc_gpu_rw("ryo_vertex_ring", VERTEX_RING_BUFFER_SIZE, _MEMORY_TYPE_MAIN, intern->vertexRingBuffer, intern->vertexRingBufferId)) {
            error_set("Failed to allocate vertex ring buffer");
            return false;
        }
        if (!_memory_alloc_gpu_rw("ryo_fragment_ring", FRAGMENT_RING_BUFFER_SIZE, _MEMORY_TYPE_MAIN, intern->fragmentRingBuffer, intern->fragmentRingBufferId)) {
            error_set("Failed to allocate fragment ring buffer");
            return false;
        }

        // Fragment USSE
        if (!_memory_alloc_usse("ryo_fragment_ring_usse", FRAGMENT_USSE_SIZE, _USSE_MEMORY_TYPE_FRAGMENT, intern->fragmentUsseRingBuffer, intern->fragmentUsseRingBufferId, intern->fragmentUsseRingOffset)) {
            error_set("Failed to allocate fragment USSE ring buffer");
            return false;
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
            error_set("Failed to create GXM context");
            return false;
        }

        return true;
    }
}