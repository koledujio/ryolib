#import "ryolib/ui/RYView.h"

using namespace ryo;

@implementation RYView

@synthesize superview = _superview;
@synthesize viewRoot = _viewRoot;
@synthesize subviews = _subviews;
@synthesize position = _position;
@synthesize size = _size;
@synthesize backgroundColor = _backgroundColor;

- (instancetype)init {
    self = [super init];
    if (self) {
        _matrix = Matrix4();

        _superview = nil;
        _viewRoot = nil;
        _subviews = [[RYArray alloc] init];

        _position = {0.0f, 0.0f};
        _size = {100.0f, 100.0f};
        _backgroundColor = RYColorMake(1.0f, 1.0f, 1.0f, 1.0f);
    }
    return self;
}

- (void)dealloc {
    [_subviews release];
    [super dealloc];
}

- (RYViewRoot *)viewRoot {
    return _viewRoot;
}

- (void)addSubview:(RYView *)view {
    if (!view || view == self) {
        return;
    }

    [view removeFromSuperview];
    view->_superview = self;
    [_subviews addObject:view];
}

- (void)removeFromSuperview {
    if (!_superview) {
        return;
    }

    [_superview->_subviews removeObject:self];
    _superview = nil;
}

- (void)update:(float)deltaTime {
    for (unsigned long i = 0; i < _subviews.count; i++) {
        RYView *view = (RYView *)[_subviews objectAtIndex:i];

        [view update:deltaTime];
    }
}

- (void)layout:(Matrix4)parentMatrix {
    _matrix = parentMatrix * Matrix4::translation(_position.x, _position.y, 0.0f) * Matrix4::scaling(_size.x, _size.y, 1.0f);

    for (unsigned long i = 0; i < _subviews.count; i++) {
        RYView *view = (RYView *)[_subviews objectAtIndex:i];

        [view layout:_matrix];
    }
}

- (void)preDraw:(RYDrawContext &)context {
    for (unsigned long i = 0; i < _subviews.count; i++) {
        RYView *view = (RYView *)[_subviews objectAtIndex:i];

        [view preDraw:context];
    }

    // nothing needs to be done for RYView, but for some views that have their own render targets, this is needed
}

- (void)draw:(RYDrawContext &)context {
    [context.renderer drawRectColor:_backgroundColor matrix:_matrix settings:context.settings];

    for (unsigned long i = 0; i < _subviews.count; i++) {
        RYView *view = (RYView *)[_subviews objectAtIndex:i];

        [view draw:context];
    }
}

- (void)ry_setViewRoot:(RYViewRoot *)viewRoot {
    _viewRoot = viewRoot;

    for (unsigned long i = 0; i < [_subviews count]; i++) {
        RYView *subview = [_subviews objectAtIndex:i];
        [subview ry_setViewRoot:viewRoot];
    }
}

@end
