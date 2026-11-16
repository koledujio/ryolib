#include "../private.h"

namespace ryo {
    bool _render_target_create(const Renderer& renderer, unsigned int width, unsigned int height, InternalRenderTarget* target) {
        RYO_ENSURE(renderer.valid(), false, "Invalid renderer");
        RYO_ENSURE(target != nullptr, false, "Null target pointer");

        target->renderer = &renderer;
        target->width = width;
        target->height = height;
        target->stride = (width + 7) & ~7; // 8-pixel alignment for GXM

        SceGxmRenderTargetParams params = {};
        params.flags = 0;
        params.width = width;
        params.height = height;
        params.scenesPerFrame = 1;
        params.multisampleMode = SCE_GXM_MULTISAMPLE_NONE;
        params.multisampleLocations = 0;
        params.driverMemBlock = -1;

        int renderTargetCreateResult = sceGxmCreateRenderTarget(&params, &target->renderTarget);
        if (renderTargetCreateResult < 0) {
            error_set("sceGxmCreateRenderTarget failed: 0x%08X", renderTargetCreateResult);
            return false;
        }

        const uint32_t colorSize = _memory_align_cdram(target->stride * height * 4);
        if (!_memory_alloc_gpu_rw("rt_color", colorSize, _MEMORY_TYPE_CDRAM, target->colorMem, target->colorMemId)) {
            error_set("Failed to allocate render target color memory");
            return false;
        }

        int colorSurfaceInitResult = sceGxmColorSurfaceInit(
            &target->colorSurface,
            SCE_GXM_COLOR_FORMAT_A8B8G8R8,
            SCE_GXM_COLOR_SURFACE_LINEAR,
            SCE_GXM_COLOR_SURFACE_SCALE_NONE,
            SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
            width,
            height,
            target->stride,
            target->colorMem
        );
        if (colorSurfaceInitResult < 0) {
            error_set("sceGxmColorSurfaceInit failed: 0x%08X", colorSurfaceInitResult);
            return false;
        }

        const uint32_t depthStride = (width + 31) & ~31;
        const uint32_t depthHeight = (height + 31) & ~31;
        const uint32_t depthSize = _memory_align_cdram(depthStride * depthHeight * 4);
        if (!_memory_alloc_gpu_rw("rt_depth", depthSize, _MEMORY_TYPE_CDRAM, target->depthMem, target->depthMemId)) {
            error_set("Failed to allocate render target depth memory");
            return false;
        }

        int depthStencilSurfaceInitResult = sceGxmDepthStencilSurfaceInit(
            &target->depthSurface,
            SCE_GXM_DEPTH_STENCIL_FORMAT_S8D24,
            SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
            depthStride,
            target->depthMem,
            nullptr
        );
        if (depthStencilSurfaceInitResult < 0) {
            error_set("sceGxmDepthStencilSurfaceInit failed: 0x%08X", depthStencilSurfaceInitResult);
            return false;
        }

        int syncObjectCreateResult = sceGxmSyncObjectCreate(&target->sync);
        if (syncObjectCreateResult < 0) {
            error_set("sceGxmSyncObjectCreate failed: 0x%08X", syncObjectCreateResult);
            return false;
        }

        InternalTexture* colorTexIntern = new InternalTexture;
        colorTexIntern->renderer = &renderer;
        colorTexIntern->mem = target->colorMem;
        colorTexIntern->memId = -1;
        colorTexIntern->width = width;
        colorTexIntern->height = height;

        int textureInitLinearResult = sceGxmTextureInitLinearStrided(
            &colorTexIntern->texture,
            target->colorMem,
            SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
            width,
            height,
            target->stride * 4
        );
        if (textureInitLinearResult < 0) {
            delete colorTexIntern;
            error_set("sceGxmTextureInitLinearStrided for render target color texture failed: 0x%08X", textureInitLinearResult);
            return false;
        }

        target->colorTexture = new Texture(colorTexIntern);

        return true;
    }
}