#pragma once

#import "../objc/RYObject.h"
#import "../objc/RYError.h"
#import "RYGraphicsTypes.h"

/**
 * @brief A compiled pipeline state.
 *
 * A pipeline bundles the shaders, vertex layout, and blend mode used by a
 * draw call. The renderer creates a few built-in pipelines that most drawing
 * can use.
 */
@interface RYPipeline : RYObject {
@protected
    void *_internal;
    RYRenderer *_renderer;
    RYPipelineDesc _desc;
    BOOL _ownsInternal;
}

/** Renderer the pipeline was created on. */
@property (nonatomic, readonly) RYRenderer *renderer;

/** Description used to create the pipeline. */
@property (nonatomic, readonly) RYPipelineDesc desc;

/** Backend-specific pipeline state. Do not free this pointer yourself. */
@property (nonatomic, readonly) void *internalPipeline;

/** Creates a pipeline from a description. Returns nil on failure. */
+ (instancetype)newPipelineWithRenderer:(RYRenderer *)renderer
                                   desc:(const RYPipelineDesc *)desc;

/** Creates a pipeline from a description and reports errors. */
+ (instancetype)newPipelineWithRenderer:(RYRenderer *)renderer
                                   desc:(const RYPipelineDesc *)desc
                                  error:(RYError **)error;

/** Designated initializer. */
- (instancetype)initWithRenderer:(RYRenderer *)renderer
                            desc:(const RYPipelineDesc *)desc
                           error:(RYError **)error;

/**
 * Wraps an existing backend pipeline.
 *
 * Borrows the internal pipeline; the renderer owns it and releases it when the
 * renderer is deallocated. Used for the renderer's built-in pipelines, not by
 * application code.
 */
- (instancetype)initWithInternalPipeline:(void *)internalPipeline
                                renderer:(RYRenderer *)renderer
                                    desc:(const RYPipelineDesc *)desc;

@end
