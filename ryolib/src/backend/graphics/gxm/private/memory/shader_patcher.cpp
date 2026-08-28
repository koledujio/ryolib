#include "../private.h"

#include <stdlib.h>
#include <unordered_map>

namespace ryo {
    static std::unordered_map<void*, SceUID> g_patcher_memory_registry;

    void* _memory_patcher_alloc(void* userdata, SceSize size) {
        return malloc(size);
    }

    void _memory_patcher_free(void* userdata, void* mem) {
        free(mem);
    }

    void* _memory_patcher_buffer_alloc(void* userdata, SceSize size) {
        size_t alignedSize = _memory_align_main(size);

        void* mem = nullptr;
        SceUID memId = -1;

        if (!_memory_alloc("gxm_shader_patcher_host_mem", alignedSize, _MEMORY_TYPE_MAIN, mem, memId))
            return nullptr;

        g_patcher_memory_registry[mem] = memId;

        return mem;
    }

    void _memory_patcher_buffer_free(void* userdata, void* mem) {
        if (!mem)
            return;

        auto it = g_patcher_memory_registry.find(mem);
        if (it != g_patcher_memory_registry.end()) {
            SceUID memId = it->second;

            _memory_free(memId);

            g_patcher_memory_registry.erase(it);
        }
    }

    void* _memory_patcher_vertex_usse_alloc(void* userdata, SceSize size, unsigned int* usseOffset) {
        size_t alignedSize = _memory_align_main(size);

        void* mem = nullptr;
        SceUID memId = -1;
        unsigned int offset = 0;

        if (!_memory_alloc_usse("gxm_shader_patcher_vertex_usse_mem", alignedSize, _USSE_MEMORY_TYPE_VERTEX, mem, memId, offset))
            return nullptr;

        g_patcher_memory_registry[mem] = memId;

        if (usseOffset)
            *usseOffset = offset;

        return mem;
    }

    void _memory_patcher_vertex_usse_free(void* userdata, void* mem) {
        if (!mem)
            return;

        auto it = g_patcher_memory_registry.find(mem);
        if (it != g_patcher_memory_registry.end()) {
            SceUID memId = it->second;

            _memory_free_usse(_USSE_MEMORY_TYPE_VERTEX, mem, memId);

            g_patcher_memory_registry.erase(it);
        }
    }

    void* _memory_patcher_fragment_usse_alloc(void* userdata, SceSize size, unsigned int* usseOffset) {
        size_t alignedSize = _memory_align_main(size);

        void* mem = nullptr;
        SceUID memId = -1;
        unsigned int offset = 0;

        if (!_memory_alloc_usse("gxm_shader_patcher_fragment_usse_mem", alignedSize, _USSE_MEMORY_TYPE_FRAGMENT, mem, memId, offset))
            return nullptr;

        g_patcher_memory_registry[mem] = memId;

        if (usseOffset)
            *usseOffset = offset;

        return mem;
    }

    void _memory_patcher_fragment_usse_free(void* userdata, void* mem) {
        if (!mem)
            return;

        auto it = g_patcher_memory_registry.find(mem);
        if (it != g_patcher_memory_registry.end()) {
            SceUID memId = it->second;

            _memory_free_usse(_USSE_MEMORY_TYPE_FRAGMENT, mem, memId);

            g_patcher_memory_registry.erase(it);
        }
    }
}