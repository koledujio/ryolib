#include "../private.h"

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

    static void _gxm_stencil_func(
        SceGxmContext* context,
        SceGxmStencilFunc func,
        SceGxmStencilOp stencilFail,
        SceGxmStencilOp depthFail,
        SceGxmStencilOp depthPass,
        unsigned char compareMask,
        unsigned char writeMask
    ) {
        sceGxmSetFrontStencilFunc(context, func, stencilFail, depthFail, depthPass, compareMask, writeMask);
        sceGxmSetBackStencilFunc(context, func, stencilFail, depthFail, depthPass, compareMask, writeMask);
    }

    static void _gxm_stencil_ref(SceGxmContext* context, unsigned int ref) {
        sceGxmSetFrontStencilRef(context, ref);
        sceGxmSetBackStencilRef(context, ref);
    }

    static void _gxm_stencil_write_state(SceGxmContext* context, unsigned int ref) {
        _gxm_stencil_func(
            context,
            SCE_GXM_STENCIL_FUNC_ALWAYS,
            SCE_GXM_STENCIL_OP_KEEP,
            SCE_GXM_STENCIL_OP_KEEP,
            SCE_GXM_STENCIL_OP_REPLACE,
            0xFF,
            0xFF
        );
        _gxm_stencil_ref(context, ref);
    }

    static void _gxm_stencil_test_state(SceGxmContext* context, unsigned int ref) {
        _gxm_stencil_func(
            context,
            SCE_GXM_STENCIL_FUNC_EQUAL,
            SCE_GXM_STENCIL_OP_KEEP,
            SCE_GXM_STENCIL_OP_KEEP,
            SCE_GXM_STENCIL_OP_KEEP,
            0xFF,
            0xFF
        );
        _gxm_stencil_ref(context, ref);
    }

    static void _gxm_stencil_disabled_state(SceGxmContext* context) {
        _gxm_stencil_func(
            context,
            SCE_GXM_STENCIL_FUNC_ALWAYS,
            SCE_GXM_STENCIL_OP_KEEP,
            SCE_GXM_STENCIL_OP_KEEP,
            SCE_GXM_STENCIL_OP_KEEP,
            0xFF,
            0xFF
        );
        _gxm_stencil_ref(context, 0);
    }

    static void _gxm_stencil_clear(InternalRenderer* internal) {
        SceGxmContext* context = internal->context;
        const Pipeline* pipeline = internal->stencilPipeline;

        _gxm_stencil_func(
            context,
            SCE_GXM_STENCIL_FUNC_NEVER,
            SCE_GXM_STENCIL_OP_ZERO,
            SCE_GXM_STENCIL_OP_ZERO,
            SCE_GXM_STENCIL_OP_ZERO,
            0xFF,
            0xFF
        );
        _gxm_stencil_ref(context, 0);

        sceGxmSetCullMode(context, SCE_GXM_CULL_NONE);
        sceGxmSetFrontPolygonMode(context, SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);
        sceGxmSetBackPolygonMode(context, SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);

        sceGxmSetVertexProgram(context, pipeline->internal->vertexProgram);
        sceGxmSetFragmentProgram(context, pipeline->internal->fragmentProgram);
        sceGxmSetVertexStream(context, 0, internal->quadVertexBuffer->internal->mem);

        Matrix4 mvp = Matrix4::scaling(2.0f, 2.0f, 1.0f).transpose();

        void* vertexUniform = nullptr;
        sceGxmReserveVertexDefaultUniformBuffer(context, &vertexUniform);
        sceGxmSetUniformDataF(vertexUniform, pipeline->internal->vertexParameters[0], 0, 16, mvp.m);

        const float fragmentData[4] = {
            1.0f,
            1.0f,
            0.0f,
            static_cast<float>(_STENCIL_SHAPE_RECTANGLE)
        };

        void* fragmentUniform = nullptr;
        sceGxmReserveFragmentDefaultUniformBuffer(context, &fragmentUniform);
        sceGxmSetUniformDataF(fragmentUniform, pipeline->internal->fragmentParameters[0], 0, 4, fragmentData);

        sceGxmDraw(
            context,
            SCE_GXM_PRIMITIVE_TRIANGLES,
            SCE_GXM_INDEX_FORMAT_U32,
            internal->quadIndexBuffer->internal->mem,
            6
        );
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
            RYO_ENSURE(internal->stencilPipeline != nullptr && internal->stencilPipeline->valid(), false, "Invalid stencil pipeline");
            RYO_ENSURE(internal->quadVertexBuffer != nullptr && internal->quadVertexBuffer->valid(), false, "Invalid quad vertex buffer");
            RYO_ENSURE(internal->quadIndexBuffer != nullptr && internal->quadIndexBuffer->valid(), false, "Invalid quad index buffer");

            sceGxmSetFrontDepthWriteEnable(context, SCE_GXM_DEPTH_WRITE_DISABLED);
            sceGxmSetBackDepthWriteEnable(context, SCE_GXM_DEPTH_WRITE_DISABLED);
            sceGxmSetFrontDepthFunc(context, SCE_GXM_DEPTH_FUNC_ALWAYS);
            sceGxmSetBackDepthFunc(context, SCE_GXM_DEPTH_FUNC_ALWAYS);

            _gxm_stencil_clear(internal);

            _gxm_stencil_write_state(context, 1);

            sceGxmSetCullMode(context, SCE_GXM_CULL_NONE);
            sceGxmSetFrontPolygonMode(context, SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);
            sceGxmSetBackPolygonMode(context, SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);

            sceGxmSetVertexProgram(context, internal->stencilPipeline->internal->vertexProgram);
            sceGxmSetFragmentProgram(context, internal->stencilPipeline->internal->fragmentProgram);

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

                void* fUni = nullptr;
                sceGxmReserveFragmentDefaultUniformBuffer(context, &fUni);
                const float fragData[4] = { 1.0f, 1.0f, 0.0f, static_cast<float>(_STENCIL_SHAPE_RECTANGLE) };
                sceGxmSetUniformDataF(fUni, internal->stencilPipeline->internal->fragmentParameters[0], 0, 4, fragData);

                if (command.vertexBuffer != nullptr && command.indexBuffer != nullptr && command.indexCount > 0) {
                    sceGxmSetVertexStream(context, 0, command.vertexBuffer->internal->mem);
                    sceGxmDraw(context, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U32, command.indexBuffer->internal->mem, command.indexCount);
                } else {
                    sceGxmSetVertexStream(context, 0, internal->quadVertexBuffer->internal->mem);
                    sceGxmDraw(context, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U32, internal->quadIndexBuffer->internal->mem, 6);
                }
            }

            _gxm_stencil_test_state(context, 1);
        } else {
            _gxm_stencil_disabled_state(context);
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