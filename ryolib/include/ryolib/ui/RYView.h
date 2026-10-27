#pragma once

#include "../objc/RYObject.h"
#include "../objc/RYArray.h"

#include "../math.h"
#include "../graphics.h"

@class RYViewRoot;

struct RYDrawContext {
    ryo::Renderer& renderer;
    const ryo::DrawSettings& settings;
};

@interface RYView : RYObject {
@protected
    ryo::Matrix4 _matrix;

    RYView *_superview;
    RYViewRoot *_viewRoot;
    RYArray *_subviews;
    
    ryo::Vector2 _position;
    ryo::Vector2 _size;
    ryo::Color _backgroundColor;
}

@property (nonatomic, readonly) RYView *superview;
@property (nonatomic, readonly) RYViewRoot *viewRoot;
@property (nonatomic, readonly) RYArray* subviews;

@property (nonatomic) ryo::Vector2 position;
@property (nonatomic) ryo::Vector2 size;
@property (nonatomic) ryo::Color backgroundColor;

- (void)addSubview:(RYView *)view;
- (void)removeFromSuperview;

- (void)update:(float)deltaTime;
- (void)layout:(ryo::Matrix4)parentMatrix;

- (void)preDraw:(RYDrawContext &)context; // is called before draw, outside of a draw_pass
- (void)draw:(RYDrawContext &)context;

@end
