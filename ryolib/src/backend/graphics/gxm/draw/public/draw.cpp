#include "../../private.h"

#include "ryolib/math.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace ryo {
    void Renderer::draw_pass_begin(RenderTarget* renderTarget, const Color* clearColor) {
        RYO_ENSURE_VOID(valid(), "Invalid renderer");
        RYO_ENSURE_VOID(internal->inFrame, "Renderer not in a frame");
        RYO_ENSURE_VOID(!internal->inPass, "Renderer is already in a draw pass");

        if (true) {
            _DisplayBuffer& buffer = internal->displayBuffers[internal->backBuffer];

            int result = sceGxmBeginScene(
                internal->context,
                0,
                internal->renderTarget,
                nullptr,
                nullptr,
                buffer.sync,
                &buffer.surface,
                &internal->depthSurface
            );

            if (result < 0) {
                FILE* log = fopen("ux0:data/ryolib-draw-pass-begin.log", "a");
                fprintf(log, "sceGxmBeginScene failed 0x%08X\n", result);
                fclose(log);
                return;
            }
        } else {
            return;
        }

        float width = static_cast<float>(internal->width);
        float height = static_cast<float>(internal->height);

        float xOffset = width * 0.5f;
        float yOffset = height * 0.5f;
        float widthScale = width * 0.5f;
        float heightScale = -height * 0.5f;

        sceGxmSetViewport(internal->context, xOffset, widthScale, yOffset, heightScale, 0.0f, 1.0f);

        internal->inPass = true;
    }

    void Renderer::draw_pass_end() {
        RYO_ENSURE_VOID(valid(), "Invalid renderer");
        RYO_ENSURE_VOID(internal->inFrame, "Renderer not in a frame");
        RYO_ENSURE_VOID(internal->inPass, "Renderer is not in a draw pass");

        sceGxmEndScene(internal->context, nullptr, nullptr);
        internal->inPass = false;
    }

    bool Renderer::draw_rect_color(const Color& color, const Matrix4& matrix, const DrawSettings* settings) const {
        RYO_ENSURE(valid(), false, "Invalid renderer");
        RYO_ENSURE(internal->inFrame, false, "Renderer not in a frame");
        RYO_ENSURE(internal->inPass, false, "Renderer not in a draw pass");

        const Pipeline* pipeline = nullptr;
        if (settings != nullptr && settings->pipelineOverride != nullptr && settings->pipelineOverride->valid())
            pipeline = settings->pipelineOverride;
        else {
            pipeline = internal->defaultColorPipeline;
            if (pipeline == nullptr || !pipeline->valid())
                return false;
        }

        sceGxmSetVertexProgram(internal->context, pipeline->internal->vertexProgram);
        sceGxmSetFragmentProgram(internal->context, pipeline->internal->fragmentProgram);

        sceGxmSetCullMode(internal->context, SCE_GXM_CULL_NONE);
        sceGxmSetFrontStencilRef(internal->context, 0);
        sceGxmSetBackStencilRef(internal->context, 0);

        // To be changed when adding render targets
        int width = internal->width;
        int height = internal->height;

        Matrix4 mvp;
        if (settings != nullptr && settings->camera != nullptr) {
            float aspect = static_cast<float>(width) / static_cast<float>(height);

            Matrix4 projection;
            switch (settings->camera->projectionType) {
                case CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC: {
                    float halfHeight = settings->camera->orthographicSize * 0.5f;
                    float halfWidth = halfHeight * aspect;

                    projection = Matrix4::orthographic(-halfWidth, halfWidth, -halfHeight, halfHeight, settings->camera->nearPlane, settings->camera->farPlane);
                    break;
                }

                case CAMERA_PROJECTION_TYPE_PERSPECTIVE:
                    projection = Matrix4::perspective(settings->camera->fovY, aspect, settings->camera->nearPlane, settings->camera->farPlane);
                    break;
            }

            Matrix4 view = Matrix4::look_at(settings->camera->position, settings->camera->target, settings->camera->up);

            mvp = projection * view * matrix;
        } else
            mvp = matrix;
        
        mvp = mvp.transpose();
        
        // void* vertexUniforms = nullptr;
        // int result = sceGxmReserveVertexDefaultUniformBuffer(internal->context, &vertexUniforms);
        // if (result < 0) {
        //     error_set("sceGxmReserveVertexDefaultUniformBuffer 0x%08X", result);
        //     return false;
        // }
        // result = sceGxmSetUniformDataF(vertexUniforms, pipeline->internal->vertexParameters[0], 0, 16, mvp.m);
        // if (result < 0) {
        //     error_set("sceGxmSetUniformDataF: 0x%08X", result);
        //     return false;
        // }

        // void* fragmentUniforms = nullptr;
        // result = sceGxmReserveFragmentDefaultUniformBuffer(internal->context, &fragmentUniforms);
        // if (result < 0) {
        //     error_set("sceGxmReserveFragmentDefaultUniformBuffer: 0x%08X", result);
        //     return false;
        // }
        // float rgba[4] {
        //     color.r,
        //     color.g,
        //     color.b,
        //     color.a
        // };
        // result = sceGxmSetUniformDataF(fragmentUniforms, pipeline->internal->fragmentParameters[0], 0, 4, rgba);
        // if (result < 0) {
        //     error_set("sceGxmSetUniformDataF: 0x%08X", result);
        //     return false;
        // }

        // sceGxmSetVertexStream(internal->context, 0, internal->quadVertexBuffer->internal->mem);

        // int result = sceGxmDraw(internal->context, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U32, internal->quadIndexBuffer->internal->mem, 6);
        // if (result < 0) {
        //     error_set("GXM error code: %d", result);
        //     return false;
        // }

        static const PositionVertex testVertices[] = {
            {-0.5f, -0.5f, 0.0f},
            { 0.5f, -0.5f, 0.0f},
            {-0.5f,  0.5f, 0.0f},
            { 0.5f,  0.5f, 0.0f}
        };

        static const uint16_t testIndices[] = {
            0, 1, 2,
            2, 1, 3
        };

        void* testVB = nullptr;
        SceUID testVBId = -1;

        _memory_alloc_gpu_rw(
            "test_vb",
            sizeof(testVertices),
            _MEMORY_TYPE_UNCACHE,
            testVB,
            testVBId
        );

        memcpy(testVB, testVertices, sizeof(testVertices));

        void* testIB = nullptr;
        SceUID testIBId = -1;

        _memory_alloc_gpu_rw(
            "test_ib",
            sizeof(testIndices),
            _MEMORY_TYPE_UNCACHE,
            testIB,
            testIBId
        );

        memcpy(testIB, testIndices, sizeof(testIndices));

        int result = sceGxmSetVertexStream(
            internal->context,
            0,
            testVB
        );

        if (result < 0) {
            error_set("sceGxmSetVertexStream error code: %d", result);
            return false;
        }

        result = sceGxmDraw(
            internal->context,
            SCE_GXM_PRIMITIVE_TRIANGLES,
            SCE_GXM_INDEX_FORMAT_U16,
            testIB,
            6
        );
        if (result < 0) {
            error_set("sceGxmDraw error code: %d", result);
            return false;
        }

        // if (internal->quadIndexBuffer == nullptr || internal->quadIndexBuffer->internal->mem == nullptr) {
        //     error_set("Quad index buffer memory is null!");
        //     return false;
        // }

        // result = sceGxmSetVertexStream(
        //     internal->context,
        //     0,
        //     internal->quadVertexBuffer->internal->mem
        // );

        // if (result < 0) {
        //     error_set("sceGxmSetVertexStream error code: 0x%08X", result);
        //     return false;
        // }

        // result = sceGxmDraw(
        //     internal->context,
        //     SCE_GXM_PRIMITIVE_TRIANGLES,
        //     SCE_GXM_INDEX_FORMAT_U16,
        //     internal->quadIndexBuffer->internal->mem,
        //     6
        // );

        // if (result < 0) {
        //     static bool logged = false;
        //     if (!logged) {
        //         FILE* dbgLog = fopen("ux0:data/ryolib-draw-debug.log", "w");
        //         if (dbgLog) {
        //             fprintf(dbgLog, "=== GXM Draw Debug Info ===\n");
        //             fprintf(dbgLog, "Context: %p\n", internal->context);
        //             fprintf(dbgLog, "VertexProgram: %p\n", pipeline->internal->vertexProgram);
        //             fprintf(dbgLog, "FragmentProgram: %p\n", pipeline->internal->fragmentProgram);
        //             fprintf(dbgLog, "VertexParam[0]: %p\n", pipeline->internal->vertexParameters ? pipeline->internal->vertexParameters[0] : nullptr);
        //             fprintf(dbgLog, "FragmentParam[0]: %p\n", pipeline->internal->fragmentParameters ? pipeline->internal->fragmentParameters[0] : nullptr);
        //             fprintf(dbgLog, "VertexUniforms: %p\n", vertexUniforms);
        //             fprintf(dbgLog, "FragmentUniforms: %p\n", fragmentUniforms);
        //             fprintf(dbgLog, "QuadVB mem: %p\n", internal->quadVertexBuffer ? internal->quadVertexBuffer->internal->mem : nullptr);
        //             fprintf(dbgLog, "QuadIB mem: %p\n", internal->quadIndexBuffer ? internal->quadIndexBuffer->internal->mem : nullptr);
        //             fprintf(dbgLog, "sceGxmDraw result: 0x%08X (%d)\n", result, result);
        //             fclose(dbgLog);
        //         }
        //         logged = true;
        //     }
        //     error_set("sceGxmDraw error code: 0x%08X", result);
        //     return false;
        // }

        return true;
    }

    bool Renderer::draw_rect_texture(const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings) const {return true;}
    bool Renderer::draw_mesh_texture(const Buffer& vertexBuffer, const Buffer& indexBuffer, uint32_t indexCount, const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings) const {return true;}
}