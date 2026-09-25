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
    struct InternalStencil;

    struct RendererConfig;
    struct RenderTarget;
    struct Pipeline;
    struct Color;
    struct DrawSettings;
    struct Texture;
    struct Buffer;
    struct PipelineDesc;
    struct DrawCall;
    struct Camera;

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

    enum BuiltinPipeline {
        BUILTIN_PIPELINE_COLOR,
        BUILTIN_PIPELINE_TEXTURE,
        BUILTIN_PIPELINE_STENCIL
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
        const Pipeline& pipeline_builtin_get(BuiltinPipeline builtinPipeline) const;

        void draw_pass_begin(RenderTarget* renderTarget, const Color* clearColor);
        void draw_pass_end();

        bool draw_ex(const DrawCall& draw) const;
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

    struct Stencil {
        InternalStencil* internal = nullptr;

        Stencil(const Renderer& renderer);
        ~Stencil();

        Stencil(const Stencil&) = delete;
        Stencil& operator=(const Stencil&&) = delete;

        Stencil(Stencil&&);
        Stencil& operator=(Stencil&&);

        bool valid() const;

        bool clear();
        bool empty() const;
        
        bool draw_rectangle(const Matrix4& matrix, const Camera* camera = nullptr);
        bool draw_rounded_rectangle(const Matrix4& matrix, float radius, const Camera* camera = nullptr);
        bool draw_oval(const Matrix4& matrix, const Camera* camera = nullptr);
    };

    struct RendererConfig {
        // the native window handle is ignored on the vita
        // the width and height on the vita are hardcoded to be 960x544
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
        BUILTIN_SHADER_COLOR,
        BUILTIN_SHADER_STENCIL
    };

    struct PipelineDesc {
        BuiltinShader shader;
        VertexLayoutType vertexLayout = VERTEX_LAYOUT_POSITION_UV;
        BlendMode blendMode = BLEND_MODE_OPAQUE;
    };

    enum DepthMode {
        DEPTH_MODE_DISABLED,
        DEPTH_MODE_READ,
        DEPTH_MODE_READ_WRITE
    };

    struct DrawSettings {
        CullMode cullMode = CULL_MODE_BACK;
        WindingOrder windingOrder = WINDING_ORDER_COUNTER_CLOCKWISE;
        FillMode fillMode = FILL_MODE_SOLID;
        DepthMode depthMode = DEPTH_MODE_READ_WRITE;
        
        const Camera* camera = nullptr;
        const Pipeline* pipelineOverride = nullptr;
        const Stencil* stencil = nullptr;
    };

    struct DrawCall {
        const Pipeline& pipeline;

        const Buffer& vertexBuffer;
        uint32_t vertexStride = 0;
        const Buffer& indexBuffer;
        uint32_t indexCount = 0;

        const Texture** textures = nullptr;
        SamplerType* samplerTypes = nullptr;
        uint32_t textureCount = 0;

        const void* vertexUniformData = nullptr;
        size_t vertexUniformDataSize = 0;

        const void* fragmentUniformData = nullptr;
        size_t fragmentUniformDataSize = 0;

        CullMode cullMode = CULL_MODE_BACK;
        WindingOrder windingOrder = WINDING_ORDER_COUNTER_CLOCKWISE;
        FillMode fillMode = FILL_MODE_SOLID;
        DepthMode depthMode = DEPTH_MODE_READ_WRITE;
        
        const Stencil* stencil = nullptr;
    };
}