#include "../private.h"

#include <psp2/kernel/sysmem.h>

namespace ryo {
    bool _memory_alloc(const char* name, size_t size, _MemoryType type, void*& mem, SceUID& memId) {
        memId = sceKernelAllocMemBlock(
            name,
            type == _MEMORY_TYPE_MAIN ? SCE_KERNEL_MEMBLOCK_TYPE_USER_RW : SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW,
            size,
            nullptr
        );

        if (memId < 0)
            return false;

        if (sceKernelGetMemBlockBase(memId, &mem) < 0)
            return false;

        return true;
    }

    bool _memory_alloc_gpu(const char* name, size_t size, _MemoryType type, void*& mem, SceUID& memId) {
        size_t alignedSize = type == _MEMORY_TYPE_MAIN ? _memory_align_main(size) : _memory_align_cdram(size);

        if (!_memory_alloc(name, alignedSize, type, mem, memId))
            return false;

        SceGxmMemoryAttribFlags flags = static_cast<SceGxmMemoryAttribFlags>(SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE);

        if (sceGxmMapMemory(mem, alignedSize, flags) < 0)
            return false;

        return true;
    }

    bool _memory_alloc_usse(const char* name, size_t size, _USSEMemoryType type, void*& mem, SceUID& memId, unsigned int& offset) {
        size_t alignedSize = _memory_align_main(size);

        if (!_memory_alloc(name, alignedSize, _MEMORY_TYPE_MAIN, mem, memId))
            return false;

        switch (type) {
            case _USSE_MEMORY_TYPE_VERTEX:
                if (sceGxmMapVertexUsseMemory(mem, alignedSize, &offset) < 0)
                    return false;
                break;

            case _USSE_MEMORY_TYPE_FRAGMENT:
                if (sceGxmMapFragmentUsseMemory(mem, alignedSize, &offset) < 0)
                    return false;
                break;
        }

        return true;
    }
}