#include "../private.h"

#include <cmath>

namespace ryo {
    static void _gxm_apply_depth_mode(SceGxmContext* context, DepthMode mode) {
        switch (mode) {
            case DEPTH_MODE_DISABLED:
                sceGxmSetFrontDepthFunc(context, SCE_GXM_DEPTH_FUNC_ALWAYS);
                sceGxmSetBackDepthFunc(context, SCE_GXM_DEPTH_FUNC_ALWAYS);
                sceGxmSetFrontDepthWriteEnable(context, SCE_GXM_DEPTH_WRITE_DISABLED);
                sceGxmSetBackDepthWriteEnable(context, SCE_GXM_DEPTH_WRITE_DISABLED);
                break;

            case DEPTH_MODE_READ:
                sceGxmSetFrontDepthFunc(context, SCE_GXM_DEPTH_FUNC_LESS_EQUAL);
                sceGxmSetBackDepthFunc(context, SCE_GXM_DEPTH_FUNC_LESS_EQUAL);
                sceGxmSetFrontDepthWriteEnable(context, SCE_GXM_DEPTH_WRITE_DISABLED);
                sceGxmSetBackDepthWriteEnable(context, SCE_GXM_DEPTH_WRITE_DISABLED);
                break;

            case DEPTH_MODE_READ_WRITE:
            default:
                sceGxmSetFrontDepthFunc(context, SCE_GXM_DEPTH_FUNC_LESS_EQUAL);
                sceGxmSetBackDepthFunc(context, SCE_GXM_DEPTH_FUNC_LESS_EQUAL);
                sceGxmSetFrontDepthWriteEnable(context, SCE_GXM_DEPTH_WRITE_ENABLED);
                sceGxmSetBackDepthWriteEnable(context, SCE_GXM_DEPTH_WRITE_ENABLED);
                break;
        }
    }

    static void _gxm_apply_rasterizer(SceGxmContext* context, CullMode cull, FillMode fill, WindingOrder winding) {
        if (cull == CULL_MODE_NONE) {
            sceGxmSetCullMode(context, SCE_GXM_CULL_NONE);
        } else {
            bool ccw = (winding == WINDING_ORDER_COUNTER_CLOCKWISE);
            sceGxmSetCullMode(
                context,
                cull == CULL_MODE_BACK ?
                (ccw ? SCE_GXM_CULL_CW : SCE_GXM_CULL_CCW) :
                (ccw ? SCE_GXM_CULL_CCW : SCE_GXM_CULL_CW)
            );
        }

        SceGxmPolygonMode polyMode = (fill == FILL_MODE_WIREFRAME) ? SCE_GXM_POLYGON_MODE_LINE : SCE_GXM_POLYGON_MODE_TRIANGLE_FILL;

        sceGxmSetFrontPolygonMode(context, polyMode);
        sceGxmSetBackPolygonMode(context, polyMode);
    }

    bool Renderer::draw_ex(const DrawCall& draw) const {
        RYO_ENSURE(valid(), false, "Invalid renderer");
        RYO_ENSURE(internal->inFrame, false, "Renderer not in a frame");
        RYO_ENSURE(internal->inPass, false, "Renderer not in a draw pass");
        RYO_ENSURE(draw.pipeline.valid(), false, "Invalid pipeline");
        RYO_ENSURE(draw.vertexBuffer.valid(), false, "Invalid vertex buffer");
        RYO_ENSURE(draw.indexBuffer.valid(), false, "Invalid index buffer");
        RYO_ENSURE(draw.indexCount > 0, false, "Invalid number of indices");

        bool useTextures = false;
        bool useVertexUniforms = false;
        bool useFragmentUniforms = false;
        bool useStencil = false;

        if (draw.textureCount > 0) {
            useTextures = true;

#if defined(RYO_DEBUG) || defined(RYO_SAFETY)
            RYO_ENSURE(draw.textures != nullptr, false, "Invalid texture array");
            RYO_ENSURE(draw.samplerTypes != nullptr, false, "Invalid sampler type array");

            for (int i = 0; i < draw.textureCount; i++) {
                RYO_ENSURE(draw.textures[i]->valid(), false, "Invalid texture");
            }
#endif
        }

        if (draw.vertexUniformDataSize > 0) {
            RYO_ENSURE(draw.vertexUniformData != nullptr, false, "Invalid vertex uniform data");
            useVertexUniforms = true;
        }

        if (draw.fragmentUniformDataSize > 0) {
            RYO_ENSURE(draw.fragmentUniformData != nullptr, false, "Invalid fragment uniform data");
            useFragmentUniforms = true;
        }

        if (draw.stencil != nullptr && !draw.stencil->empty()) {
            RYO_ENSURE(draw.stencil->valid(), false, "Invalid stencil");
            useStencil = !draw.stencil->empty();
        }

        SceGxmContext* context = internal->context;

        if (useStencil) {
            sceGxmSetFrontDepthWriteEnable(context, SCE_GXM_DEPTH_WRITE_DISABLED);
            sceGxmSetBackDepthWriteEnable(context, SCE_GXM_DEPTH_WRITE_DISABLED);
            sceGxmSetFrontDepthFunc(context, SCE_GXM_DEPTH_FUNC_ALWAYS);
            sceGxmSetBackDepthFunc(context, SCE_GXM_DEPTH_FUNC_ALWAYS);

            sceGxmSetFrontStencilFunc(
                context,
                SCE_GXM_STENCIL_FUNC_ALWAYS,
                SCE_GXM_STENCIL_OP_KEEP,
                SCE_GXM_STENCIL_OP_KEEP,
                SCE_GXM_STENCIL_OP_REPLACE,
                0xFF,
                0xFF
            );
            sceGxmSetBackStencilFunc(
                context,
                SCE_GXM_STENCIL_FUNC_ALWAYS,
                SCE_GXM_STENCIL_OP_KEEP,
                SCE_GXM_STENCIL_OP_KEEP,
                SCE_GXM_STENCIL_OP_REPLACE,
                0xFF,
                0xFF
            );
            sceGxmSetFrontStencilRef(context, 1);
            sceGxmSetBackStencilRef(context, 1);

            sceGxmSetVertexProgram(context, internal->stencilPipeline->internal->vertexProgram);
            sceGxmSetFragmentProgram(context, internal->stencilPipeline->internal->fragmentProgram);

            sceGxmSetVertexStream(context, 0, internal->quadVertexBuffer->internal->mem);

            int targetWidth = 0, targetHeight = 0;
            _draw_get_target_size(*internal, targetWidth, targetHeight);
            float aspect = (targetHeight > 0) ? ((float)targetWidth / (float)targetHeight) : 1.0f;

            for (size_t i = 0; i < draw.stencil->internal->commandCount; i++) {
                const StencilCommand& command = draw.stencil->internal->commands[i];

                Matrix4 mvp = command.matrix;
                if (command.hasCamera) {
                    mvp = _draw_compute_camera_vp(command.camera, aspect) * command.matrix;
                }
                Matrix4 mvpTransposed = mvp.transpose();

                void* vUni = nullptr;
                sceGxmReserveVertexDefaultUniformBuffer(context, &vUni);
                sceGxmSetUniformDataF(vUni, internal->stencilPipeline->internal->vertexParameters[0], 0, 16, mvpTransposed.m);

                float sx = sqrtf(command.matrix.m[0] * command.matrix.m[0] + command.matrix.m[1] * command.matrix.m[1] + command.matrix.m[2] * command.matrix.m[2]);
                float sy = sqrtf(command.matrix.m[4] * command.matrix.m[4] + command.matrix.m[5] * command.matrix.m[5] + command.matrix.m[6] * command.matrix.m[6]);
                if (sx <= 0.0f) sx = 1.0f;
                if (sy <= 0.0f) sy = 1.0f;

                void* fUni = nullptr;
                sceGxmReserveFragmentDefaultUniformBuffer(context, &fUni);
                float fragData[4] = {
                    sx,
                    sy,
                    command.radius,
                    static_cast<float>(command.shape)
                };
                sceGxmSetUniformDataF(fUni, internal->stencilPipeline->internal->fragmentParameters[0], 0, 4, fragData);

                sceGxmDraw(context, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U32, internal->quadIndexBuffer->internal->mem, 6);
            }

            sceGxmSetFrontStencilFunc(
                context,
                SCE_GXM_STENCIL_FUNC_EQUAL,
                SCE_GXM_STENCIL_OP_KEEP,
                SCE_GXM_STENCIL_OP_KEEP,
                SCE_GXM_STENCIL_OP_KEEP,
                0xFF,
                0
            );
            sceGxmSetBackStencilFunc(
                context,
                SCE_GXM_STENCIL_FUNC_EQUAL,
                SCE_GXM_STENCIL_OP_KEEP,
                SCE_GXM_STENCIL_OP_KEEP,
                SCE_GXM_STENCIL_OP_KEEP,
                0xFF,
                0
            );
            sceGxmSetFrontStencilRef(context, 1);
            sceGxmSetBackStencilRef(context, 1);
        } else {
            sceGxmSetFrontStencilFunc(
                context,
                SCE_GXM_STENCIL_FUNC_ALWAYS,
                SCE_GXM_STENCIL_OP_KEEP,
                SCE_GXM_STENCIL_OP_KEEP,
                SCE_GXM_STENCIL_OP_KEEP,
                0,
                0
            );
            sceGxmSetBackStencilFunc(
                context,
                SCE_GXM_STENCIL_FUNC_ALWAYS,
                SCE_GXM_STENCIL_OP_KEEP,
                SCE_GXM_STENCIL_OP_KEEP,
                SCE_GXM_STENCIL_OP_KEEP,
                0,
                0
            );
            sceGxmSetFrontStencilRef(context, 0);
            sceGxmSetBackStencilRef(context, 0);
        }

        _gxm_apply_depth_mode(context, draw.depthMode);
        _gxm_apply_rasterizer(context, draw.cullMode, draw.fillMode, draw.windingOrder);

        sceGxmSetVertexProgram(context, draw.pipeline.internal->vertexProgram);
        sceGxmSetFragmentProgram(context, draw.pipeline.internal->fragmentProgram);

        if (useVertexUniforms) {
            void* vUni = nullptr;
            sceGxmReserveVertexDefaultUniformBuffer(context, &vUni);
            sceGxmSetUniformDataF(
                vUni,
                draw.pipeline.internal->vertexParameters[0],
                0,
                static_cast<unsigned int>(draw.vertexUniformDataSize / sizeof(float)),
                static_cast<const float*>(draw.vertexUniformData)
            );
        }

        if (useFragmentUniforms) {
            void* fUni = nullptr;
            sceGxmReserveFragmentDefaultUniformBuffer(context, &fUni);
            sceGxmSetUniformDataF(
                fUni,
                draw.pipeline.internal->fragmentParameters[0],
                0,
                static_cast<unsigned int>(draw.fragmentUniformDataSize / sizeof(float)),
                static_cast<const float*>(draw.fragmentUniformData)
            );
        }

        if (useTextures) {
            for (uint32_t i = 0; i < draw.textureCount; i++) {
                SceGxmTexture* gxmTexture = &draw.textures[i]->internal->texture;
                _draw_apply_sampler_settings(gxmTexture, draw.samplerTypes[i]);
                sceGxmTextureSetUAddrMode(gxmTexture, SCE_GXM_TEXTURE_ADDR_CLAMP);
                sceGxmTextureSetVAddrMode(gxmTexture, SCE_GXM_TEXTURE_ADDR_CLAMP);
                sceGxmSetFragmentTexture(context, i, gxmTexture);
            }
        }

        sceGxmSetVertexStream(context, 0, draw.vertexBuffer.internal->mem);
        sceGxmDraw(
            context,
            SCE_GXM_PRIMITIVE_TRIANGLES,
            SCE_GXM_INDEX_FORMAT_U32,
            draw.indexBuffer.internal->mem,
            draw.indexCount
        );

        return true;
    }
}