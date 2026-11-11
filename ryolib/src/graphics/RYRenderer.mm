#import "ryolib/graphics/RYRenderer.h"

#import "stub/RYStubGraphicsBackend.h"

#if defined(RYO_ENABLE_GRAPHICS_METAL)
#import "metal/RYMetalGraphicsBackend.h"
#elif defined(RYO_ENABLE_GRAPHICS_D3D11)
#import "d3d11/RYD3D11GraphicsBackend.h"
#endif

@implementation RYRenderer

@synthesize backend = _backend;
@synthesize backendInstance = _backendInstance;

+ (instancetype)newRendererWithConfig:(const RYRendererConfig *)config {
    return [self newRendererWithBackend:RYGraphicsBackendDefault config:config error:NULL];
}

+ (instancetype)newRendererWithConfig:(const RYRendererConfig *)config error:(RYError **)error {
    return [self newRendererWithBackend:RYGraphicsBackendDefault config:config error:error];
}

+ (instancetype)newRendererWithConfig:(const RYRendererConfig *)config
                        shareRenderer:(RYRenderer *)shareRenderer
                                error:(RYError **)error {
    return [self newRendererWithBackend:RYGraphicsBackendDefault
                                 config:config
                          shareRenderer:shareRenderer
                                  error:error];
}

+ (instancetype)newRendererWithBackend:(RYGraphicsBackendType)backend
                                config:(const RYRendererConfig *)config {
    return [self newRendererWithBackend:backend config:config error:NULL];
}

+ (instancetype)newRendererWithBackend:(RYGraphicsBackendType)backend
                                config:(const RYRendererConfig *)config
                                 error:(RYError **)error {
    return [self newRendererWithBackend:backend config:config shareRenderer:nil error:error];
}

+ (instancetype)newRendererWithBackend:(RYGraphicsBackendType)backend
                                config:(const RYRendererConfig *)config
                         shareRenderer:(RYRenderer *)shareRenderer
                                 error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    Class backendClass = Nil;

    switch (backend) {
        case RYGraphicsBackendDefault:
#if defined(RYO_ENABLE_GRAPHICS_METAL)
            backendClass = [RYMetalGraphicsBackend class];
            backend = RYGraphicsBackendMetal;
#elif defined(RYO_ENABLE_GRAPHICS_D3D11)
            backendClass = [RYD3D11GraphicsBackend class];
            backend = RYGraphicsBackendD3D11;
#else
            backendClass = [RYStubGraphicsBackend class];
            backend = RYGraphicsBackendStub;
#endif
            break;

        case RYGraphicsBackendStub:
            backendClass = [RYStubGraphicsBackend class];
            break;

        case RYGraphicsBackendMetal:
#if defined(RYO_ENABLE_GRAPHICS_METAL)
            backendClass = [RYMetalGraphicsBackend class];
#endif
            break;

        case RYGraphicsBackendD3D11:
#if defined(RYO_ENABLE_GRAPHICS_D3D11)
            backendClass = [RYD3D11GraphicsBackend class];
#endif
            break;

        default:
            break;
    }

    if (backendClass == Nil) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInvalidRenderer
                                           message:"Invalid or unsupported graphics backend"];
        }
        return nil;
    }

    RYGraphicsBackend *backendInstance = [[backendClass alloc] initBackendWithConfig:config
                                                                        shareBackend:shareRenderer ? shareRenderer.backendInstance : nil
                                                                               error:error];
    if (!backendInstance) {
        return nil;
    }

    RYRenderer *renderer = [[RYRenderer alloc] initWithBackendInstance:backendInstance
                                                          backendType:backend];
    [backendInstance release];
    return renderer;
}

+ (instancetype)newRendererWithCustomBackend:(RYGraphicsBackend *)customBackend {
    return [self newRendererWithCustomBackend:customBackend error:NULL];
}

+ (instancetype)newRendererWithCustomBackend:(RYGraphicsBackend *)customBackend
                                       error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!customBackend) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInvalidRenderer
                                           message:"Custom graphics backend is nil"];
        }
        return nil;
    }

    return [[RYRenderer alloc] initWithBackendInstance:customBackend
                                          backendType:RYGraphicsBackendCustom];
}

- (instancetype)initWithBackendInstance:(RYGraphicsBackend *)backendInstance
                            backendType:(RYGraphicsBackendType)backend {
    self = [super init];
    if (!self) {
        return nil;
    }

    if (!backendInstance) {
        [self release];
        return nil;
    }

    _backend = backend;
    _backendInstance = [backendInstance retain];

    for (int i = 0; i < 3; ++i) {
        _builtinPipelines[i] = nil;
    }

    return self;
}

- (void)dealloc {
    for (int i = 0; i < 3; ++i) {
        if (_builtinPipelines[i]) {
            [_builtinPipelines[i] release];
            _builtinPipelines[i] = nil;
        }
    }

    if (_backendInstance) {
        [_backendInstance release];
        _backendInstance = nil;
    }

    [super dealloc];
}

- (BOOL)resizeWithWidth:(unsigned int)width height:(unsigned int)height {
    return [_backendInstance resizeWithWidth:width height:height];
}

- (BOOL)getSize:(unsigned int *)outWidth height:(unsigned int *)outHeight {
    return [_backendInstance getSize:outWidth height:outHeight];
}

- (BOOL)getContentScale:(float *)outScale {
    return [_backendInstance getContentScale:outScale];
}

- (BOOL)setContentScale:(float)scale {
    return [_backendInstance setContentScale:scale];
}

- (RYPipeline *)builtinPipeline:(RYBuiltinPipeline)pipeline {
    if (pipeline < RYBuiltinPipelineColor || pipeline > RYBuiltinPipelineStencil) {
        return nil;
    }

    if (!_builtinPipelines[pipeline]) {
        void *handle = [_backendInstance builtinPipelineHandle:pipeline];
        if (!handle) {
            return nil;
        }

        // The backend owns the raw pipeline, so the wrapper only borrows it.
        _builtinPipelines[pipeline] = [[RYPipeline alloc] initWithInternalPipeline:handle
                                                                         renderer:nil
                                                                             desc:NULL];
    }

    return _builtinPipelines[pipeline];
}

@end

@implementation RYRenderer (Frame)

- (void)frameBegin {
    [_backendInstance frameBegin];
}

- (BOOL)frameEnd {
    return [_backendInstance frameEnd];
}

- (float)frameTime {
    return [_backendInstance frameTime];
}

@end

@implementation RYRenderer (DrawPass)

- (void)drawPassBeginWithRenderTarget:(RYRenderTarget *)renderTarget clearColor:(const RYColor *)clearColor {
    [_backendInstance drawPassBeginWithRenderTarget:renderTarget clearColor:clearColor];
}

- (void)drawPassEnd {
    [_backendInstance drawPassEnd];
}

@end

@implementation RYRenderer (DrawCall)

- (BOOL)drawEx:(const RYDrawCall *)drawCall {
    return [_backendInstance drawEx:drawCall];
}

@end

@implementation RYRenderer (RectColor)

- (BOOL)drawRectColor:(RYColor)color matrix:(ryo::Matrix4)matrix settings:(const RYDrawSettings *)settings {
    return [_backendInstance drawRectColor:color matrix:matrix settings:settings];
}

@end

@implementation RYRenderer (RectTexture)

- (BOOL)drawRectTexture:(RYTexture *)texture
            samplerType:(RYSamplerType)samplerType
                  color:(RYColor)color
                 matrix:(ryo::Matrix4)matrix
               settings:(const RYDrawSettings *)settings {
    return [_backendInstance drawRectTexture:texture
                                 samplerType:samplerType
                                       color:color
                                      matrix:matrix
                                    settings:settings];
}

@end

@implementation RYRenderer (MeshTexture)

- (BOOL)drawMeshTextureWithVertexBuffer:(RYBuffer *)vertexBuffer
                            indexBuffer:(RYBuffer *)indexBuffer
                             indexCount:(uint32_t)indexCount
                                texture:(RYTexture *)texture
                            samplerType:(RYSamplerType)samplerType
                                  color:(RYColor)color
                                 matrix:(ryo::Matrix4)matrix
                               settings:(const RYDrawSettings *)settings {
    return [_backendInstance drawMeshTextureWithVertexBuffer:vertexBuffer
                                                 indexBuffer:indexBuffer
                                                  indexCount:indexCount
                                                     texture:texture
                                                 samplerType:samplerType
                                                       color:color
                                                      matrix:matrix
                                                    settings:settings];
}

@end
