#pragma once

#include <cstddef>
#include <stdint.h>

#include "math.h"
#include "templates.h"

namespace ryo {
    struct InternalRenderer;
    struct InternalRenderTarget;
    struct InternalTexture;
    struct InternalBuffer;
    struct InternalPipeline;

    struct RendererConfig;
    struct RenderTarget;
    struct Color;
    struct DrawSettings;
    struct Texture;
    struct Buffer;
    struct PipelineDesc;

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

    struct Renderer {
        InternalRenderer* internal = nullptr;

        Renderer(const RendererConfig& config); // new device
        Renderer(const RendererConfig& config, const Renderer& renderer); // share device and resources
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        Renderer(Renderer&&);
        Renderer& operator=(Renderer&&);

        bool valid() const;

        bool resize(unsigned int width, unsigned int height);

        void frame_begin();
        bool frame_end();

        float frame_time_get() const; // time between last two frame_begin calls

        void draw_pass_begin(RenderTarget* renderTarget, const Color* clearColor);
        void draw_pass_end();

        bool draw_rect_color(const Color& color, const Matrix4& matrix, const DrawSettings* settings = nullptr) const;
        bool draw_rect_texture(const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings = nullptr) const;
        bool draw_mesh_texture(const Buffer& vertexBuffer, const Buffer& indexBuffer, uint32_t indexCount, const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings = nullptr) const;
    };

    struct RenderTarget {
        InternalRenderTarget* internal = nullptr;

        RenderTarget(const Renderer& renderer, unsigned int width, unsigned int height);
        ~RenderTarget();

        RenderTarget(const RenderTarget*) = delete;
        RenderTarget& operator=(const RenderTarget&) = delete;

        RenderTarget(RenderTarget&&);
        RenderTarget& operator=(RenderTarget&&);

        bool valid() const;

        const Texture* texture_color_get() const;
        const Texture* texture_depth_get() const; 
    };

    struct Texture {
        InternalTexture* internal = nullptr;

        Texture(const Renderer& renderer, const void* pixels, unsigned int width, unsigned int height);
        Texture(InternalTexture* internal);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&&);
        Texture& operator=(Texture&&);

        bool valid() const;
    };

    struct Buffer {
        InternalBuffer* internal = nullptr;

        Buffer(const Renderer& renderer, const void* data, size_t size, BufferType type);
        ~Buffer();

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        Buffer(Buffer&&);
        Buffer& operator=(Buffer&&);

        bool valid() const;
    };

    struct Pipeline {
        InternalPipeline* internal = nullptr;

        Pipeline(const Renderer& renderer, const PipelineDesc& desc);
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        Pipeline& operator=(const Pipeline&) = delete;

        Pipeline(Pipeline&&);
        Pipeline& operator=(Pipeline&&);

        bool valid() const;
    };

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
}