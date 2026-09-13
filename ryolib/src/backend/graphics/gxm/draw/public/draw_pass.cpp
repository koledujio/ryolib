#include "../private.h"

namespace ryo {
    void Renderer::draw_pass_begin(RenderTarget* renderTarget, const Color* clearColor) {
        RYO_ENSURE_VOID(valid(), "Invalid renderer");
        RYO_ENSURE_VOID(internal->inFrame, "Renderer not in a frame");
        RYO_ENSURE_VOID(!internal->inPass, "Renderer is already in a draw pass");

        SceGxmRenderTarget* gxmTarget = nullptr;
        SceGxmColorSurface* colorSurface = nullptr;
        SceGxmDepthStencilSurface* depthSurface = nullptr;
        SceGxmSyncObject* sync = nullptr;
        float width = 0.0f;
        float height = 0.0f;

        if (renderTarget != nullptr && renderTarget->valid()) {
            gxmTarget = renderTarget->internal->renderTarget;
            colorSurface = &renderTarget->internal->colorSurface;
            depthSurface = &renderTarget->internal->depthSurface;
            sync = renderTarget->internal->sync;
            width = static_cast<float>(renderTarget->internal->width);
            height = static_cast<float>(renderTarget->internal->height);
            internal->currentTarget = renderTarget;
        } else {
            _DisplayBuffer& buffer = internal->displayBuffers[internal->backBuffer];
            gxmTarget = internal->renderTarget;
            colorSurface = &buffer.surface;
            depthSurface = &internal->depthSurface;
            sync = buffer.sync;
            width = static_cast<float>(internal->width);
            height = static_cast<float>(internal->height);
            internal->currentTarget = nullptr;
        }

        int result = sceGxmBeginScene(
            internal->context,
            0,
            gxmTarget,
            nullptr,
            nullptr,
            sync,
            colorSurface,
            depthSurface
        );
        if (result < 0) {
            error_set("sceGxmBeginScene failed: 0x%08X", result);
            return; // TODO: make this function return a bool
        }

        float xOffset = width * 0.5f;
        float yOffset = height * 0.5f;
        float widthScale = width * 0.5f;
        float heightScale = -height * 0.5f;

        sceGxmSetViewport(internal->context, xOffset, widthScale, yOffset, heightScale, 0.0f, 1.0f);

        internal->inPass = true;

        if (clearColor != nullptr) {
            DrawSettings clearSettings = {};
            clearSettings.cullMode = CULL_MODE_NONE;
            clearSettings.camera = nullptr;

            Matrix4 clearTransform = Matrix4::translation(0.0f, 0.0f, 1.0f) * Matrix4::scaling(2.0f, 2.0f, 1.0f);

            draw_rect_color(*clearColor, clearTransform, &clearSettings);
        }
    }

    void Renderer::draw_pass_end() {
        RYO_ENSURE_VOID(valid(), "Invalid renderer");
        RYO_ENSURE_VOID(internal->inFrame, "Renderer not in a frame");
        RYO_ENSURE_VOID(internal->inPass, "Renderer is not in a draw pass");

        sceGxmEndScene(internal->context, nullptr, nullptr);
        internal->currentTarget = nullptr;
        internal->inPass = false;
    }
}