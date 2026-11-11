#import "ryolib/graphics/RYPipeline.h"
#import "ryolib/graphics/RYRenderer.h"

@implementation RYPipeline

@synthesize renderer = _renderer;
@synthesize desc = _desc;

+ (instancetype)newPipelineWithRenderer:(RYRenderer *)renderer
                                   desc:(const RYPipelineDesc *)desc {
    return [self newPipelineWithRenderer:renderer desc:desc error:NULL];
}

+ (instancetype)newPipelineWithRenderer:(RYRenderer *)renderer
                                   desc:(const RYPipelineDesc *)desc
                                  error:(RYError **)error {
    return [[self alloc] initWithRenderer:renderer desc:desc error:error];
}

- (instancetype)initWithRenderer:(RYRenderer *)renderer
                            desc:(const RYPipelineDesc *)desc
                           error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!renderer || !renderer.backendInstance || !desc) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInvalidRenderer
                                           message:"Invalid renderer or desc passed to RYPipeline"];
        }
        return nil;
    }

    void *pipelineHandle = [renderer.backendInstance createPipelineWithDesc:desc error:error];
    if (!pipelineHandle) {
        return nil;
    }

    self = [super init];
    if (!self) {
        [renderer.backendInstance destroyPipeline:pipelineHandle];
        return nil;
    }

    _internal = pipelineHandle;
    _renderer = [renderer retain];
    _desc = *desc;
    _ownsInternal = YES;

    return self;
}

- (instancetype)initWithInternalPipeline:(void *)internalPipeline
                                renderer:(RYRenderer *)renderer
                                    desc:(const RYPipelineDesc *)desc {
    self = [super init];
    if (!self) {
        return nil;
    }

    _internal = internalPipeline;
    _renderer = [renderer retain];
    if (desc) {
        _desc = *desc;
    }
    _ownsInternal = NO;

    return self;
}

- (void)dealloc {
    if (_internal) {
        if (_ownsInternal && _renderer) {
            [_renderer.backendInstance destroyPipeline:_internal];
        }
        _internal = NULL;
    }

    if (_renderer) {
        [_renderer release];
        _renderer = nil;
    }

    [super dealloc];
}

- (void *)internalPipeline {
    return _internal;
}

@end
