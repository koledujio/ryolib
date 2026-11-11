#include "private.h"

#include "renderer/private.h"
#include "render_target/private.h"
#include "pipeline/private.h"

static bool _renderer_has_required_resources(const RYInternalRenderer *intern) {
    return intern->device != nil &&
           intern->queue != nil &&
           intern->shaderLibrary != nil &&
           intern->depthStates[0] != nil &&
           intern->mainWindowDepthTexture != nil &&
           intern->nearestSampler != nil &&
           intern->linearSampler != nil &&
           intern->minNearestMagLinearSampler != nil &&
           intern->minLinearMagNearestSampler != nil &&
           intern->metalLayer != nullptr;
}

@implementation RYMetalGraphicsBackend

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
                                           message:"Null config passed to Metal backend"];
        }
        [self release];
        return nil;
    }

    id<MTLDevice> device = nil;
    BOOL ownsDevice = NO;

    if (shareBackend) {
        if ([shareBackend class] != [RYMetalGraphicsBackend class]) {
            if (error) {
                *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInvalidRenderer
                                               message:"Shared backend is not a Metal backend"];
            }
            [self release];
            return nil;
        }

        RYInternalRenderer *shareIntern = (RYInternalRenderer *)((RYMetalGraphicsBackend *)shareBackend)->_internal;
        device = shareIntern ? shareIntern->device : nil;
        if (!device) {
            if (error) {
                *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInitializationFailed
                                               message:"Shared backend has no Metal device"];
            }
            [self release];
            return nil;
        }
    } else {
        device = MTLCreateSystemDefaultDevice();
        ownsDevice = YES;
        if (!device) {
            if (error) {
                *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInitializationFailed
                                               message:"Failed to create Metal device"];
            }
            [self release];
            return nil;
        }
    }

    RYInternalRenderer *intern = _renderer_create(*config, device, error);
    if (ownsDevice) {
        [device release];
    }

    if (!intern) {
        [self release];
        return nil;
    }

    _internal = intern;

    if (!_renderer_pipeline_init(self, *intern) ||
        !_renderer_buffers_init(self, *intern) ||
        !_renderer_has_required_resources(intern)) {
        if (error && !*error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInitializationFailed
                                           message:"Failed to initialize required Metal resources"];
        }
        [self release];
        return nil;
    }

    return self;
}

- (void)dealloc {
    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    if (intern) {
        if (intern->metalLayer) {
            [intern->metalLayer removeFromSuperlayer];
            [intern->metalLayer release];
        }

        if (intern->defaultColorPipeline) {
            void *handle = intern->defaultColorPipeline.internalPipeline;
            [intern->defaultColorPipeline release];
            intern->defaultColorPipeline = nil;
            [self destroyPipeline:handle];
        }

        if (intern->defaultTexturePipeline) {
            void *handle = intern->defaultTexturePipeline.internalPipeline;
            [intern->defaultTexturePipeline release];
            intern->defaultTexturePipeline = nil;
            [self destroyPipeline:handle];
        }

        if (intern->stencilPipeline) {
            void *handle = intern->stencilPipeline.internalPipeline;
            [intern->stencilPipeline release];
            intern->stencilPipeline = nil;
            [self destroyPipeline:handle];
        }

        if (intern->quadVertexBuffer) {
            void *handle = intern->quadVertexBuffer.internalBuffer;
            [intern->quadVertexBuffer release];
            intern->quadVertexBuffer = nil;
            [self destroyBuffer:handle];
        }

        if (intern->quadIndexBuffer) {
            void *handle = intern->quadIndexBuffer.internalBuffer;
            [intern->quadIndexBuffer release];
            intern->quadIndexBuffer = nil;
            [self destroyBuffer:handle];
        }

        if (intern->quadVertexBufferUV) {
            void *handle = intern->quadVertexBufferUV.internalBuffer;
            [intern->quadVertexBufferUV release];
            intern->quadVertexBufferUV = nil;
            [self destroyBuffer:handle];
        }

        if (intern->quadIndexBufferUV) {
            void *handle = intern->quadIndexBufferUV.internalBuffer;
            [intern->quadIndexBufferUV release];
            intern->quadIndexBufferUV = nil;
            [self destroyBuffer:handle];
        }

        for (int i = 0; i < 3; i++) {
            if (intern->depthStates[i])
                [intern->depthStates[i] release];
            if (intern->stencilTestStates[i])
                [intern->stencilTestStates[i] release];
        }

        if (intern->stencilWriteState)
            [intern->stencilWriteState release];

        if (intern->mainWindowDepthTexture)
            [intern->mainWindowDepthTexture release];

        if (intern->nearestSampler)
            [intern->nearestSampler release];

        if (intern->linearSampler)
            [intern->linearSampler release];

        if (intern->minNearestMagLinearSampler)
            [intern->minNearestMagLinearSampler release];

        if (intern->minLinearMagNearestSampler)
            [intern->minLinearMagNearestSampler release];

        if (intern->shaderLibrary)
            [intern->shaderLibrary release];

        if (intern->autoreleasePool)
            [intern->autoreleasePool drain];

        if (intern->queue)
            [intern->queue release];

        if (intern->device)
            [intern->device release];

        delete intern;
        _internal = nullptr;
    }

    [super dealloc];
}

- (RYGraphicsBackendType)backendType {
    return RYGraphicsBackendMetal;
}

- (BOOL)resizeWithWidth:(unsigned int)width height:(unsigned int)height {
    if (!_internal) return NO;

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    unsigned int pixelWidth = static_cast<unsigned int>(static_cast<float>(width) * intern->contentScale);
    unsigned int pixelHeight = static_cast<unsigned int>(static_cast<float>(height) * intern->contentScale);

    if (pixelWidth == static_cast<unsigned int>(intern->width) &&
        pixelHeight == static_cast<unsigned int>(intern->height)) {
        return NO;
    }

    _renderer_set_layer_size(*intern, pixelWidth, pixelHeight);
    return YES;
}

- (BOOL)getSize:(unsigned int *)outWidth height:(unsigned int *)outHeight {
    if (!_internal) return NO;

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    if (outWidth) *outWidth = static_cast<unsigned int>(intern->width);
    if (outHeight) *outHeight = static_cast<unsigned int>(intern->height);
    return YES;
}

- (BOOL)getContentScale:(float *)outScale {
    if (!_internal) return NO;

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    if (outScale) *outScale = intern->contentScale;
    return YES;
}

- (BOOL)setContentScale:(float)scale {
    if (!_internal) return NO;

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
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

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
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
