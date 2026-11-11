#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYColor.h"

#include "../math.h"
#import "../graphics.h"

@class RYView;

@interface RYViewRoot : RYObject {
@protected
    RYRenderer *_renderer;
    RYRenderTarget *_renderTarget;
    ryo::Vector2 _physicalResolution;
    float _contentScale;

    RYView *_contentView;
    ryo::Vector2 _logicalSize;
    RYColor _backgroundColor;
}

- (instancetype)initWithRenderer:(RYRenderer *)renderer
              physicalResolution:(ryo::Vector2)resolution
                    contentScale:(float)contentScale
                     contentView:(RYView *)contentView;

@property (nonatomic, readonly) RYView *contentView;
@property (nonatomic, readonly) ryo::Vector2 logicalSize;
@property (nonatomic, assign)   RYColor backgroundColor;

- (void)setContentView:(RYView *)contentView;

- (void)updateSize:(ryo::Vector2)physicalResolution
      contentScale:(float)contentScale;

- (void)update:(float)deltaTime;
- (void)layout;

- (RYTexture *)draw;

@end
