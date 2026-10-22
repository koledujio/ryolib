#import "ryolib/ui/RYViewRoot.h"
#import "ryolib/ui/RYView.h"
#import "ryolib/ui/RYViewInternal.h"

using namespace ryo;

@implementation RYViewRoot

- (instancetype)initWithRenderer:(Renderer *)renderer
              physicalResolution:(Vector2)resolution
                    contentScale:(float)contentScale
                     contentView:(RYView *)contentView {
    self = [super init];
    if (!self)
        return nil;
    
    if (!renderer ||
        !renderer->valid() ||
        resolution.x <= 0 ||
        resolution.y <= 0 ||
        contentScale <= 0) {
        [self release];
        return nil;
    }

    _renderer = renderer;
    _renderTarget = nullptr;

    _physicalResolution = resolution;
    _contentScale = contentScale;

    _contentView = nil;
    _logicalSize = {
        resolution.x / contentScale,
        resolution.y / contentScale
    };

    self.backgroundColor = {0.0f, 0.0f, 0.0f, 1.0f};

    RenderTarget* renderTarget = new RenderTarget(*renderer, resolution.x, resolution.y);

    if (!renderTarget->valid()) {
        delete renderTarget;
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

    if (_renderTarget != nullptr) {
        delete _renderTarget;
        _renderTarget = nullptr;
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

    RenderTarget *newTarget = new RenderTarget(*_renderer, physicalResolution.x, physicalResolution.y);

    if (!newTarget->valid()) {
        delete newTarget;
        return;
    }
    
    delete _renderTarget; // must be valid because it's guaranteed to be created at least once by init and if a size update fails it remains valid by not changing
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

- (const Texture *)draw {
    Camera camera = {};
    camera.position = {0.0f, 0.0f, 1.0f};
    camera.target = {0.0f, 0.0f, 0.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.projectionType = CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC;
    camera.orthographicSize = _logicalSize.y;
    camera.nearPlane = -1.0f;
    camera.farPlane = 1.0f;

    DrawSettings settings = {};
    settings.cullMode = CULL_MODE_BACK;
    settings.windingOrder = WINDING_ORDER_COUNTER_CLOCKWISE;
    settings.fillMode = FILL_MODE_SOLID;
    settings.depthMode = DEPTH_MODE_DISABLED;
    settings.camera = &camera;
    settings.pipelineOverride = nullptr;
    settings.stencil = nullptr;

    RYDrawContext context{*_renderer, settings};

    if (_contentView)
        [_contentView preDraw:context]; // for drawing outside of a draw pass, required if a view wants it's own render target that's then drawn in draw
    
    _renderer->draw_pass_begin(_renderTarget, &_backgroundColor);

    if (_contentView)
        [_contentView draw:context];

    _renderer->draw_pass_end();

    return _renderTarget->texture_color_get();
}

@end
