#include "../private.h"

namespace ryo {
    void draw_pass_begin(Renderer renderer, RenderTarget* target, const Color* clearColor) {
        RYO_ENSURE_VOID(renderer_is_valid(renderer), "Invalid renderer handle");

        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];

        RYO_ENSURE_VOID(!ir.inPass, "Renderer already in a draw pass");

        ID3D11RenderTargetView* colorView = nullptr;
        ID3D11DepthStencilView* depthView = nullptr;

        if (target) {
            RYO_ENSURE_VOID(render_target_is_valid(*target), "Invalid non-null render target handle");

            _InternalRenderTarget& renderTarget = _render_target_pool.items[target->index()];

            colorView = renderTarget.colorView;
            depthView = renderTarget.depthView;
        } else {
            colorView = ir.renderTargetView;
            depthView = ir.depthStencilView;
        }

        RYO_ASSERT_VOID(colorView != nullptr, "Failed to get color view");
        RYO_ASSERT_VOID(depthView != nullptr, "Failed to get depth view");

        ir.context->OMSetRenderTargets(1, &colorView, depthView);
        ir.context->OMSetDepthStencilState(ir.depthStencilState, 0);

        int width = target ? _render_target_pool.items[target->index()].width : ir.width;
        int height = target ? _render_target_pool.items[target->index()].height : ir.height;

        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<float>(width);
        viewport.Height = static_cast<float>(height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        ir.context->RSSetViewports(1, &viewport);

        if (clearColor) {
            const float color[] = {
                clearColor->r,
                clearColor->g,
                clearColor->b,
                clearColor->a
            };

            ir.context->ClearRenderTargetView(colorView, color);

            if (depthView) {
                ir.context->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
            }
        }

        ir.currentTarget = target ? *target : RenderTarget{ 0 };
        ir.inPass = true;
    }

    void draw_pass_end(Renderer renderer) {
        RYO_ENSURE_VOID(renderer_is_valid(renderer), "Invalid renderer handle");

        _InternalRenderer& ir = _renderer_pool.items[renderer.index()];

        RYO_ENSURE_VOID(ir.inPass, "Renderer not in a draw pass");

        ir.context->OMSetRenderTargets(0, nullptr, nullptr);

        ir.currentTarget = { 0 };
        ir.inPass = false;
    }
}
