#include "../private.h"

namespace ryo {
    bool Renderer::draw_rect_texture(const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings) const {
        RYO_ENSURE(valid(), false, "Invalid renderer");
        RYO_ENSURE(internal->inFrame, false, "Renderer not in a frame");
        RYO_ENSURE(internal->inPass, false, "Renderer not in a draw pass");

        const Pipeline* pipeline = _draw_active_pipeline_get(internal->defaultTexturePipeline, settings);
        RYO_ASSERT(pipeline != nullptr && pipeline->valid(), false, "Failed to get valid pipeline");

        sceGxmSetVertexProgram(internal->context, pipeline->internal->vertexProgram);
        sceGxmSetFragmentProgram(internal->context, pipeline->internal->fragmentProgram);

        _draw_apply_settings(internal->context, settings);
        _draw_apply_depth_settings(internal->context, settings);

        sceGxmSetFrontStencilRef(internal->context, 0);
        sceGxmSetBackStencilRef(internal->context, 0);

        Matrix4 mvp = _draw_compute_mvp(*internal, matrix, settings);

        void* vertexUniforms = nullptr;
        int vertexUniformResult = sceGxmReserveVertexDefaultUniformBuffer(internal->context, &vertexUniforms);
        if (vertexUniformResult < 0) {
            error_set("sceGxmReserveVertexDefaultUniformBuffer 0x%08X", vertexUniformResult);
            return false;
        }
        vertexUniformResult = sceGxmSetUniformDataF(vertexUniforms, pipeline->internal->vertexParameters[0], 0, 16, mvp.m);
        if (vertexUniformResult < 0) {
            error_set("sceGxmSetUniformDataF: 0x%08X", vertexUniformResult);
            return false;
        }

        void* fragmentUniforms = nullptr;
        int fragmentUniformResult = sceGxmReserveFragmentDefaultUniformBuffer(internal->context, &fragmentUniforms);
        if (fragmentUniformResult < 0) {
            error_set("sceGxmReserveFragmentDefaultUniformBuffer: 0x%08X", fragmentUniformResult);
            return false;
        }
        float rgba[4] {
            color.r,
            color.g,
            color.b,
            color.a
        };
        fragmentUniformResult = sceGxmSetUniformDataF(fragmentUniforms, pipeline->internal->fragmentParameters[0], 0, 4, rgba);
        if (fragmentUniformResult < 0) {
            error_set("sceGxmSetUniformDataF: 0x%08X", fragmentUniformResult);
            return false;
        }

        SceGxmTexture* gxmTexture = &texture.internal->texture;

        _draw_apply_sampler_settings(gxmTexture, samplerType);

        sceGxmTextureSetUAddrMode(gxmTexture, SCE_GXM_TEXTURE_ADDR_CLAMP);
        sceGxmTextureSetVAddrMode(gxmTexture, SCE_GXM_TEXTURE_ADDR_CLAMP);

        int samplerResult = sceGxmSetFragmentTexture(internal->context, 0, gxmTexture);
        if (samplerResult < 0) {
            error_set("sceGxmSetFragmentTexture: 0x%08X", samplerResult);
            return false;
        }

        RYO_ENSURE(
            internal->quadVertexBufferUV != nullptr &&
            internal->quadVertexBufferUV->internal->mem != nullptr,
            false,
            "Quad UV vertex buffer is null"
        );
        RYO_ENSURE(
            internal->quadIndexBufferUV != nullptr &&
            internal->quadIndexBufferUV->internal->mem != nullptr,
            false,
            "Quad UV index buffer is null"
        );

        int vertexStreamResult = sceGxmSetVertexStream(internal->context, 0, internal->quadVertexBufferUV->internal->mem);
        if (vertexStreamResult < 0) {
            error_set("sceGxmSetVertexStream: 0x%08X", vertexStreamResult);
            return false;
        }

        int drawResult = sceGxmDraw(
            internal->context,
            SCE_GXM_PRIMITIVE_TRIANGLES,
            SCE_GXM_INDEX_FORMAT_U32,
            internal->quadIndexBufferUV->internal->mem,
            6
        );
        if (drawResult < 0) {
            error_set("sceGxmDraw: 0x%08X", drawResult);
            return false;
        }

        return true;
    }
}