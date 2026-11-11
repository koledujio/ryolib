#include "../private.h"

@implementation RYD3D11GraphicsBackend (DrawPass)

- (void)drawPassBeginWithRenderTarget:(RYRenderTarget *)target clearColor:(const RYColor *)clearColor {
    if (!_internal) return;

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    if (!intern->inFrame || intern->inPass) return;

    ID3D11RenderTargetView* colorView = nullptr;
    ID3D11DepthStencilView* depthView = nullptr;

    if (target) {
        RYInternalRenderTarget* targetIntern = (RYInternalRenderTarget*)target.internalRenderTarget;
        colorView = targetIntern->colorView;
        depthView = targetIntern->depthView;
    } else {
        colorView = intern->colorView;
        depthView = intern->depthView;
    }

    if (!colorView || !depthView) return;

    intern->context->OMSetRenderTargets(1, &colorView, depthView);
    intern->context->OMSetDepthStencilState(intern->depthStates[0], 0);

    int width = 0, height = 0;
    if (target) {
        RYInternalRenderTarget* targetIntern = (RYInternalRenderTarget*)target.internalRenderTarget;
        width = targetIntern->width;
        height = targetIntern->height;
    } else {
        width = intern->width;
        height = intern->height;
    }

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    intern->context->RSSetViewports(1, &viewport);

    if (clearColor) {
        const float color[] = {
            clearColor->r,
            clearColor->g,
            clearColor->b,
            clearColor->a
        };

        intern->context->ClearRenderTargetView(colorView, color);

        if (depthView) {
            intern->context->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        }
    }

    intern->currentTarget = target;
    intern->inPass = true;
}

- (void)drawPassEnd {
    if (!_internal) return;

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    if (!intern->inFrame || !intern->inPass) return;

    intern->context->OMSetRenderTargets(0, nullptr, nullptr);

    intern->currentTarget = nil;
    intern->inPass = false;
}

@end
