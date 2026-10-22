#pragma once

#import "RYView.h"

@class RYViewRoot;

@interface RYView (Internal)

- (void)ry_setViewRoot:(RYViewRoot *)viewRoot;

@end
