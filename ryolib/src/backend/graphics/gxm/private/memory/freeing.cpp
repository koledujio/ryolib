#include "../private.h"

#include <psp2/kernel/sysmem.h>

namespace ryo {
    void _memory_free(SceUID memId) {
        if (memId < 0)
            sceKernelFreeMemBlock(memId);
    }

    void _memory_free_gpu(void* mem, SceUID memId) {
        if (mem)
            sceGxmUnmapMemory(mem);

        _memory_free(memId);
    }

    void _memory_free_usse(_USSEMemoryType type, void* mem, SceUID memId) {
        if (mem)
            switch (type) {
                case _USSE_MEMORY_TYPE_FRAGMENT:
                    sceGxmUnmapFragmentUsseMemory(mem);
                    break;
                
                case _USSE_MEMORY_TYPE_VERTEX:
                    sceGxmUnmapVertexUsseMemory(mem);
                    break;
            }
        
        _memory_free(memId);
    }
}