#include "../private.h"

@implementation RYMetalGraphicsBackend (Pipeline)

- (void *)createPipelineWithDesc:(const RYPipelineDesc *)desc
                           error:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_internal || !desc) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInvalidRenderer
                                           message:"Invalid Metal backend or pipeline desc"];
        }
        return NULL;
    }

    RYInternalRenderer *intern = (RYInternalRenderer *)_internal;
    id<MTLDevice> device = intern->device;
    id<MTLLibrary> library = intern->shaderLibrary;

    if (!library) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsPipelineCreationFailed
                                           message:"Renderer has no shader library"];
        }
        return NULL;
    }

    NSString *nameStr = _pipeline_get_shader_name(desc->shader);
    if (!nameStr) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsPipelineCreationFailed
                                           message:"Unsupported built-in shader"];
        }
        return NULL;
    }

    NSString *vertFunName = [NSString stringWithFormat:@"%@_vertex_main", nameStr];
    NSString *fragFunName = [NSString stringWithFormat:@"%@_fragment_main", nameStr];

    id<MTLFunction> vert = [library newFunctionWithName:vertFunName];
    id<MTLFunction> frag = [library newFunctionWithName:fragFunName];

    if (!vert || !frag) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsPipelineCreationFailed
                                           message:"Failed to find shader functions"];
        }
        if (vert) [vert release];
        if (frag) [frag release];
        return NULL;
    }

    MTLVertexDescriptor *vertexDesc = _pipeline_get_vertex_descriptor(desc->vertexLayout);
    if (!vertexDesc) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsPipelineCreationFailed
                                           message:"Unsupported vertex layout"];
        }
        [vert release];
        [frag release];
        return NULL;
    }

    MTLRenderPipelineDescriptor *pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineDesc.vertexFunction = vert;
    pipelineDesc.vertexDescriptor = vertexDesc;
    pipelineDesc.fragmentFunction = frag;
    pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
    pipelineDesc.stencilAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;

    if (desc->shader == RYBuiltinShaderStencil) {
        pipelineDesc.colorAttachments[0].writeMask = MTLColorWriteMaskNone;
    } else {
        _pipeline_set_blend_mode(pipelineDesc, desc->blendMode);
    }

    NSError *mtlError = nil;
    id<MTLRenderPipelineState> pso = [device newRenderPipelineStateWithDescriptor:pipelineDesc error:&mtlError];

    [vert release];
    [frag release];
    [pipelineDesc release];

    if (mtlError || !pso) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsPipelineCreationFailed
                                           message:mtlError ? mtlError.localizedDescription.UTF8String : "Failed to create MTLRenderPipelineState"];
        }
        return NULL;
    }

    RYInternalPipeline *pipeline = new RYInternalPipeline;
    pipeline->pso = pso;

    return pipeline;
}

- (void)destroyPipeline:(void *)pipelineHandle {
    if (!pipelineHandle) return;

    RYInternalPipeline *pipeline = (RYInternalPipeline *)pipelineHandle;
    if (pipeline->pso) {
        [pipeline->pso release];
    }
    delete pipeline;
}

@end
