#include "../private.h"

namespace ryo {
    bool _renderer_pipeline_init(Renderer& renderer, InternalRenderer& internalRenderer) {
        NSError* error = nil;
        id<MTLLibrary> library = nil;

        NSString* bundlePath = [[NSBundle mainBundle] pathForResource:@"shaders" ofType:@"metallib"];
        if (bundlePath) {
            library = [internalRenderer.device newLibraryWithURL:[NSURL fileURLWithPath:bundlePath] error:&error];
        }

        // todo add proper error logging here

        if(!library) {
            NSString* execDir = [[NSBundle mainBundle] resourcePath];
            NSString* fallbackPath = [execDir stringByAppendingPathComponent:@"shaders.metallib"];
            NSURL* fallbackURL = [NSURL fileURLWithPath:fallbackPath];

            library = [internalRenderer.device newLibraryWithURL:fallbackURL error:&error];
        }

        if (!library) {
            error_set("Failed to locate shaders.metallib");
            return false;
        }

        internalRenderer.shaderLibrary = library;

        PipelineDesc colorDesc = {};
        colorDesc.shader = BUILTIN_SHADER_COLOR;
        colorDesc.vertexLayout = VERTEX_LAYOUT_POSITION;
        colorDesc.blendMode = BLEND_MODE_OPAQUE;
        internalRenderer.defaultColorPipeline = new Pipeline(renderer, colorDesc);
        RYO_ASSERT_NORETURN(internalRenderer.defaultColorPipeline->valid(), "Failed to create default color pipeline");

        PipelineDesc textureDesc = {};
        textureDesc.shader = BUILTIN_SHADER_TEXTURE;
        textureDesc.vertexLayout = VERTEX_LAYOUT_POSITION_UV;
        textureDesc.blendMode = BLEND_MODE_OPAQUE;
        internalRenderer.defaultTexturePipeline = new Pipeline(renderer, textureDesc);
        RYO_ASSERT_NORETURN(internalRenderer.defaultTexturePipeline->valid(), "Failed to create default texture pipeline");

        PipelineDesc stencilDesc = {};
        stencilDesc.shader = BUILTIN_SHADER_STENCIL;
        stencilDesc.vertexLayout = VERTEX_LAYOUT_POSITION;
        stencilDesc.blendMode = BLEND_MODE_OPAQUE;
        internalRenderer.stencilPipeline = new Pipeline(renderer, stencilDesc);
        RYO_ASSERT_NORETURN(internalRenderer.stencilPipeline->valid(), "Failed to create stencil pipeline");

        return true;
    }
}