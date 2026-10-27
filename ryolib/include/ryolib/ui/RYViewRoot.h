#pragma once

#include "../objc/RYObject.h"

#include "../math.h"
#include "../graphics.h"

@class RYView;

@interface RYViewRoot : RYObject {
@protected
    ryo::Renderer *_renderer;
    ryo::RenderTarget *_renderTarget;
    ryo::Vector2 _physicalResolution;
    float _contentScale;

    RYView *_contentView;
    ryo::Vector2 _logicalSize;
    ryo::Color _backgroundColor;
}

- (instancetype)initWithRenderer:(ryo::Renderer *)renderer
              physicalResolution:(ryo::Vector2)resolution
                    contentScale:(float)contentScale
                     contentView:(RYView *)contentView;

@property (nonatomic, readonly) RYView *contentView;
@property (nonatomic, readonly) ryo::Vector2 logicalSize;
@property (nonatomic, assign)   ryo::Color backgroundColor;

- (void)setContentView:(RYView *)contentView;

- (void)updateSize:(ryo::Vector2)physicalResolution
      contentScale:(float)contentScale;

- (void)update:(float)deltaTime;
- (void)layout;

- (const ryo::Texture *)draw;

@end