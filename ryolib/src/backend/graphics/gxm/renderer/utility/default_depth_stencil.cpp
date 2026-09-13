#include "../private.h"

#include <psp2/kernel/sysmem.h>

namespace ryo {
    bool _create_default_depth_stencil(InternalRenderer* intern) {
        void* depthBuffer = nullptr;
        const uint32_t depthSize = _memory_align_cdram(intern->stride * intern->height * 4); // D24S8 = 32 bits

        SceUID depthMemId = sceKernelAllocMemBlock(
            "depth_stencil",
            SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW,
            depthSize,
            nullptr
        );

        if (depthMemId < 0) {
            error_set("Failed to allocate depth stencil memory %d", depthMemId);
            return false;
        }

        intern->depthMemId = depthMemId;

        int result = sceKernelGetMemBlockBase(depthMemId, &depthBuffer);

        if (result < 0) {
            error_set("Failed to allocate depth stencil memory (sceKernelGetMemBlockBase) %d", result);
            return false;
        }

        intern->depthMem = depthBuffer;

        result = sceGxmMapMemory(
            depthBuffer,
            depthSize,
            (SceGxmMemoryAttribFlags)(SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE)
        );

        if (result < 0) {
            error_set("Failed to map depth stencil memory %d", result);
            return false;
        }

        SceGxmDepthStencilSurface depthSurface;

        result = sceGxmDepthStencilSurfaceInit(
            &depthSurface,
            SCE_GXM_DEPTH_STENCIL_FORMAT_S8D24,
            SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
            intern->stride,
            depthBuffer,
            nullptr
        );

        if (result < 0) {
            error_set("Failed to initialize depth stencil %d", result);
            return false;
        }

        intern->depthSurface = depthSurface;

        return true;
    }
}