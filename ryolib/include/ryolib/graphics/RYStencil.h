#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYError.h"
#import "RYGraphicsTypes.h"

/**
 * @brief Collects shapes that can be used as a mask for draw calls.
 *
 * Add shapes to the stencil, then pass it through RYDrawSettings. The shapes
 * stay in the stencil until you call clear.
 */
@interface RYStencil : RYObject {
@protected
    void *_internal;
    RYRenderer *_renderer;
}

/** Whether no shapes have been added. */
@property (nonatomic, readonly) BOOL isEmpty;

/** Renderer the stencil was created on. */
@property (nonatomic, readonly) RYRenderer *renderer;

/** Backend-specific stencil state. Do not free this pointer yourself. */
@property (nonatomic, readonly) void *internalStencil;

/** Creates a stencil. Returns nil on failure. */
+ (instancetype)newStencilWithRenderer:(RYRenderer *)renderer;

/** Creates a stencil and reports errors. */
+ (instancetype)newStencilWithRenderer:(RYRenderer *)renderer error:(RYError **)error;

/** Designated initializer. */
- (instancetype)initWithRenderer:(RYRenderer *)renderer error:(RYError **)error;

/** Removes all shapes. */
- (BOOL)clear;

/** Adds a rectangle shape. */
- (BOOL)drawRectangle:(ryo::Matrix4)matrix camera:(const RYCamera *)camera;

/** Adds a rounded rectangle shape. */
- (BOOL)drawRoundedRectangle:(ryo::Matrix4)matrix radius:(float)radius camera:(const RYCamera *)camera;

/** Adds an oval shape. */
- (BOOL)drawOval:(ryo::Matrix4)matrix camera:(const RYCamera *)camera;

@end
