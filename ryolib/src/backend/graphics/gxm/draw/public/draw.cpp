#include "../private.h"

#include "ryolib/math.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace ryo {
    void Renderer::draw_pass_begin(RenderTarget* renderTarget, const Color* clearColor) {
        RYO_ENSURE_VOID(valid(), "Invalid renderer");
        RYO_ENSURE_VOID(internal->inFrame, "Renderer not in a frame");
        RYO_ENSURE_VOID(!internal->inPass, "Renderer is already in a draw pass");

        SceGxmRenderTarget* gxmTarget = nullptr;
        SceGxmColorSurface* colorSurface = nullptr;
        SceGxmDepthStencilSurface* depthSurface = nullptr;
        SceGxmSyncObject* sync = nullptr;
        float width = 0.0f;
        float height = 0.0f;

        if (renderTarget != nullptr && renderTarget->valid()) {
            gxmTarget = renderTarget->internal->renderTarget;
            colorSurface = &renderTarget->internal->colorSurface;
            depthSurface = &renderTarget->internal->depthSurface;
            sync = renderTarget->internal->sync;
            width = static_cast<float>(renderTarget->internal->width);
            height = static_cast<float>(renderTarget->internal->height);
            internal->currentTarget = renderTarget;
        } else {
            _DisplayBuffer& buffer = internal->displayBuffers[internal->backBuffer];
            gxmTarget = internal->renderTarget;
            colorSurface = &buffer.surface;
            depthSurface = &internal->depthSurface;
            sync = buffer.sync;
            width = static_cast<float>(internal->width);
            height = static_cast<float>(internal->height);
            internal->currentTarget = nullptr;
        }

        int result = sceGxmBeginScene(
            internal->context,
            0,
            gxmTarget,
            nullptr,
            nullptr,
            sync,
            colorSurface,
            depthSurface
        );
        if (result < 0) {
            error_set("sceGxmBeginScene failed: 0x%08X", result);
            return; // TODO: make this function return a bool
        }

        float xOffset = width * 0.5f;
        float yOffset = height * 0.5f;
        float widthScale = width * 0.5f;
        float heightScale = -height * 0.5f;

        sceGxmSetViewport(internal->context, xOffset, widthScale, yOffset, heightScale, 0.0f, 1.0f);

        internal->inPass = true;

        if (clearColor != nullptr) {
            DrawSettings clearSettings = {};
            clearSettings.cullMode = CULL_MODE_NONE;
            clearSettings.camera = nullptr;

            Matrix4 clearTransform = Matrix4::translation(0.0f, 0.0f, 1.0f) * Matrix4::scaling(2.0f, 2.0f, 1.0f);

            draw_rect_color(*clearColor, clearTransform, &clearSettings);
        }
    }

    void Renderer::draw_pass_end() {
        RYO_ENSURE_VOID(valid(), "Invalid renderer");
        RYO_ENSURE_VOID(internal->inFrame, "Renderer not in a frame");
        RYO_ENSURE_VOID(internal->inPass, "Renderer is not in a draw pass");

        sceGxmEndScene(internal->context, nullptr, nullptr);
        internal->currentTarget = nullptr;
        internal->inPass = false;
    }

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

    bool Renderer::draw_mesh_texture(const Buffer& vertexBuffer, const Buffer& indexBuffer, uint32_t indexCount, const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings) const {
        RYO_ENSURE(valid(), false, "Invalid renderer");
        RYO_ENSURE(internal->inFrame, false, "Renderer not in a frame");
        RYO_ENSURE(internal->inPass, false, "Renderer not in a draw pass");
        RYO_ENSURE(indexBuffer.valid(), false, "Invalid index buffer");
        RYO_ENSURE(vertexBuffer.valid(), false, "Invalid vertex buffer");

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

        int vertexStreamResult = sceGxmSetVertexStream(internal->context, 0, vertexBuffer.internal->mem);
        if (vertexStreamResult < 0) {
            error_set("sceGxmSetVertexStream: 0x%08X", vertexStreamResult);
            return false;
        }

        int drawResult = sceGxmDraw(
            internal->context,
            SCE_GXM_PRIMITIVE_TRIANGLES,
            SCE_GXM_INDEX_FORMAT_U32,
            indexBuffer.internal->mem,
            indexCount
        );
        if (drawResult < 0) {
            error_set("sceGxmDraw: 0x%08X", drawResult);
            return false;
        }

        return true;
    }
}