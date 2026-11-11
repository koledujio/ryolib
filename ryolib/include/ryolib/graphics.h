#pragma once

#include <cstddef>
#include <stdint.h>

#include "math.h"
#include "templates.h"

namespace ryo {
    /**
     * Forward declarations.
     */

    struct InternalRenderer;
    struct InternalRenderTarget;
    struct InternalTexture;
    struct InternalBuffer;
    struct InternalPipeline;
    struct InternalStencil;

    struct Renderer;

    /**
     * Renderer and color configuration.
     */

    /**
     * @brief Configuration of a renderer's framebuffer and graphics device.
     *
     * @see Renderer
     */
    struct RendererConfig {
        /// On Windows/Mac, the native OS window handle.
        /// On iOS/iPadOS, a native OS window handle with a pre-created metal layer view.
        /// On Playstation Vita, this value is ignored.
        void* nativeWindowHandle;

        /// Window's width in logical pixels, refer to @ref Window::size_get().
        /// On Playstation Vita, this value is ignored and hardcoded to be 960 pixels.
        int width;

        /// Window's height in logical pixels, refer to @ref Window::size_get().
        /// On Playstation Vita, this value is ignored and hardcoded to be 544 pixels.
        int height;

        /// Amount of render pixels per every logical pixels.
        /// Can be retrieved from @ref Window::hidpi_scale_get() if using the ryolib
        /// windowing subsystem.
        /// If the value is less than 1.0f, it's normalized to 1.0f.
        /// On Playstation Vita, this value is ignored.
        float contentScale = 1.0f;
    };

    /**
     * @brief Color, in linear RGBA.
     *
     * Every channel is from 0.0f to 1.0f, where 0.0f is none of the channel and 1.0f is
     * all of it. The alpha channel is only used while blending, refer to @ref BlendMode.
     */
    struct Color {
        float r; ///< Red channel, from 0.0f to 1.0f.
        float g; ///< Green channel, from 0.0f to 1.0f.
        float b; ///< Blue channel, from 0.0f to 1.0f.
        float a; ///< Alpha channel, from 0.0f (transparent) to 1.0f (opaque).
    };

    /**
     * Vertex definitions.
     */

    /**
     * @brief Vertex with a position, the smallest vertex layout.
     *
     * Matches @ref VertexLayoutType::VERTEX_LAYOUT_POSITION.
     */
    struct PositionVertex {
        float position[3]; ///< Position of the vertex in world units.
    };

    /**
     * @brief Vertex with a position and texture coordinates.
     *
     * Matches @ref VertexLayoutType::VERTEX_LAYOUT_POSITION_UV, which is the default
     * vertex layout of a pipeline, refer to @ref PipelineDesc.
     */
    struct PositionUVVertex {
        float position[3]; ///< Position of the vertex in world units.
        float uv[2];       ///< Texture coordinates, from 0.0f to 1.0f.
    };

    /**
     * Camera definitions.
     */

    /**
     * @brief Camera projection types.
     *
     * @see Camera
     * @see CameraProjectionType
     */
    enum CameraProjectionType {
        CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC, ///< Parallel projection without perspective distortion.
        CAMERA_PROJECTION_TYPE_PERSPECTIVE   ///< Projection with perspective distortion.
    };

    /**
     * @brief Camera, the view and projection geometry drawn with.
     *
     * When a camera is provided to a draw call, refer to @ref DrawSettings::camera, the
     * matrix of the geometry is transformed by the camera's view and projection, otherwise
     * it's used as it is, which is how 2D interfaces are usually drawn.
     *
     * @note The projection's aspect ratio is taken from the render target being drawn into,
     * so changing a camera's size has to be followed by redrawing, the aspect ratio itself
     * isn't part of the camera.
     */
    struct Camera {
        /// Position of the camera in world units.
        Vector3 position;

        /**
         * @brief Target the camera looks at.
         *
         * The view matrix is built from the position, the target and the up vector, refer
         * to @ref Matrix4::look_at.
         */
        Vector3 target;

        /// Up direction of the camera, shouldn't be parallel to the view direction.
        Vector3 up;

        /// Projection used for the view.
        CameraProjectionType projectionType;

        /// Vertical field of view in degrees, only used for @ref CameraProjectionType::CAMERA_PROJECTION_TYPE_PERSPECTIVE.
        float fovY;

        /// Height of the view in world units, only used for @ref CameraProjectionType::CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC.
        float orthographicSize;

        /// Distance to the near clipping plane, in world units.
        float nearPlane;

        /// Distance to the far clipping plane, in world units.
        float farPlane;
    };

    /**
     * Pipeline and shader configuration.
     */

    /**
     * @brief Texture sampler types.
     *
     * Texture filtering, the minification (texture smaller on screen) and magnification
     * (texture larger on screen) filters can be picked independently, refer to
     * @ref DrawCall::samplerTypes.
     *
     * @see DrawCall
     * @see SamplerType
     */
    enum SamplerType {
        SAMPLER_TYPE_NEAREST,                ///< Nearest filtering while minifying and magnifying.
        SAMPLER_TYPE_LINEAR,                 ///< Linear filtering while minifying and magnifying.
        SAMPLER_TYPE_MIN_NEAREST_MAG_LINEAR, ///< Nearest filtering while minifying, linear while magnifying.
        SAMPLER_TYPE_MIN_LINEAR_MAG_NEAREST  ///< Linear filtering while minifying, nearest while magnifying.
    };

    /**
     * @brief Buffer types.
     *
     * @see Buffer
     * @see BufferType
     */
    enum BufferType {
        BUFFER_TYPE_VERTEX, ///< Buffer holding vertex data.
        BUFFER_TYPE_INDEX   ///< Buffer holding triangle indices.
    };

    /**
     * @brief Vertex layouts.
     *
     * The layout of the vertex data in a buffer, which has to match the vertex layout of
     * the pipeline the buffer is drawn with, refer to @ref DrawCall and @ref PositionVertex.
     *
     * @see VertexLayoutType
     * @see DrawCall
     */
    enum VertexLayoutType {
        VERTEX_LAYOUT_POSITION,   ///< Positions only, @ref PositionVertex.
        VERTEX_LAYOUT_POSITION_UV ///< Positions and texture coordinates, @ref PositionUVVertex.
    };

    /**
     * @brief Blend modes.
     *
     * How the fragments written by a draw call are combined with the ones already in the
     * render target, refer to @ref PipelineDesc::blendMode.
     *
     * @see BlendMode
     * @see PipelineDesc
     */
    enum BlendMode {
        BLEND_MODE_OPAQUE,  ///< Fragments overwrite the render target.
        BLEND_MODE_ALPHA,   ///< Fragments are blended by their alpha, refer to @ref Color.
        BLEND_MODE_ADDITIVE ///< Fragments are added to the render target.
    };

    /**
     * @brief Built-in shaders.
     *
     * The shaders the built-in pipelines are compiled from, refer to @ref PipelineDesc::shader.
     *
     * @see BuiltinShader
     * @see BuiltinPipeline
     * @see PipelineDesc
     */
    enum BuiltinShader {
        BUILTIN_SHADER_TEXTURE, ///< Shader that samples a texture and tints it with a color.
        BUILTIN_SHADER_COLOR,   ///< Shader that fills geometry with a solid color.
        BUILTIN_SHADER_STENCIL  ///< Shader the stencil shapes are rasterized with.
    };

    /**
     * @brief Built-in pipelines.
     *
     * Built-in pipeline configurations owned by the renderer:
     * - @ref BUILTIN_PIPELINE_COLOR: Vertex layout @ref PositionVertex (@ref VERTEX_LAYOUT_POSITION). Expects MVP matrix (@ref Matrix4) as vertex uniform and tint color (@ref Color) as fragment uniform.
     * - @ref BUILTIN_PIPELINE_TEXTURE: Vertex layout @ref PositionUVVertex (@ref VERTEX_LAYOUT_POSITION_UV). Expects MVP matrix (@ref Matrix4) as vertex uniform, tint color (@ref Color) as fragment uniform, and 1 texture with sampler.
     * - @ref BUILTIN_PIPELINE_STENCIL: Vertex layout @ref PositionVertex (@ref VERTEX_LAYOUT_POSITION). Expects MVP matrix (@ref Matrix4) as vertex uniform.
     *
     * @see Renderer::pipeline_builtin_get()
     * @see BuiltinPipeline
     */
    enum BuiltinPipeline {
        BUILTIN_PIPELINE_COLOR,   ///< Pipeline that draws solid colored geometry.
        BUILTIN_PIPELINE_TEXTURE, ///< Pipeline that draws texture mapped geometry.
        BUILTIN_PIPELINE_STENCIL  ///< Pipeline the stencil shapes are rasterized with.
    };

    /**
     * @brief Description of a pipeline to compile.
     *
     * @see Pipeline
     * @see PipelineDesc
     */
    struct PipelineDesc {
        /// Built-in shader to compile the pipeline from.
        BuiltinShader shader;

        /**
         * @brief Layout of the vertex data drawn with the pipeline.
         *
         * Has to match the layout of the vertex data in the buffers the pipeline is drawn
         * with, refer to @ref DrawCall.
         */
        VertexLayoutType vertexLayout = VERTEX_LAYOUT_POSITION_UV;

        /// How the drawn fragments are combined with the render target, refer to @ref BlendMode.
        BlendMode blendMode = BLEND_MODE_OPAQUE;
    };

    /**
     * Draw and rasterization state.
     */

    /**
     * @brief Face culling modes.
     *
     * Which triangle faces are discarded before rasterization, refer to
     * @ref DrawSettings::cullMode.
     *
     * @see CullMode
     * @see WindingOrder
     */
    enum CullMode {
        CULL_MODE_NONE,  ///< No faces are discarded.
        CULL_MODE_FRONT, ///< Front facing triangles are discarded.
        CULL_MODE_BACK   ///< Back facing triangles are discarded.
    };

    /**
     * @brief Triangle winding orders.
     *
     * Which winding order is the front facing one, refer to @ref DrawSettings::windingOrder.
     *
     * @see WindingOrder
     * @see CullMode
     */
    enum WindingOrder {
        WINDING_ORDER_CLOCKWISE,        ///< Triangles wound clockwise are front facing.
        WINDING_ORDER_COUNTER_CLOCKWISE ///< Triangles wound counter clockwise are front facing.
    };

    /**
     * @brief Triangle fill modes.
     *
     * How triangles are filled after rasterization, refer to @ref DrawSettings::fillMode.
     *
     * @see FillMode
     */
    enum FillMode {
        FILL_MODE_SOLID,    ///< Triangles are filled.
        FILL_MODE_WIREFRAME ///< Triangles are drawn as line outlines of default triangles.
    };

    /**
     * @brief Depth testing and writing modes.
     *
     * How the depth of the drawn fragments is tested against and written to the depth
     * attachment, refer to @ref DrawSettings::depthMode.
     *
     * @see DepthMode
     */
    enum DepthMode {
        DEPTH_MODE_DISABLED,  ///< The depth attachment isn't used.
        DEPTH_MODE_READ,      ///< The depth attachment is tested against but not written to.
        DEPTH_MODE_READ_WRITE ///< The depth attachment is tested against and written to.
    };

    /**
     * Graphics resources.
     */

    /**
     * @brief A 2D image the graphics subsystem can sample from.
     *
     * Textures are created with pixel data and used as the source in a @ref DrawCall,
     * or obtained from a @ref RenderTarget.
     */
    struct Texture {
        InternalTexture* internal = nullptr;

        /**
         * @brief Creates a texture from pixel data and uploads it to the graphics device.
         *
         * @param[in] renderer Renderer to create the texture with.
         * @param[in] pixels   Pixel data, tightly packed rows of RGBA8 pixels (4 bytes each).
         * @param[in] width    Width of the texture in pixels.
         * @param[in] height   Height of the texture in pixels.
         *
         * @pre The @p renderer must be valid (@ref Renderer::valid() == true).
         * @pre The @p pixels pointer has to be non null and point to @p width * @p height
         * tightly packed RGBA8 pixels.
         * @pre @p width and @p height have to be greater than 0.
         * @pre No frame has to be in progress on the @p renderer.
         *
         * @note The pixel data is uploaded right away and doesn't have to be kept around.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see Renderer
         * @see valid()
         * @see error_get()
         */
        Texture(const Renderer& renderer, const void* pixels, unsigned int width, unsigned int height);

        /**
         * @brief Creates a texture that takes over an already existing internal texture.
         *
         * Used internally to expose a render target's textures, refer to
         * @ref RenderTarget::texture_color_get().
         *
         * @param[in] internal Internal texture to take over.
         *
         * @see RenderTarget
         */
        Texture(InternalTexture* internal);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&&);
        Texture& operator=(Texture&&);

        /**
         * @brief Checks if the texture is valid and initialized.
         *
         * @return True if the texture is initialized, false otherwise.
         *
         * @see error_get()
         */
        bool valid() const;
    };

    /**
     * @brief A block of data the graphics subsystem reads while drawing.
     *
     * Buffers hold vertex data or triangle indices and are submitted as part of a
     * @ref DrawCall, refer to @ref BufferType and @ref VertexLayoutType.
     */
    struct Buffer {
        InternalBuffer* internal = nullptr;

        /**
         * @brief Creates a buffer and uploads the data to the graphics device.
         *
         * @param[in] renderer Renderer to create the buffer with.
         * @param[in] data     Data to upload, tightly packed.
         * @param[in] size     Size of the data in bytes.
         * @param[in] type     Type of the buffer.
         *
         * @pre The @p renderer must be valid (@ref Renderer::valid() == true).
         * @pre The @p data pointer has to be non null.
         * @pre @p size has to be greater than 0.
         * @pre No frame has to be in progress on the @p renderer.
         *
         * @note The data is uploaded right away and doesn't have to be kept around.
         * @note The layout of the data is determined by the vertex layout of the pipeline
         * the buffer is drawn with, refer to @ref DrawCall.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see BufferType
         * @see DrawCall
         * @see valid()
         * @see error_get()
         */
        Buffer(const Renderer& renderer, const void* data, size_t size, BufferType type);
        ~Buffer();

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        Buffer(Buffer&&);
        Buffer& operator=(Buffer&&);

        /**
         * @brief Checks if the buffer is valid and initialized.
         *
         * @return True if the buffer is initialized, false otherwise.
         *
         * @see error_get()
         */
        bool valid() const;
    };

    /**
     * @brief Pipeline, the compiled state a draw call is submitted with.
     *
     * A pipeline combines the shader, the vertex layout and the blend mode a draw call is
     * drawn with, refer to @ref PipelineDesc.
     */
    struct Pipeline {
        InternalPipeline* internal = nullptr;

        /**
         * @brief Creates an invalid pipeline.
         *
         * A pipeline that hasn't been compiled yet, which is the same state a moved from
         * pipeline is left in. It holds nothing and has to be assigned a compiled pipeline
         * before it's used, refer to @ref operator=().
         *
         * @note Invalid pipelines fail @ref valid() and can't be drawn with, refer to
         * @ref DrawCall::pipeline and @ref DrawSettings::pipelineOverride.
         *
         * @see valid()
         */
        Pipeline() = default;

        /**
         * @brief Compiles a pipeline for the given description.
         *
         * @param[in] renderer Renderer to create the pipeline with.
         * @param[in] desc     Description of the shader, vertex layout and blend mode.
         *
         * @pre The @p renderer must be valid (@ref Renderer::valid() == true).
         * @pre No frame has to be in progress on the @p renderer.
         *
         * @note The vertex layout of the pipeline determines the layout of the vertex data
         * in the buffer it's drawn with, refer to @ref DrawCall.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see PipelineDesc
         * @see Renderer
         * @see valid()
         * @see error_get()
         */
        Pipeline(const Renderer& renderer, const PipelineDesc& desc);
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        Pipeline& operator=(const Pipeline&) = delete;

        Pipeline(Pipeline&&);
        Pipeline& operator=(Pipeline&&);

        /**
         * @brief Checks if the pipeline is valid and compiled.
         *
         * @return True if the pipeline is compiled and initialized, false otherwise.
         *
         * @note A pipeline is invalid until it's compiled, which is the case for a default
         * constructed one, a moved from one and the one @ref Renderer::pipeline_builtin_get()
         * returns on failure. Invalid pipelines hold nothing and can't be drawn with.
         *
         * @see Pipeline()
         * @see Renderer::pipeline_builtin_get()
         * @see error_get()
         */
        bool valid() const;
    };

    /**
     * @brief A mask of shapes that clips the geometry drawn with it.
     *
     * A stencil collects shapes (rectangles, rounded rectangles and ovals) and masks the
     * geometry of every draw call it is passed to down to the area they cover, refer to
     * @ref DrawCall::stencil and @ref DrawSettings::stencil.
     *
     * The shapes are recorded when they are added and rasterized right before the geometry
     * of each draw call that uses them, so they can be changed between draw calls. Their
     * matrices are used as they are, a camera can be provided to have them transformed by
     * its view and projection like the geometry's matrix.
     *
     * @note A stencil doesn't have to be cleared between draw calls, the shapes it collects
     * are used as they are, which means a shape added once masks everything drawn after it.
     */
    struct Stencil {
        InternalStencil* internal = nullptr;

        /**
         * @brief Creates an empty stencil.
         *
         * @param[in] renderer Renderer that draws the stencil's shapes.
         *
         * @pre The @p renderer must be valid (@ref Renderer::valid() == true).
         *
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see Renderer
         * @see error_get()
         */
        Stencil(const Renderer& renderer);
        ~Stencil();

        Stencil(const Stencil&) = delete;
        Stencil& operator=(const Stencil&) = delete;

        Stencil(Stencil&&);
        Stencil& operator=(Stencil&&);

        /**
         * @brief Checks if the stencil is valid and initialized.
         *
         * @return True if the stencil is initialized, false otherwise.
         *
         * @see Stencil()
         * @see error_get()
         */
        bool valid() const;

        /**
         * @brief Removes all of the stencil's shapes.
         *
         * @pre The stencil must be valid (@ref valid() == true).
         *
         * @return True if the stencil was cleared successfully, false on failure.
         *
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see valid()
         * @see empty()
         * @see error_get()
         */
        bool clear();

        /**
         * @brief Checks if the stencil has any shapes.
         *
         * @pre The stencil must be valid (@ref valid() == true).
         *
         * @return True if the stencil has no shapes, false if it has any, true on failure.
         *
         * @note An empty stencil isn't rasterized and doesn't clip anything.
         *
         * @see valid()
         * @see clear()
         */
        bool empty() const;

        /**
         * @brief Adds a rectangle to the stencil.
         *
         * @param[in] matrix  Transformation matrix of the rectangle.
         * @param[in] camera  Camera to transform the matrix with, or nullptr to use it as it is.
         *
         * @pre The stencil must be valid (@ref valid() == true).
         *
         * @return True if the rectangle was added successfully, false on failure.
         *
         * @note The rectangle is a unit quad scaled by the matrix, refer to @ref Matrix4.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see draw_rounded_rectangle()
         * @see draw_oval()
         * @see Camera
         * @see error_get()
         */
        bool draw_rectangle(const Matrix4& matrix, const Camera* camera = nullptr);

        /**
         * @brief Adds a rectangle with rounded corners to the stencil.
         *
         * @param[in] matrix Transformation matrix of the rectangle.
         * @param[in] radius Radius of the rounded corners, in world units.
         * @param[in] camera Camera to transform the matrix with, or nullptr to use it as it is.
         *
         * @pre The stencil must be valid (@ref valid() == true).
         * @pre @p radius has to be greater than 0.
         *
         * @return True if the rectangle was added successfully, false on failure.
         *
         * @note The rectangle is a unit quad scaled by the matrix, refer to @ref Matrix4.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see draw_rectangle()
         * @see draw_oval()
         * @see Camera
         * @see error_get()
         */
        bool draw_rounded_rectangle(const Matrix4& matrix, float radius, const Camera* camera = nullptr);

        /**
         * @brief Adds an oval to the stencil.
         *
         * @param[in] matrix Transformation matrix of the oval.
         * @param[in] camera Camera to transform the matrix with, or nullptr to use it as it is.
         *
         * @pre The stencil must be valid (@ref valid() == true).
         *
         * @return True if the oval was added successfully, false on failure.
         *
         * @note The oval is a unit quad scaled by the matrix, refer to @ref Matrix4.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see draw_rectangle()
         * @see draw_rounded_rectangle()
         * @see Camera
         * @see error_get()
         */
        bool draw_oval(const Matrix4& matrix, const Camera* camera = nullptr);
    };

    /**
     * @brief Offscreen render target.
     *
     * A color texture with a depth and stencil texture attached to it, which can be rendered
     * into with @ref Renderer::draw_pass_begin() and sampled from afterwards. Useful for
     * rendering into a texture instead of the window, refer to @ref Texture.
     *
     * @see Renderer
     * @see Renderer::draw_pass_begin()
     */
    struct RenderTarget {
        InternalRenderTarget* internal = nullptr;

        /**
         * @brief Creates a render target with the given dimensions.
         *
         * @param[in] renderer Renderer to create the render target's textures with.
         * @param[in] width    Width of the render target in pixels.
         * @param[in] height   Height of the render target in pixels.
         *
         * @pre The @p renderer must be valid (@ref Renderer::valid() == true).
         * @pre @p width and @p height have to be greater than 0.
         *
         * @note The dimensions are in pixels, a render target is not affected by the
         * renderer's content scale, refer to @ref RendererConfig::contentScale.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see Renderer
         * @see valid()
         * @see error_get()
         */
        RenderTarget(const Renderer& renderer, unsigned int width, unsigned int height);
        ~RenderTarget();

        RenderTarget(const RenderTarget&) = delete;
        RenderTarget& operator=(const RenderTarget&) = delete;

        RenderTarget(RenderTarget&&);
        RenderTarget& operator=(RenderTarget&&);

        /**
         * @brief Checks if the render target is valid and initialized.
         *
         * @return True if the render target and its textures are initialized, false otherwise.
         *
         * @see RenderTarget()
         * @see error_get()
         */
        bool valid() const;

        /**
         * @brief Gets the render target's color texture.
         *
         * The texture that was rendered into, which can be sampled from afterwards.
         *
         * @pre The render target must be valid (@ref valid() == true).
         *
         * @return Pointer to the render target's color @ref Texture, or nullptr if invalid.
         *
         * @note The texture is owned by the render target and must not be destroyed, it is
         * invalid after the render target is destroyed.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see Texture
         * @see valid()
         * @see error_get()
         */
        const Texture* texture_color_get() const;

        /**
         * @brief Gets the render target's depth texture.
         *
         * The depth and stencil attachment of the render target.
         *
         * @pre The render target must be valid (@ref valid() == true).
         *
         * @return Pointer to the render target's depth @ref Texture, or nullptr if invalid.
         *
         * @note The texture is owned by the render target and must not be destroyed, it is
         * invalid after the render target is destroyed.
         * @note Its format depends on the platform, refer to @ref Texture.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see Texture
         * @see valid()
         * @see error_get()
         */
        const Texture* texture_depth_get() const;
    };

    /**
     * Draw call definitions.
     */

    /**
     * @brief Draw state and overrides of draw functions other than draw_ex.
     *
     * Passed to the convenience draw functions, which turn it into a @ref DrawCall, which
     * makes it the place for the state that differs between draws. The members that aren't
     * set keep the state the draw function defaults to.
     *
     * @note The camera, pipeline and stencil are not owned and have to stay alive and valid
     * for as long as they are used to draw with.
     *
     * @see Renderer::draw_rect_color()
     * @see DrawCall
     */
    struct DrawSettings {
        /// Which triangle faces are discarded, refer to @ref CullMode.
        CullMode cullMode = CULL_MODE_BACK;

        /// Which winding order is the front facing one, refer to @ref WindingOrder.
        WindingOrder windingOrder = WINDING_ORDER_COUNTER_CLOCKWISE;

        /// How the triangles are filled, refer to @ref FillMode.
        FillMode fillMode = FILL_MODE_SOLID;

        /// How the depth attachment is tested against and written to, refer to @ref DepthMode.
        DepthMode depthMode = DEPTH_MODE_READ_WRITE;

        /**
         * @brief Camera to transform the drawn geometry's matrix with.
         *
         * The geometry's matrix is transformed by the camera's view and projection, refer
         * to @ref Camera.
         */
        const Camera* camera = nullptr;

        /**
         * @brief Pipeline to draw the geometry with instead of the default one.
         *
         * Its vertex layout has to match the vertex data of the geometry, refer to
         * @ref Pipeline and @ref DrawCall.
         */
        const Pipeline* pipelineOverride = nullptr;

        /// Stencil that masks the drawn geometry down to the shapes it collected, refer to @ref Stencil.
        const Stencil* stencil = nullptr;
    };

    /**
     * @brief Everything needed to submit geometry to the graphics device.
     *
     * The low level draw description, refer to @ref Renderer::draw_ex(). The convenience
     * draw functions build one of these from simpler arguments, refer to
     * @ref Renderer::draw_rect_texture() and @ref Renderer::draw_mesh_texture().
     *
     * @note Triangles are the only primitive type, the index buffer has to hold 32bit
     * indices and the vertex data has to match the vertex layout of the pipeline.
     */
    struct DrawCall {
        /// Pipeline the geometry is drawn with, has to be valid, refer to @ref Pipeline.
        const Pipeline& pipeline;

        /// Buffer holding the vertex data, has to be valid, refer to @ref Buffer.
        const Buffer& vertexBuffer;

        /**
         * @brief Distance between the vertices in the vertex buffer in bytes.
         *
         * Only used by the backends that build the vertex layout per draw call, the rest
         * take the stride from the pipeline's vertex layout, refer to @ref VertexLayoutType.
         */
        uint32_t vertexStride = 0;

        /// Buffer holding the 32bit triangle indices, has to be valid, refer to @ref Buffer.
        const Buffer& indexBuffer;

        /// Amount of indices to draw, has to be greater than 0.
        uint32_t indexCount = 0;

        /**
         * @brief Textures to sample from while drawing.
         *
         * Array of @ref textureCount texture pointers, bound to consecutive fragment texture
         * slots in order. Null when the pipeline's shader doesn't sample any.
         */
        const Texture** textures = nullptr;

        /// Array of @ref textureCount filtering modes, one per texture in @ref textures.
        SamplerType* samplerTypes = nullptr;

        /// Amount of entries in the @ref textures and @ref samplerTypes arrays.
        uint32_t textureCount = 0;

        /**
         * @brief Data bound as the vertex shader's uniform.
         *
         * The built-in shaders expect the model view projection matrix here, a
         * @ref Matrix4, refer to @ref DrawSettings::camera.
         */
        const void* vertexUniformData = nullptr;

        /// Size of the @ref vertexUniformData in bytes.
        size_t vertexUniformDataSize = 0;

        /**
         * @brief Data bound as the fragment shader's uniform.
         *
         * The built-in shaders expect the tint color here, a @ref Color, refer to
         * @ref Renderer::draw_rect_texture().
         */
        const void* fragmentUniformData = nullptr;

        /// Size of the @ref fragmentUniformData in bytes.
        size_t fragmentUniformDataSize = 0;

        /// Which triangle faces are discarded, refer to @ref CullMode.
        CullMode cullMode = CULL_MODE_BACK;

        /// Which winding order is the front facing one, refer to @ref WindingOrder.
        WindingOrder windingOrder = WINDING_ORDER_COUNTER_CLOCKWISE;

        /// How the triangles are filled, refer to @ref FillMode.
        FillMode fillMode = FILL_MODE_SOLID;

        /// How the depth attachment is tested against and written to, refer to @ref DepthMode.
        DepthMode depthMode = DEPTH_MODE_READ_WRITE;

        /// Stencil that masks the geometry down to the shapes it collected, refer to @ref Stencil.
        const Stencil* stencil = nullptr;
    };

    /**
     * Main class interfaces.
     */

    /**
     * @brief Owns the graphics device and the window's framebuffer, and everything that has to
     * be submitted as a unit, a frame. Drawing is organized in two nested scopes:
     *
     * - a frame, @ref frame_begin() to @ref frame_end(), which acquires the window's
     *   drawable and presents it,
     * - a render pass inside a frame, @ref draw_pass_begin() to @ref draw_pass_end(),
     *   which renders into the window's framebuffer (a null render target) or into a
     *   @ref RenderTarget.
     *
     * Both scopes have to be entered and left in order, they cannot be nested into
     * themselves and everything submitted outside of them fails.
     *
     * @note Resources created from a renderer (@ref Texture, @ref Buffer, @ref Pipeline,
     * @ref RenderTarget and @ref Stencil) use its device and have to be destroyed before
     * the renderer they were created from.
     *
     * @see RendererConfig
     * @see valid()
     * @see frame_begin()
     * @see frame_end()
     * @see draw_pass_begin()
     * @see draw_pass_end()
     */
    struct Renderer {
        InternalRenderer* internal = nullptr;

        /**
         * @brief Creates a renderer along with its own graphics device.
         *
         * @param[in] config Configuration of the window's framebuffer and the device.
         *
         * @warning Always verify construction success using @ref valid(). In case of
         * failure, call @ref error_get() for error diagnostics and discard the object.
         *
         * @see RendererConfig
         * @see valid()
         * @see error_get()
         */
        Renderer(const RendererConfig& config);

        /**
         * @brief Creates a renderer that shares the device of another renderer.
         *
         * Resources created with this renderer can be used with the @p renderer it shares
         * the device with, and the other way around. Resources like Texture and Buffer created
         * with a renderer can be used with any other renderer with the same graphics device.
         *
         * @param[in] config   Configuration of the window's framebuffer and the device.
         * @param[in] renderer Renderer to share the graphics device with.
         *
         * @pre The @p renderer must be valid (@ref Renderer::valid() == true).
         *
         * @warning Always verify construction success using @ref valid(). In case of
         * failure, call @ref error_get() for error diagnostics and discard the object.
         *
         * @see RendererConfig
         * @see Renderer::valid()
         * @see valid()
         * @see error_get()
         */
        Renderer(const RendererConfig& config, const Renderer& renderer);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        Renderer(Renderer&&);
        Renderer& operator=(Renderer&&);

        /**
         * @brief Checks if the renderer is valid and initialized.
         *
         * @return True if the graphics device and all of the renderer's resources are
         * initialized, false otherwise.
         *
         * @see Renderer()
         * @see error_get()
         */
        bool valid() const;

        /**
         * @brief Resizes the window's framebuffer.
         *
         * The width and height are in logical pixels, so they're multiplied by the renderer's
         * content scale to get the render resolution, refer to @ref RendererConfig::contentScale.
         *
         * @param[in] width  New width in logical pixels.
         * @param[in] height New height in logical pixels.
         *
         * @pre The renderer must be valid (@ref valid() == true).
         * @pre @p width and @p height have to be greater than 0.
         *
         * @return True if the resize completed successfully, false on failure or if the new
         * size is the current one.
         *
         * @note On Playstation Vita, this function call is ignored and always returns true,
         * as the system doesn't support resizing the renderer.
         * @note The platform owns the window's size and can resize it at any time, which the
         * framebuffer follows, so this is only needed when the size has to change ahead of
         * the platform doing it.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see valid()
         * @see error_get()
         * @see size_get()
         */
        bool resize(unsigned int width, unsigned int height);

        /**
         * @brief Gets the window's framebuffer size.
         *
         * @param[out] outWidth  The framebuffer's width in physical pixels.
         * @param[out] outHeight The framebuffer's height in physical pixels.
         *
         * @pre The renderer must be valid (@ref valid() == true).
         *
         * @return True on success, false if the renderer is invalid.
         *
         * @note This is the render resolution, not the window's logical size, it differs from
         * it by the renderer's content scale, refer to @ref RendererConfig::contentScale.
         * @note If the return value is false, the values weren't changed.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see valid()
         * @see resize()
         * @see error_get()
         */
        bool size_get(unsigned int& outWidth, unsigned int& outHeight) const;

        /**
         * @brief Gets the renderer's content scale.
         *
         * The content scale is a multiplier by which values like the logical width and height in
         * @ref resize() are multiplied by and represents the amount of physical pixels per each
         * logical pixel. This value is initially set in @ref RendererConfig::contentScale during
         * initial creation of the renderer. This value is never less than 1.0f.
         *
         * @param[out] outScale Current content scale.
         *
         * @pre The renderer must be valid (@ref valid() == true).
         * 
         * @return True if the content scale output has been written successfully, false otherwise.
         *
         * @note On Playstation Vita, the scale is always 1.0f.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see valid()
         * @see error_get()
         * @see Renderer()
         * @see RendererConfig
         */
        bool content_scale_get(float& outScale);

        /**
         * @brief Sets the renderer's content scale.
         *
         * The content scale is a multiplier by which values like the logical width and height in
         * @ref resize() are multiplied by and represents the amount of physical pixels per each
         * logical pixel. Changing this value to a different one will cause the renderer to resize
         * automatically, since the actual resolution is scaled by this value.
         *
         * @param[in] scale Requested content scale.
         *
         * @pre The renderer must be valid (@ref valid() == true).
         * 
         * @return True if the content scale has been changed successfully or was unaltered, false otherwise.
         *
         * @note On Playstation Vita, this function call will be ignored and return true, as
         * the system doesn't support content scale.
         * @note If the provided value is less than 1.0f, the value is normalized to 1.0f.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see valid()
         * @see error_get()
         * @see Renderer()
         * @see RendererConfig
         */
        bool content_scale_set(float scale);

        /**
         * @brief Begins a frame.
         *
         * Acquires the window's drawable and starts recording into it, everything drawn
         * until @ref frame_end() ends up in it. Has to be called once per frame and can't
         * be called while a frame is already in progress.
         *
         * @pre The renderer must be valid (@ref valid() == true).
         *
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see valid()
         * @see frame_end()
         * @see error_get()
         */
        void frame_begin();

        /**
         * @brief Ends a frame and presents it.
         *
         * Submits everything recorded since @ref frame_begin() and presents it to the
         * window. Has to be called once per frame and can't be called while no frame is in
         * progress.
         *
         * @pre The renderer must be valid (@ref valid() == true).
         * @pre A frame has to be in progress, @ref frame_begin() has to be called first.
         *
         * @return True if the frame was submitted and presented successfully, false on failure.
         *
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see valid()
         * @see frame_begin()
         * @see error_get()
         */
        bool frame_end();

        /**
         * @brief Gets the time between the last two frames.
         *
         * @pre The renderer must be valid (@ref valid() == true).
         *
         * @return The time in seconds between the last two @ref frame_begin() calls, 0.0f on
         * failure or when there hasn't been more than one frame yet.
         *
         * @see valid()
         * @see frame_begin()
         */
        float frame_time_get() const;

        /**
         * @brief Gets a built-in pipeline.
         *
         * The renderer owns a pipeline for each built-in shader, this returns them so that
         * they can be used in @ref DrawCall::pipeline or @ref DrawSettings::pipelineOverride
         * instead of creating an equivalent @ref Pipeline.
         *
         * Built-in pipeline specifics:
         * - @ref BuiltinPipeline::BUILTIN_PIPELINE_COLOR: Vertex layout @ref PositionVertex, vertex uniform @ref Matrix4 (MVP matrix), fragment uniform @ref Color (tint).
         * - @ref BuiltinPipeline::BUILTIN_PIPELINE_TEXTURE: Vertex layout @ref PositionUVVertex, vertex uniform @ref Matrix4 (MVP matrix), fragment uniform @ref Color (tint), 1 texture.
         * - @ref BuiltinPipeline::BUILTIN_PIPELINE_STENCIL: Vertex layout @ref PositionVertex, vertex uniform @ref Matrix4 (MVP matrix).
         *
         * @param[in] builtinPipeline The built-in pipeline to get.
         *
         * @pre The renderer must be valid (@ref valid() == true).
         *
         * @return Const reference to the requested built-in @ref Pipeline, or an invalid one
         * (which fails @ref Pipeline::valid()) if the renderer is invalid.
         *
         * @note The returned pipeline is owned by the renderer and must not be destroyed.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see BuiltinPipeline
         * @see Pipeline
         * @see error_get()
         */
        const Pipeline& pipeline_builtin_get(BuiltinPipeline builtinPipeline) const;

        /**
         * @brief Begins a render pass.
         *
         * Starts recording into the window's framebuffer, or into the color and depth
         * textures of a render target, until @ref draw_pass_end() is called. Has to be
         * called inside a frame and can't be called while a render pass is already open.
         *
         * @param[in] renderTarget Render target to render into, or nullptr for the window's
         * framebuffer.
         * @param[in] clearColor   Color to clear the color, depth and stencil attachments to
         * before rendering, or nullptr to keep their current contents.
         *
         * @pre The renderer must be valid (@ref valid() == true).
         * @pre A frame has to be in progress, @ref frame_begin() has to be called first.
         *
         * @note A render pass has to be opened and closed around the draw calls made into
         * the same attachment, several passes into the same render target can be done in
         * one frame.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see valid()
         * @see draw_pass_end()
         * @see RenderTarget
         * @see error_get()
         */
        void draw_pass_begin(RenderTarget* renderTarget, const Color* clearColor);

        /**
         * @brief Ends the current render pass.
         *
         * Stops recording into the attachments of the render pass opened by
         * @ref draw_pass_begin(). Has to be called inside a frame and can't be called while
         * no render pass is open.
         *
         * @pre The renderer must be valid (@ref valid() == true).
         * @pre A frame has to be in progress, @ref frame_begin() has to be called first.
         * @pre A render pass has to be open, @ref draw_pass_begin() has to be called first.
         *
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see valid()
         * @see draw_pass_begin()
         * @see error_get()
         */
        void draw_pass_end();

        /**
         * @brief Submits a draw call.
         *
         * Draws indexed triangles from the draw call's vertex and index buffers with its
         * pipeline, textures and state, which is what the other draw functions do as well.
         *
         * @param[in] draw The draw call to submit.
         *
         * @pre The renderer must be valid (@ref valid() == true).
         * @pre A frame has to be in progress, @ref frame_begin() has to be called first.
         * @pre A render pass has to be open, @ref draw_pass_begin() has to be called first.
         * @pre The draw call's pipeline, vertex buffer and index buffer must be valid, and
         * @ref DrawCall::indexCount has to be greater than 0.
         *
         * @return True if the draw call was submitted successfully, false on failure.
         *
         * @note Triangles are the only primitive type, refer to @ref DrawCall.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see DrawCall
         * @see valid()
         * @see error_get()
         */
        bool draw_ex(const DrawCall& draw) const;

        /**
         * @brief Draws a solid colored rectangle.
         *
         * Draws a unit quad, transformed by the matrix and filled with the color.
         * By default uses @ref BuiltinPipeline::BUILTIN_PIPELINE_COLOR (or @ref DrawSettings::pipelineOverride),
         * binding the transformed model-view-projection matrix as the vertex uniform and @p color as the fragment uniform.
         *
         * @param[in] color    Color to fill the rectangle with.
         * @param[in] matrix   Transformation matrix of the rectangle.
         * @param[in] settings Draw state and overrides, or nullptr for the defaults.
         *
         * @pre The renderer must be valid (@ref valid() == true).
         * @pre A frame has to be in progress, @ref frame_begin() has to be called first.
         * @pre A render pass has to be open, @ref draw_pass_begin() has to be called first.
         *
         * @return True if the rectangle was drawn successfully, false on failure.
         *
         * @note When @p settings provides a camera, the matrix is transformed by its view and
         * projection, using the aspect ratio of the current render target.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see draw_rect_texture()
         * @see DrawSettings
         * @see Camera
         * @see error_get()
         */
        bool draw_rect_color(const Color& color, const Matrix4& matrix, const DrawSettings* settings = nullptr) const;

        /**
         * @brief Draws a texture mapped rectangle.
         *
         * Draws a unit quad, transformed by the matrix, mapped with the texture's coordinates
         * and tinted with the color.
         * By default uses @ref BuiltinPipeline::BUILTIN_PIPELINE_TEXTURE (or @ref DrawSettings::pipelineOverride),
         * binding the transformed model-view-projection matrix as the vertex uniform, @p color as the fragment uniform,
         * and @p texture with @p samplerType into texture slot 0.
         *
         * @param[in] texture     Texture to map onto the rectangle.
         * @param[in] samplerType Filtering used while sampling the texture.
         * @param[in] color       Color to tint the texture with.
         * @param[in] matrix      Transformation matrix of the rectangle.
         * @param[in] settings    Draw state and overrides, or nullptr for the defaults.
         *
         * @pre The renderer must be valid (@ref valid() == true).
         * @pre The @p texture must be valid (@ref Texture::valid() == true).
         * @pre A frame has to be in progress, @ref frame_begin() has to be called first.
         * @pre A render pass has to be open, @ref draw_pass_begin() has to be called first.
         *
         * @return True if the rectangle was drawn successfully, false on failure.
         *
         * @note When @p settings provides a camera, the matrix is transformed by its view and
         * projection, using the aspect ratio of the current render target.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see draw_rect_color()
         * @see Texture
         * @see SamplerType
         * @see error_get()
         */
        bool draw_rect_texture(const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings = nullptr) const;

        /**
         * @brief Draws a texture mapped mesh.
         *
         * Draws indexed triangles from the vertex and index buffers, transformed by the
         * matrix, mapped with the texture's coordinates and tinted with the color. The vertex
         * data has to match the vertex layout of the pipeline used, refer to @ref DrawCall.
         * By default uses @ref BuiltinPipeline::BUILTIN_PIPELINE_TEXTURE (or @ref DrawSettings::pipelineOverride),
         * binding the transformed model-view-projection matrix as the vertex uniform, @p color as the fragment uniform,
         * and @p texture with @p samplerType into texture slot 0.
         *
         * @param[in] vertexBuffer Buffer holding the vertex data.
         * @param[in] indexBuffer  Buffer holding the triangle indices.
         * @param[in] indexCount   Amount of indices to draw.
         * @param[in] texture      Texture to map onto the mesh.
         * @param[in] samplerType  Filtering used while sampling the texture.
         * @param[in] color        Color to tint the mesh with.
         * @param[in] matrix       Transformation matrix of the mesh.
         * @param[in] settings     Draw state and overrides, or nullptr for the defaults.
         *
         * @pre The renderer must be valid (@ref valid() == true).
         * @pre The @p vertexBuffer and @p indexBuffer must be valid
         * (@ref Buffer::valid() == true).
         * @pre The @p texture must be valid (@ref Texture::valid() == true).
         * @pre @p indexCount has to be greater than 0.
         * @pre A frame has to be in progress, @ref frame_begin() has to be called first.
         * @pre A render pass has to be open, @ref draw_pass_begin() has to be called first.
         *
         * @return True if the mesh was drawn successfully, false on failure.
         *
         * @note The index buffer has to hold 32bit indices.
         * @note When @p settings provides a camera, the matrix is transformed by its view and
         * projection, using the aspect ratio of the current render target.
         * @note On failure, an error message is set and can be retrieved via @ref error_get().
         *
         * @see draw_rect_texture()
         * @see Buffer
         * @see BufferType
         * @see error_get()
         */
        bool draw_mesh_texture(const Buffer& vertexBuffer, const Buffer& indexBuffer, uint32_t indexCount, const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings = nullptr) const;
    };
}