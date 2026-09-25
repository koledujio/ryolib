#include "../private.h"

namespace ryo {
    Pipeline::Pipeline(const Renderer& renderer, const PipelineDesc& desc) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid renderer");
        RYO_ENSURE_VOID(!renderer.internal->inFrame, "Renderer is in a frame");

        id<MTLDevice> device = renderer.internal->device;

        id<MTLLibrary> library = renderer.internal->shaderLibrary;
        RYO_ASSERT_VOID(library != nullptr, "No shaderLibrary in renderer");

        NSString* nameStr = _pipeline_get_shader_name(desc.shader);
        NSString* vertFunNameSuffix = @"_vertex_main";
        NSString* vertFunName = [NSString stringWithFormat:@"%@%@", nameStr, vertFunNameSuffix];
        NSString* fragFunNameSuffix = @"_fragment_main";
        NSString* fragFunName = [NSString stringWithFormat:@"%@%@", nameStr, fragFunNameSuffix];

        id<MTLFunction> vert = [library newFunctionWithName:vertFunName];
        id<MTLFunction> frag = [library newFunctionWithName:fragFunName];

        if (!vert && !frag) {
            error_set("Failed to find metal shader functions(vertex, fragment): %s, %s", vertFunName.UTF8String, fragFunName.UTF8String);
            return;
        } else if (!vert) {
            error_set("Failed to find metal shader vertex function: %s", vertFunName.UTF8String);
            [frag release];
            return;
        } else if (!frag) {
            error_set("Failed to find metal shader fragment function: %s", fragFunName.UTF8String);
            [vert release];
            return;
        }

        MTLVertexDescriptor* vertexDesc = _pipeline_get_vertex_descriptor(desc.vertexLayout);

        MTLRenderPipelineDescriptor* pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
        pipelineDesc.vertexFunction = vert;
        pipelineDesc.vertexDescriptor = vertexDesc;
        pipelineDesc.fragmentFunction = frag;
        pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
        pipelineDesc.stencilAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;

        if (desc.shader == BUILTIN_SHADER_STENCIL) {
            pipelineDesc.colorAttachments[0].writeMask = MTLColorWriteMaskNone;
        } else {
            _pipeline_set_blend_mode(pipelineDesc, desc.blendMode);
        }

        NSError* error = nil;
        id<MTLRenderPipelineState> pso = [device newRenderPipelineStateWithDescriptor:pipelineDesc error:&error];

        if (error) {
            error_set("Failed to create pipeline: %s", error.localizedDescription);
            [vert release];
            [frag release];
            [pipelineDesc release];
            return;
        }

        [vert release];
        [frag release];
        [pipelineDesc release];

        InternalPipeline* intern = new InternalPipeline;
        intern->pso = pso;

        internal = intern;
    }

    Pipeline::~Pipeline() {
        if (!internal)
            return;

        if (internal->pso)
            [internal->pso release];
        
        delete internal;
    }

    Pipeline::Pipeline(Pipeline&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Pipeline& Pipeline::operator=(Pipeline&& other) {
        if (this == &other)
            return *this;
        
        if (internal) {
            if (internal->pso)
                [internal->pso release];
            
            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Pipeline::valid() const {
        return internal != nullptr && internal->pso != nil;
    }
}
