#include "../private.h"

namespace ryo {
    bool _create_shader_patcher(InternalRenderer* intern) {
        constexpr size_t PATCHER_BUFFER_SIZE = 64 * 1024;
        constexpr size_t PATCHER_VERTEX_USSE_SIZE = 64 * 1024;
        constexpr size_t PATCHER_FRAGMENT_USSE_SIZE = 64 * 1024;

        if (!_memory_alloc_gpu_rw("ryo_shader_patcher", PATCHER_BUFFER_SIZE, _MEMORY_TYPE_MAIN, intern->shaderPatcherBuffer, intern->shaderPatcherBufferId)) {
            error_set("Failed to allocate shader patcher buffer");
            return false;
        }
        if (!_memory_alloc_usse("ryo_vertex_usse", PATCHER_VERTEX_USSE_SIZE, _USSE_MEMORY_TYPE_VERTEX, intern->vertexUsse, intern->vertexUsseId, intern->vertexUsseOffset)) {
            error_set("Failed to allocate vertex USSE buffer");
            return false;
        }
        if (!_memory_alloc_usse("ryo_fragment_usse", PATCHER_FRAGMENT_USSE_SIZE, _USSE_MEMORY_TYPE_FRAGMENT, intern->fragmentUsse, intern->fragmentUsseId, intern->fragmentUsseOffset)) {
            error_set("Failed to allocate fragment USSE buffer");
            return false;
        }

        SceGxmShaderPatcherParams patcherParams = {};
        patcherParams.userData = nullptr;

        patcherParams.hostAllocCallback = _memory_patcher_alloc;
        patcherParams.hostFreeCallback = _memory_patcher_free;

        patcherParams.bufferAllocCallback = nullptr;
        patcherParams.bufferFreeCallback = nullptr;
        patcherParams.bufferMem = intern->shaderPatcherBuffer;
        patcherParams.bufferMemSize = PATCHER_BUFFER_SIZE;

        patcherParams.vertexUsseAllocCallback = nullptr;
        patcherParams.vertexUsseFreeCallback = nullptr;
        patcherParams.vertexUsseMem = intern->vertexUsse;
        patcherParams.vertexUsseMemSize = PATCHER_VERTEX_USSE_SIZE;
        patcherParams.vertexUsseOffset = intern->vertexUsseOffset;

        patcherParams.fragmentUsseAllocCallback = nullptr;
        patcherParams.fragmentUsseFreeCallback = nullptr;
        patcherParams.fragmentUsseMem = intern->fragmentUsse;
        patcherParams.fragmentUsseMemSize = PATCHER_FRAGMENT_USSE_SIZE;
        patcherParams.fragmentUsseOffset = intern->fragmentUsseOffset;

        int shaderPatcherCreateResult = sceGxmShaderPatcherCreate(&patcherParams, &intern->shaderPatcher);
        if (shaderPatcherCreateResult < 0) {
            error_set("Failed to create shader patcher %d", shaderPatcherCreateResult);
            return false;
        }

        return true;
    }
}