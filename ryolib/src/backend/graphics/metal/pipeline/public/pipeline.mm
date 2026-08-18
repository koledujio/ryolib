#include "../private.h"

namespace ryo {
    ResourcePool<id<MTLRenderPipelineState>, MAX_PIPELINES> _pipeline_pool;

    static void _pipeline_force_destroy(INDEX idx) {
        if (_pipeline_pool.items[idx]) {
            [_pipeline_pool.items[idx] release];
        }
        _pipeline_pool.items[idx] = nil;
        _pipeline_pool.free_index(idx);
    }

    Pipeline pipeline_create(Renderer renderer, const PipelineDesc& desc) {
        if (_pipeline_pool.active_count >= MAX_PIPELINES) {
            error_set("Maximum number of pipelines reached");
            return {};
        }

        RYO_ENSURE(renderer_is_valid(renderer), {}, "Invalid renderer handle");

        id<MTLDevice> device = _renderer_pool.items[renderer.index()].device;

        id<MTLLibrary> library = _renderer_pool.items[renderer.index()].shaderLibrary; // todo: add shaderLibrary validation to renderer_is_valid
        RYO_ASSERT(library != nullptr, {}, "No shaderLibrary in renderer");

        INDEX idx = _pipeline_pool.allocate_index();
        GENID gen = _pipeline_pool.generations[idx];

        NSString* nameStr = _pipeline_get_shader_name(desc.shader);
        NSString* vertFunNameSuffix = @"_vertex_main";
        NSString* vertFunName = [NSString stringWithFormat:@"%@%@", nameStr, vertFunNameSuffix];
        NSString* fragFunNameSuffix = @"_fragment_main";
        NSString* fragFunName = [NSString stringWithFormat:@"%@%@", nameStr, fragFunNameSuffix];

        id<MTLFunction> vert = [library newFunctionWithName:vertFunName];
        id<MTLFunction> frag = [library newFunctionWithName:fragFunName];

        if (!vert && !frag) {
            error_set("Failed to find metal shader functions(vertex, fragment): %s, %s", vertFunName.UTF8String, fragFunName.UTF8String);
            _pipeline_force_destroy(idx);
            return {};
        } else if (!vert) {
            error_set("Failed to find metal shader vertex function: %s", vertFunName.UTF8String);
            [frag release];
            _pipeline_force_destroy(idx);
            return {};
        } else if (!frag) {
            error_set("Failed to find metal shader fragment function: %s", fragFunName.UTF8String);
            [vert release];
            _pipeline_force_destroy(idx);
            return {};
        }

        MTLVertexDescriptor* vertexDesc = _pipeline_get_vertex_descriptor(desc.vertexLayout);

        MTLRenderPipelineDescriptor* pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
        pipelineDesc.vertexFunction = vert;
        pipelineDesc.vertexDescriptor = vertexDesc;
        pipelineDesc.fragmentFunction = frag;
        pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;

        _pipeline_set_blend_mode(pipelineDesc, desc.blendMode);

        NSError* error = nil;
        id<MTLRenderPipelineState> pso = [device newRenderPipelineStateWithDescriptor:pipelineDesc error:&error];

        if (error) {
            error_set("Failed to create pipeline: %s", error.localizedDescription);
            [vert release];
            [frag release];
            [pipelineDesc release];
            _pipeline_force_destroy(idx);
            return {};
        }

        [vert release];
        [frag release];
        [pipelineDesc release];
        _pipeline_pool.items[idx] = pso;

        return Pipeline::pack(idx, gen);
    }

    void pipeline_destroy(Pipeline pipeline) {
        RYO_ENSURE_VOID(pipeline_is_valid(pipeline), "Invalid pipeline handle");

        INDEX idx = pipeline.index();

       _pipeline_force_destroy(idx);
    }

    bool pipeline_is_valid(Pipeline pipeline) {
        INDEX idx = pipeline.index();

        if (idx >= MAX_PIPELINES)
            return false;
        
        if (!_pipeline_pool.is_valid(idx, pipeline.gen()))
            return false;
        
        return _pipeline_pool.items[idx] != nil;
    }
}
