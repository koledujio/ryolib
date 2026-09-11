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
            SCE_GXM_INDEX_FORMAT_U16,
            internal->quadIndexBuffer->internal->mem,
            6
        );

        if (result < 0) {
            error_set("sceGxmDraw error code: 0x%08X", result);
            return false;
        }

        return true;
    }

    bool Renderer::draw_rect_texture(const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings) const {
        RYO_ENSURE(valid(), false, "Invalid renderer");
        RYO_ENSURE(internal->inFrame, false, "Renderer not in a frame");
        RYO_ENSURE(internal->inPass, false, "Renderer not in a draw pass");

        const Pipeline* pipeline = nullptr;
        if (settings != nullptr && settings->pipelineOverride != nullptr && settings->pipelineOverride->valid())
            pipeline = settings->pipelineOverride;
        else {
            pipeline = internal->defaultTexturePipeline;
            if (pipeline == nullptr || !pipeline->valid())
                return false;
        }

        sceGxmSetVertexProgram(internal->context, pipeline->internal->vertexProgram);
        sceGxmSetFragmentProgram(internal->context, pipeline->internal->fragmentProgram);

        sceGxmSetCullMode(internal->context, SCE_GXM_CULL_NONE);
        sceGxmSetFrontStencilRef(internal->context, 0);
        sceGxmSetBackStencilRef(internal->context, 0);

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
        switch (samplerType) {
            case SAMPLER_TYPE_NEAREST:
                sceGxmTextureSetMinFilter(gxmTexture, SCE_GXM_TEXTURE_FILTER_POINT);
                sceGxmTextureSetMagFilter(gxmTexture, SCE_GXM_TEXTURE_FILTER_POINT);
                break;

            case SAMPLER_TYPE_LINEAR:
                sceGxmTextureSetMinFilter(gxmTexture, SCE_GXM_TEXTURE_FILTER_LINEAR);
                sceGxmTextureSetMagFilter(gxmTexture, SCE_GXM_TEXTURE_FILTER_LINEAR);
                break;
        }
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
            SCE_GXM_INDEX_FORMAT_U16,
            internal->quadIndexBufferUV->internal->mem,
            6
        );
        if (drawResult < 0) {
            error_set("sceGxmDraw: 0x%08X", drawResult);
            return false;
        }

        return true;
    }

    bool Renderer::draw_mesh_texture(const Buffer& vertexBuffer, const Buffer& indexBuffer, uint32_t indexCount, const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings) const {return true;}
}