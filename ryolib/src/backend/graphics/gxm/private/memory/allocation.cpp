#include "../private.h"

#include <psp2/kernel/sysmem.h>

namespace ryo {
    bool _memory_alloc(const char* name, size_t size, _MemoryType type, void*& mem, SceUID& memId) {
        SceKernelMemBlockType memBlockType = SCE_KERNEL_MEMBLOCK_TYPE_USER_RW;

        if (type == _MEMORY_TYPE_CDRAM)
            memBlockType = SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW;
        else if (type == _MEMORY_TYPE_UNCACHE)
            memBlockType = SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE;

        memId = sceKernelAllocMemBlock(
            name,
            memBlockType,
            size,
            nullptr
        );

        if (memId < 0) {
            error_set("Failed to allocate memory block %d", memId);
            return false;
        }

        int result = sceKernelGetMemBlockBase(memId, &mem);
        if (result < 0) {
            error_set("Failed to get memory block base %d", result);
            return false;
        }

        return true;
    }

    bool _memory_alloc_gpu_rw(const char* name, size_t size, _MemoryType type, void*& mem, SceUID& memId) {
        size_t alignedSize = (type == _MEMORY_TYPE_MAIN || type == _MEMORY_TYPE_UNCACHE) ? _memory_align_main(size) : _memory_align_cdram(size);

        if (!_memory_alloc(name, alignedSize, type, mem, memId))
            return false;

        SceGxmMemoryAttribFlags flags = static_cast<SceGxmMemoryAttribFlags>(SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE);

        int result = sceGxmMapMemory(mem, alignedSize, flags);
        if (result < 0) {
            error_set("Failed to map gxm memory %d", result);
            return false;
        }

        return true;
    }

    bool _memory_alloc_gpu_r(const char* name, size_t size, _MemoryType type, void*& mem, SceUID& memId) {
        size_t alignedSize = (type == _MEMORY_TYPE_MAIN || type == _MEMORY_TYPE_UNCACHE) ? _memory_align_main(size) : _memory_align_cdram(size);

        if (!_memory_alloc(name, alignedSize, type, mem, memId))
            return false;

        SceGxmMemoryAttribFlags flags = static_cast<SceGxmMemoryAttribFlags>(SCE_GXM_MEMORY_ATTRIB_READ);

        int result = sceGxmMapMemory(mem, alignedSize, flags);
        if (result < 0) {
            error_set("Failed to map gxm memory %d", result);
            return false;
        }

        return true;
    }

    bool _memory_alloc_usse(const char* name, size_t size, _USSEMemoryType type, void*& mem, SceUID& memId, unsigned int& offset) {
        size_t alignedSize = _memory_align_main(size);

        if (!_memory_alloc(name, alignedSize, _MEMORY_TYPE_MAIN, mem, memId))
            return false;

        int result = 0;

        switch (type) {
            case _USSE_MEMORY_TYPE_VERTEX:
                result = sceGxmMapVertexUsseMemory(mem, alignedSize, &offset);
                if (result < 0) {
                    error_set("Failed to map vertex usse memory %d", result);
                    return false;
                }
                break;

            case _USSE_MEMORY_TYPE_FRAGMENT:
                result = sceGxmMapFragmentUsseMemory(mem, alignedSize, &offset);
                if (result < 0) {
                    error_set("Failed to map fragment usse memory %d", result);
                    return false;
                }
                break;
        }

        return true;
    }
}