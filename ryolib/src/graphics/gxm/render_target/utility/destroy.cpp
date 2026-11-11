#include "../private.h"

namespace ryo {
    void _render_target_destroy(InternalRenderTarget* target) {
        if (!target) return;

        if (target->renderer && target->renderer->valid() && target->renderer->internal && target->renderer->internal->context) {
            sceGxmFinish(target->renderer->internal->context);
        }

        if (target->colorTexture) {
            delete target->colorTexture;
            target->colorTexture = nullptr;
        }

        if (target->sync) {
            sceGxmSyncObjectDestroy(target->sync);
            target->sync = nullptr;
        }

        if (target->depthMem) {
            _memory_free_gpu(target->depthMem, target->depthMemId);
            target->depthMem = nullptr;
        }

        if (target->colorMem) {
            _memory_free_gpu(target->colorMem, target->colorMemId);
            target->colorMem = nullptr;
        }

        if (target->renderTarget) {
            sceGxmDestroyRenderTarget(target->renderTarget);
            target->renderTarget = nullptr;
        }
    }
}