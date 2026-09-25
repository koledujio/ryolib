#include "../private.h"

#include <psp2/kernel/sysmem.h>
#include <psp2/display.h>

namespace ryo {
    void _renderer_destroy(InternalRenderer*& intern) {
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

        if (intern->stencilPipeline) {
            delete intern->stencilPipeline;
            intern->stencilPipeline = nullptr;
        }

        if (intern->quadVertexBuffer) {
            delete intern->quadVertexBuffer;
            intern->quadVertexBuffer = nullptr;
        }

        if (intern->quadIndexBuffer) {
            delete intern->quadIndexBuffer;
            intern->quadIndexBuffer = nullptr;
        }

        if (intern->quadVertexBufferUV) {
            delete intern->quadVertexBufferUV;
            intern->quadVertexBufferUV = nullptr;
        }

        if (intern->quadIndexBufferUV) {
            delete intern->quadIndexBufferUV;
            intern->quadIndexBufferUV = nullptr;
        }

        if (intern->depthMem) {
            sceGxmUnmapMemory(intern->depthMem);
            intern->depthMem = nullptr;
        }

        if (intern->depthMemId >= 0) {
            sceKernelFreeMemBlock(intern->depthMemId);
        }

        intern->depthSurface = {};

        if (intern->renderTarget) {
            sceGxmDestroyRenderTarget(intern->renderTarget);
            intern->renderTarget = nullptr;
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
}