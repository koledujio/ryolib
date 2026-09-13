#include "../private.h"

namespace ryo {
    bool Renderer::draw_rect_color(const Color& color, const Matrix4& matrix, const DrawSettings* settings) const {
        RYO_ENSURE(valid(), false, "Invalid renderer");
        RYO_ENSURE(internal->inFrame, false, "Renderer not in a frame");
        RYO_ENSURE(internal->inPass, false, "Renderer not in a draw pass");

        const Pipeline* pipeline = _draw_active_pipeline_get(internal->defaultColorPipeline, settings);
        RYO_ASSERT(pipeline != nullptr && pipeline->valid(), false, "Failed to get valid pipeline");

        sceGxmSetVertexProgram(internal->context, pipeline->internal->vertexProgram);
        sceGxmSetFragmentProgram(internal->context, pipeline->internal->fragmentProgram);

        _draw_apply_settings(internal->context, settings);
        _draw_apply_depth_settings(internal->context, settings);

        sceGxmSetFrontStencilRef(internal->context, 0);
        sceGxmSetBackStencilRef(internal->context, 0);

        Matrix4 mvp = _draw_compute_mvp(*internal, matrix, settings);
        
        void* vertexUniforms = nullptr;
        int result = sceGxmReserveVertexDefaultUniformBuffer(internal->context, &vertexUniforms);
        if (result < 0) {
            error_set("sceGxmReserveVertexDefaultUniformBuffer 0x%08X", result);
            return false;
        }
        result = sceGxmSetUniformDataF(vertexUniforms, pipeline->internal->vertexParameters[0], 0, 16, mvp.m);
        if (result < 0) {
            error_set("sceGxmSetUniformDataF: 0x%08X", result);
            return false;
        }

        void* fragmentUniforms = nullptr;
        result = sceGxmReserveFragmentDefaultUniformBuffer(internal->context, &fragmentUniforms);
        if (result < 0) {
            error_set("sceGxmReserveFragmentDefaultUniformBuffer: 0x%08X", result);
            return false;
        }
        float rgba[4] {
            color.r,
            color.g,
            color.b,
            color.a
        };
        result = sceGxmSetUniformDataF(fragmentUniforms, pipeline->internal->fragmentParameters[0], 0, 4, rgba);
        if (result < 0) {
            error_set("sceGxmSetUniformDataF: 0x%08X", result);
            return false;
        }

        if (internal->quadIndexBuffer == nullptr || internal->quadIndexBuffer->internal->mem == nullptr) {
            error_set("Quad index buffer memory is null!");
            return false;
        }

        result = sceGxmSetVertexStream(
            internal->context,
            0,
            internal->quadVertexBuffer->internal->mem
        );

        if (result < 0) {
            error_set("sceGxmSetVertexStream error code: 0x%08X", result);
            return false;
        }

        result = sceGxmDraw(
            internal->context,
            SCE_GXM_PRIMITIVE_TRIANGLES,
            SCE_GXM_INDEX_FORMAT_U32,
            internal->quadIndexBuffer->internal->mem,
            6
        );

        if (result < 0) {
            error_set("sceGxmDraw error code: 0x%08X", result);
            return false;
        }

        return true;
    }
}