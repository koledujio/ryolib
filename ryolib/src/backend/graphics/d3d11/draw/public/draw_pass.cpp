#include "../private.h"

namespace ryo {
    void Renderer::draw_pass_begin(RenderTarget* target, const Color* clearColor) {
        RYO_ENSURE_VOID(valid(), "Invalid renderer");
        RYO_ENSURE_VOID(internal->inFrame, "Renderer not in a frame");
        RYO_ENSURE_VOID(!internal->inPass, "Renderer already in a draw pass");

        ID3D11RenderTargetView* colorView = nullptr;
        ID3D11DepthStencilView* depthView = nullptr;

        if (target) {
            RYO_ENSURE_VOID(target->valid(), "Invalid render target");

            colorView = target->internal->colorView;
            depthView = target->internal->depthView;
        } else {
            colorView = internal->colorView;
            depthView = internal->depthView;
        }

        RYO_ASSERT_VOID(colorView != nullptr, "Invalid render target view");
        RYO_ASSERT_VOID(depthView != nullptr, "Invalid depth stencil view");

        internal->context->OMSetRenderTargets(1, &colorView, depthView);
        internal->context->OMSetDepthStencilState(internal->depthState, 0);

        int width = target ? target->internal->width : internal->width;
        int height = target ? target->internal->height : internal->height;

        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<float>(width);
        viewport.Height = static_cast<float>(height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        internal->context->RSSetViewports(1, &viewport);

        if (clearColor) {
            const float color[] = {
                clearColor->r,
                clearColor->g,
                clearColor->b,
                clearColor->a
            };

            internal->context->ClearRenderTargetView(colorView, color);

            if (depthView) {
                internal->context->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
            }
        }

        internal->currentTarget = target;
        internal->inPass = true;
    }

    void Renderer::draw_pass_end() {
        RYO_ENSURE_VOID(valid(), "Invalid renderer");
        RYO_ENSURE_VOID(internal->inFrame, "Renderer not in a frame");
        RYO_ENSURE_VOID(internal->inPass, "Renderer not in a draw pass");

        internal->context->OMSetRenderTargets(0, nullptr, nullptr);

        internal->currentTarget = nullptr;
        internal->inPass = false;
    }
}
