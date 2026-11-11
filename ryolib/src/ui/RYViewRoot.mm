#import "ryolib/ui/RYViewRoot.h"
#import "ryolib/ui/RYView.h"
#import "ryolib/ui/RYViewInternal.h"

using namespace ryo;

@implementation RYViewRoot

@synthesize contentView = _contentView;
@synthesize logicalSize = _logicalSize;
@synthesize backgroundColor = _backgroundColor;

- (instancetype)initWithRenderer:(RYRenderer *)renderer
              physicalResolution:(Vector2)resolution
                    contentScale:(float)contentScale
                     contentView:(RYView *)contentView {
    self = [super init];
    if (!self)
        return nil;
    
    if (!renderer ||
        resolution.x <= 0 ||
        resolution.y <= 0 ||
        contentScale <= 0) {
        [self release];
        return nil;
    }

    _renderer = [renderer retain];
    _renderTarget = nil;

    _physicalResolution = resolution;
    _contentScale = contentScale;

    _contentView = nil;
    _logicalSize = {
        resolution.x / contentScale,
        resolution.y / contentScale
    };

    _backgroundColor = RYColorMake(0.0f, 0.0f, 0.0f, 1.0f);

    RYRenderTarget *renderTarget = [RYRenderTarget newRenderTargetWithRenderer:renderer width:(unsigned int)resolution.x height:(unsigned int)resolution.y];

    if (!renderTarget) {
        [self release];
        return nil;
    }
    _renderTarget = renderTarget;

    [self setContentView:contentView];

    return self;
}

- (void)dealloc {
    if (_contentView) {
        [_contentView ry_setViewRoot:nil];
        [_contentView release];
        _contentView = nil;
    }

    if (_renderTarget) {
        [_renderTarget release];
        _renderTarget = nil;
    }

    if (_renderer) {
        [_renderer release];
        _renderer = nil;
    }

    [super dealloc];
}

- (void)setContentView:(RYView *)contentView {
    if (_contentView == contentView)
        return;
    
    [contentView retain];

    if (_contentView) {
        [_contentView ry_setViewRoot:nil];
        [_contentView release];
    }

    if (contentView) {
        [contentView removeFromSuperview];
        [contentView ry_setViewRoot:self];
    }

    _contentView = contentView;
}

- (void)updateSize:(Vector2)physicalResolution
      contentScale:(float)contentScale {
    if (physicalResolution.x <= 0 ||
        physicalResolution.y <= 0 ||
        contentScale <= 0) {
        return;
    }

    if (_physicalResolution.x == physicalResolution.x &&
        _physicalResolution.y == physicalResolution.y &&
        _contentScale == contentScale)
        return;

    RYRenderTarget *newTarget = [RYRenderTarget newRenderTargetWithRenderer:_renderer width:(unsigned int)physicalResolution.x height:(unsigned int)physicalResolution.y];

    if (!newTarget) {
        return;
    }
    
    [_renderTarget release];
    _renderTarget = newTarget;

    _physicalResolution = physicalResolution;
    _contentScale = contentScale;
    _logicalSize = {
        physicalResolution.x / contentScale,
        physicalResolution.y / contentScale
    };
}

- (void)update:(float)deltaTime {
    if (!_contentView)
        return;
    
    [_contentView update:deltaTime];
}

- (void)layout {
    if (!_contentView)
        return;

    [_contentView layout:Matrix4()];
}

- (RYTexture *)draw {
    RYCamera camera = {};
    camera.position = {0.0f, 0.0f, 1.0f};
    camera.target = {0.0f, 0.0f, 0.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.projectionType = RYCameraProjectionTypeOrthographic;
    camera.orthographicSize = _logicalSize.y;
    camera.nearPlane = -1.0f;
    camera.farPlane = 1.0f;

    RYDrawSettings settings = {};
    settings.cullMode = RYCullModeBack;
    settings.windingOrder = RYWindingOrderCounterClockwise;
    settings.fillMode = RYFillModeSolid;
    settings.depthMode = RYDepthModeDisabled;
    settings.camera = &camera;
    settings.pipelineOverride = nil;
    settings.stencil = nil;

    RYDrawContext context{_renderer, &settings};

    if (_contentView)
        [_contentView preDraw:context];
    
    [_renderer drawPassBeginWithRenderTarget:_renderTarget clearColor:&_backgroundColor];

    if (_contentView)
        [_contentView draw:context];

    [_renderer drawPassEnd];

    return _renderTarget.colorTexture;
}

@end
