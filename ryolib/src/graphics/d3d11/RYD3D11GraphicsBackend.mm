#include "private.h"

#include "renderer/private.h"

@implementation RYD3D11GraphicsBackend

- (instancetype)initBackendWithConfig:(const RYRendererConfig *)config
                         shareBackend:(RYGraphicsBackend *)shareBackend
                                error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    self = [super init];
    if (!self) {
        return nil;
    }

    if (!config) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInvalidRenderer
                                           message:"Null config passed to D3D11 backend"];
        }
        [self release];
        return nil;
    }

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    BOOL ownsDevice = NO;

    if (shareBackend) {
        if ([shareBackend class] != [RYD3D11GraphicsBackend class]) {
            if (error) {
                *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInvalidRenderer
                                               message:"Shared backend is not a D3D11 backend"];
            }
            [self release];
            return nil;
        }

        RYInternalRenderer* shareIntern = (RYInternalRenderer*)((RYD3D11GraphicsBackend*)shareBackend)->_internal;
        device = shareIntern ? shareIntern->device : nullptr;
        context = shareIntern ? shareIntern->context : nullptr;

        if (!device || !context) {
            if (error) {
                *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInitializationFailed
                                               message:"Shared backend has no D3D11 device"];
            }
            [self release];
            return nil;
        }
    } else {
        UINT creationFlags = 0;
#ifdef _DEBUG
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
        D3D_FEATURE_LEVEL achievedFeatureLevel;

        HRESULT hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            featureLevels,
            1,
            D3D11_SDK_VERSION,
            &device,
            &achievedFeatureLevel,
            &context
        );

        if (FAILED(hr)) {
            if (error) {
                *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInitializationFailed
                                               message:"Failed to create D3D11 device"];
            }
            [self release];
            return nil;
        }

        ownsDevice = YES;
    }

    RYInternalRenderer* intern = _renderer_create(*config, device, context);

    if (ownsDevice) {
        device->Release();
        context->Release();
    }

    if (!intern) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInitializationFailed
                                           message:"Failed to initialize D3D11 renderer"];
        }
        [self release];
        return nil;
    }

    _internal = intern;

    if (!_renderer_init_pipelines(self, *intern) || !_create_default_buffers(self, *intern)) {
        if (error && !*error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInitializationFailed
                                           message:"Failed to initialize default pipelines or buffers"];
        }
        [self release];
        return nil;
    }

    return self;
}

- (void)dealloc {
    RYInternalRenderer* intern = (RYInternalRenderer*)_internal;
    if (intern) {
        if (intern->quadVertexBuffer) {
            void* handle = intern->quadVertexBuffer.internalBuffer;
            [intern->quadVertexBuffer release];
            intern->quadVertexBuffer = nil;
            [self destroyBuffer:handle];
        }

        if (intern->quadIndexBuffer) {
            void* handle = intern->quadIndexBuffer.internalBuffer;
            [intern->quadIndexBuffer release];
            intern->quadIndexBuffer = nil;
            [self destroyBuffer:handle];
        }

        if (intern->quadUVVertexBuffer) {
            void* handle = intern->quadUVVertexBuffer.internalBuffer;
            [intern->quadUVVertexBuffer release];
            intern->quadUVVertexBuffer = nil;
            [self destroyBuffer:handle];
        }

        if (intern->quadUVIndexBuffer) {
            void* handle = intern->quadUVIndexBuffer.internalBuffer;
            [intern->quadUVIndexBuffer release];
            intern->quadUVIndexBuffer = nil;
            [self destroyBuffer:handle];
        }

        if (intern->defaultColorPipeline) {
            void* handle = intern->defaultColorPipeline.internalPipeline;
            [intern->defaultColorPipeline release];
            intern->defaultColorPipeline = nil;
            [self destroyPipeline:handle];
        }

        if (intern->defaultTexturePipeline) {
            void* handle = intern->defaultTexturePipeline.internalPipeline;
            [intern->defaultTexturePipeline release];
            intern->defaultTexturePipeline = nil;
            [self destroyPipeline:handle];
        }

        if (intern->stencilPipeline) {
            void* handle = intern->stencilPipeline.internalPipeline;
            [intern->stencilPipeline release];
            intern->stencilPipeline = nil;
            [self destroyPipeline:handle];
        }

        if (intern->vertexCB) intern->vertexCB->Release();
        if (intern->pixelCB) intern->pixelCB->Release();

        if (intern->colorView) intern->colorView->Release();
        if (intern->depthTexture) intern->depthTexture->Release();
        if (intern->depthView) intern->depthView->Release();

        if (intern->stencilWriteState) intern->stencilWriteState->Release();
        for (int i = 0; i < 3; i++) {
            if (intern->depthStates[i])
                intern->depthStates[i]->Release();
            if (intern->stencilTestStates[i])
                intern->stencilTestStates[i]->Release();
        }

        for (int i = 0; i < 4; i++) {
            if (intern->samplers[i])
                intern->samplers[i]->Release();
        }

        for (int c = 0; c < 3; c++) {
            for (int f = 0; f < 2; f++) {
                for (int w = 0; w < 2; w++) {
                    if (intern->rasterizers[c][f][w])
                        intern->rasterizers[c][f][w]->Release();
                }
            }
        }

        if (intern->swapChain) intern->swapChain->Release();
        if (intern->context) intern->context->Release();
        if (intern->device) intern->device->Release();

        delete intern;
        _internal = nullptr;
    }

    [super dealloc];
}

- (RYGraphicsBackendType)backendType {
    return RYGraphicsBackendD3D11;
}

- (BOOL)resizeWithWidth:(unsigned int)width height:(unsigned int)height {
    if (!_internal) return NO;

    RYInternalRenderer* intern = (RYInternalRenderer*)_internal;
    unsigned int pixelWidth = static_cast<unsigned int>(static_cast<float>(width) * intern->contentScale);
    unsigned int pixelHeight = static_cast<unsigned int>(static_cast<float>(height) * intern->contentScale);

    if (pixelWidth == static_cast<unsigned int>(intern->width) &&
        pixelHeight == static_cast<unsigned int>(intern->height)) {
        return NO;
    }

    intern->context->OMSetRenderTargets(0, nullptr, nullptr);
    _renderer_release_window_targets(*intern);

    HRESULT hr = intern->swapChain->ResizeBuffers(
        0,
        pixelWidth,
        pixelHeight,
        DXGI_FORMAT_UNKNOWN,
        0
    );

    if (FAILED(hr)) return NO;

    if (!_renderer_create_window_targets(*intern, pixelWidth, pixelHeight)) return NO;

    intern->width = pixelWidth;
    intern->height = pixelHeight;

    return YES;
}

- (BOOL)getSize:(unsigned int *)outWidth height:(unsigned int *)outHeight {
    if (!_internal) return NO;

    RYInternalRenderer* intern = (RYInternalRenderer*)_internal;
    if (outWidth) *outWidth = static_cast<unsigned int>(intern->width);
    if (outHeight) *outHeight = static_cast<unsigned int>(intern->height);
    return YES;
}

- (BOOL)getContentScale:(float *)outScale {
    if (!_internal) return NO;

    RYInternalRenderer* intern = (RYInternalRenderer*)_internal;
    if (outScale) *outScale = intern->contentScale;
    return YES;
}

- (BOOL)setContentScale:(float)scale {
    if (!_internal) return NO;

    RYInternalRenderer* intern = (RYInternalRenderer*)_internal;
    if (scale == intern->contentScale) return YES;

    float old = intern->contentScale;
    intern->contentScale = scale < 1.0f ? 1.0f : scale;
    if (![self resizeWithWidth:static_cast<unsigned int>(intern->width) height:static_cast<unsigned int>(intern->height)]) {
        intern->contentScale = old;
        return NO;
    }

    return YES;
}

- (void *)builtinPipelineHandle:(RYBuiltinPipeline)pipeline {
    if (!_internal) return NULL;

    RYInternalRenderer* intern = (RYInternalRenderer*)_internal;
    switch (pipeline) {
        case RYBuiltinPipelineColor:
            return intern->defaultColorPipeline ? intern->defaultColorPipeline.internalPipeline : NULL;
        case RYBuiltinPipelineTexture:
            return intern->defaultTexturePipeline ? intern->defaultTexturePipeline.internalPipeline : NULL;
        case RYBuiltinPipelineStencil:
            return intern->stencilPipeline ? intern->stencilPipeline.internalPipeline : NULL;
    }
    return NULL;
}

@end
