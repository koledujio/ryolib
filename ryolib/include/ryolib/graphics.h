#pragma once

#include <cstddef>
#include <stdint.h>

#include "math.h"
#include "templates.h"

namespace ryo {
    using Renderer = Handle<struct RendererTag>;
    using RenderTarget = Handle<struct RenderTargetTag>;
    using Texture = Handle<struct TextureTag>;
    using Buffer = Handle<struct BufferTag>;
    using Pipeline = Handle<struct PipelineTag>;

    struct RendererConfig {
        void* nativeWindowHandle;
        int width;
        int height;
    };

    struct Color {
        float r, g, b, a;
    };

    struct PositionVertex {
        float position[3];
    };
    struct PositionUVVertex {
        float position[3];
        float uv[2];
    };

    enum SamplerType {
        SAMPLER_TYPE_NEAREST,
        SAMPLER_TYPE_LINEAR,
        SAMPLER_TYPE_MIN_NEAREST_MAG_LINEAR,
        SAMPLER_TYPE_MIN_LINEAR_MAG_NEAREST
    };

    enum BufferType {
        BUFFER_TYPE_VERTEX,
        BUFFER_TYPE_INDEX
    };

    enum CameraProjectionType {
        CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC,
        CAMERA_PROJECTION_TYPE_PERSPECTIVE
    };

    struct Camera {
        Vector3 position;
        Vector3 target;
        Vector3 up;

        CameraProjectionType projectionType;
        float fovY; // for perspective
        float orthographicSize; // for orthographic
        float nearPlane;
        float farPlane;
    };

    enum CullMode {
        CULL_MODE_NONE,
        CULL_MODE_FRONT,
        CULL_MODE_BACK
    };

    enum WindingOrder {
        WINDING_ORDER_CLOCKWISE,
        WINDING_ORDER_COUNTER_CLOCKWISE
    };

    enum FillMode {
        FILL_MODE_SOLID,
        FILL_MODE_WIREFRAME
    };

    enum VertexLayoutType {
        VERTEX_LAYOUT_POSITION,
        VERTEX_LAYOUT_POSITION_UV
    };

    enum BlendMode {
        BLEND_MODE_OPAQUE,
        BLEND_MODE_ALPHA,
        BLEND_MODE_ADDITIVE
    };

    enum BuiltinShader {
        BUILTIN_SHADER_TEXTURE,
        BUILTIN_SHADER_COLOR
    };

    struct PipelineDesc {
        BuiltinShader shader;
        VertexLayoutType vertexLayout = VERTEX_LAYOUT_POSITION_UV;
        BlendMode blendMode = BLEND_MODE_OPAQUE;
    };

    struct DrawSettings {
        CullMode cullMode = CULL_MODE_BACK;
        WindingOrder windingOrder = WINDING_ORDER_COUNTER_CLOCKWISE;
        FillMode fillMode = FILL_MODE_SOLID;
        const Camera* camera = nullptr;
        const Pipeline* pipelineOverride = nullptr;
    };

    Renderer renderer_create(const RendererConfig& config);
    Renderer renderer_create_share_resources(const RendererConfig& config, Renderer renderer);
    void renderer_destroy(Renderer renderer);
    bool renderer_is_valid(Renderer renderer);
    bool renderer_resize(Renderer renderer, int width, int height);

    void frame_begin(Renderer renderer);
    bool frame_end(Renderer renderer);
    float frame_get_delta_time(Renderer renderer);

    void draw_pass_begin(Renderer renderer, RenderTarget* target, const Color* clearColor);
    void draw_pass_end(Renderer renderer);
    bool draw_rect_color(Renderer renderer, const Color& color, const Matrix4& matrix, const DrawSettings* settings = nullptr);
    bool draw_rect_texture(Renderer renderer, Texture texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings = nullptr);
    bool draw_mesh_texture(Renderer renderer, Buffer vertexBuffer, Buffer indexBuffer, uint32_t indexCount, Texture texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings = nullptr);

    RenderTarget render_target_create(Renderer renderer, int width, int height);
    void render_target_destroy(RenderTarget target);
    bool render_target_is_valid(RenderTarget target);
    Texture render_target_get_color_texture(RenderTarget target);
    Texture render_target_get_depth_texture(RenderTarget target);

    Texture texture_create(Renderer renderer, const void* pixels, int width, int height);
    void texture_destroy(Texture texture);
    bool texture_is_valid(Texture texture);

    Buffer buffer_create(Renderer renderer, const void* data, size_t size, BufferType type);
    void buffer_destroy(Buffer buffer);
    bool buffer_is_valid(Buffer buffer);

    Pipeline pipeline_create(Renderer renderer, const PipelineDesc& desc);
    void pipeline_destroy(Pipeline pipeline);
    bool pipeline_is_valid(Pipeline pipeline);
}