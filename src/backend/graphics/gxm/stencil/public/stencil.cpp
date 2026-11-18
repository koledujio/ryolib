#include "../private.h"

#include <cmath>
#include <cstdlib>
#include <vector>

namespace ryo {
    static void _stencil_command_release_geometry(StencilCommand& command) {
        delete command.vertexBuffer;
        command.vertexBuffer = nullptr;

        delete command.indexBuffer;
        command.indexBuffer = nullptr;

        command.indexCount = 0;
    }

    static bool _stencil_command_add(InternalStencil* stencil, StencilCommand command) {
        if (stencil->commandCount >= stencil->commandCapacity) {
            const size_t newCapacity = stencil->commandCapacity == 0 ? 8 : stencil->commandCapacity + 8;

            StencilCommand* newCommands = static_cast<StencilCommand*>(realloc(stencil->commands, sizeof(StencilCommand) * newCapacity));

            if (!newCommands)
                return false;

            stencil->commands = newCommands;
            stencil->commandCapacity = newCapacity;
        }

        stencil->commands[stencil->commandCount++] = command;

        return true;
    }

    static void _stencil_destroy(InternalStencil* stencil) {
        if (!stencil)
            return;

        for (size_t i = 0; i < stencil->commandCount; i++)
            _stencil_command_release_geometry(stencil->commands[i]);

        if (stencil->commands)
            free(stencil->commands);

        delete stencil;
    }

    /*
     * GXM masks are drawn as geometry instead of being cut out with fragment
     * discard, so the outlines below have to match the shapes the Metal and
     * D3D11 backends produce with their SDF shaders. All coordinates are in the
     * local space of the stencil quad, which spans [-0.5, 0.5] on both axes.
     */
    static void _stencil_append_vertex(std::vector<PositionVertex>& vertices, float x, float y) {
        PositionVertex vertex = {};
        vertex.position[0] = x;
        vertex.position[1] = y;
        vertex.position[2] = 0.0f;
        vertices.push_back(vertex);
    }

    static void _stencil_build_rectangle(std::vector<PositionVertex>& vertices) {
        _stencil_append_vertex(vertices, 0.5f, -0.5f);
        _stencil_append_vertex(vertices, 0.5f, 0.5f);
        _stencil_append_vertex(vertices, -0.5f, 0.5f);
        _stencil_append_vertex(vertices, -0.5f, -0.5f);
    }

    static void _stencil_build_rounded_rectangle(std::vector<PositionVertex>& vertices, float radiusX, float radiusY) {
        const float halfSize = 0.5f;

        if (radiusX > halfSize) radiusX = halfSize;
        if (radiusY > halfSize) radiusY = halfSize;

        if (radiusX <= 0.0f || radiusY <= 0.0f) {
            _stencil_build_rectangle(vertices);
            return;
        }

        const float innerX = halfSize - radiusX;
        const float innerY = halfSize - radiusY;
        const float halfPi = 1.57079632679f;
        const int segments = 12;

        // Counter-clockwise, starting at the bottom-right corner.
        const float centerX[4] = { innerX, innerX, -innerX, -innerX };
        const float centerY[4] = { -innerY, innerY, innerY, -innerY };
        const float startAngle[4] = { -halfPi, 0.0f, halfPi, 2.0f * halfPi };

        for (int corner = 0; corner < 4; corner++) {
            for (int i = 0; i < segments; i++) {
                const float angle = startAngle[corner] + ((float)i / (float)segments) * halfPi;
                _stencil_append_vertex(
                    vertices,
                    centerX[corner] + radiusX * cosf(angle),
                    centerY[corner] + radiusY * sinf(angle)
                );
            }
        }
    }

    static void _stencil_build_oval(std::vector<PositionVertex>& vertices) {
        const int segments = 64;
        const float twoPi = 6.28318530718f;

        for (int i = 0; i < segments; i++) {
            const float angle = ((float)i / (float)segments) * twoPi;
            _stencil_append_vertex(vertices, 0.5f * cosf(angle), 0.5f * sinf(angle));
        }
    }

    static bool _stencil_command_build_geometry(const Renderer& renderer, StencilCommand& command) {
        std::vector<PositionVertex> outline;
        outline.reserve(64);

        switch (command.shape) {
            case _STENCIL_SHAPE_ROUNDED_RECTANGLE: {
                // The SDF uses a circular radius in world units, so convert it
                // into the local (per-axis) radii of the ellipse corners.
                float sx = sqrtf(
                    command.matrix.m[0] * command.matrix.m[0] +
                    command.matrix.m[1] * command.matrix.m[1] +
                    command.matrix.m[2] * command.matrix.m[2]
                );
                float sy = sqrtf(
                    command.matrix.m[4] * command.matrix.m[4] +
                    command.matrix.m[5] * command.matrix.m[5] +
                    command.matrix.m[6] * command.matrix.m[6]
                );
                if (sx <= 0.0f) sx = 1.0f;
                if (sy <= 0.0f) sy = 1.0f;

                const float worldRadius = command.radius * (sx < sy ? sx : sy);
                _stencil_build_rounded_rectangle(outline, worldRadius / sx, worldRadius / sy);
                break;
            }

            case _STENCIL_SHAPE_OVAL:
                _stencil_build_oval(outline);
                break;

            case _STENCIL_SHAPE_RECTANGLE:
            default:
                _stencil_build_rectangle(outline);
                break;
        }

        if (outline.size() < 3)
            return false;

        std::vector<PositionVertex> vertices;
        vertices.reserve(outline.size() + 1);
        _stencil_append_vertex(vertices, 0.0f, 0.0f);
        vertices.insert(vertices.end(), outline.begin(), outline.end());

        std::vector<uint32_t> indices;
        indices.reserve(outline.size() * 3);
        for (size_t i = 0; i < outline.size(); i++) {
            indices.push_back(0);
            indices.push_back(static_cast<uint32_t>(i + 1));
            indices.push_back(static_cast<uint32_t>((i + 1) % outline.size() + 1));
        }

        Buffer* vertexBuffer = new Buffer(
            renderer,
            vertices.data(),
            vertices.size() * sizeof(PositionVertex),
            BUFFER_TYPE_VERTEX
        );
        if (vertexBuffer == nullptr || !vertexBuffer->valid()) {
            delete vertexBuffer;
            return false;
        }

        Buffer* indexBuffer = new Buffer(
            renderer,
            indices.data(),
            indices.size() * sizeof(uint32_t),
            BUFFER_TYPE_INDEX
        );
        if (indexBuffer == nullptr || !indexBuffer->valid()) {
            delete indexBuffer;
            delete vertexBuffer;
            return false;
        }

        command.vertexBuffer = vertexBuffer;
        command.indexBuffer = indexBuffer;
        command.indexCount = static_cast<uint32_t>(indices.size());

        return true;
    }

    static bool _stencil_command_submit(const Renderer& renderer, InternalStencil* stencil, StencilCommand command) {
        if (!_stencil_command_build_geometry(renderer, command)) {
            _stencil_command_release_geometry(command);
            error_set("Failed to build stencil geometry %s", error_get());
            return false;
        }

        if (!_stencil_command_add(stencil, command)) {
            _stencil_command_release_geometry(command);
            error_set("Failed to add stencil command");
            return false;
        }

        return true;
    }

    Stencil::Stencil(const Renderer& renderer) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid renderer");

        InternalStencil* intern = new InternalStencil;
        intern->renderer = renderer.internal;
        intern->publicRenderer = &renderer;
        intern->commands = nullptr;
        intern->commandCount = 0;
        intern->commandCapacity = 0;

        internal = intern;
    }

    Stencil::~Stencil() {
        _stencil_destroy(internal);
    }

    Stencil::Stencil(Stencil&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Stencil& Stencil::operator=(Stencil&& other) {
        if (&other == this)
            return *this;

        _stencil_destroy(internal);

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Stencil::valid() const {
        return (internal != nullptr);
    }

    bool Stencil::clear() {
        RYO_ENSURE(valid(), false, "Invalid stencil");

        for (size_t i = 0; i < internal->commandCount; i++)
            _stencil_command_release_geometry(internal->commands[i]);

        internal->commandCount = 0;

        return true;
    }

    bool Stencil::empty() const {
        RYO_ENSURE(valid(), true, "Invalid stencil");

        return internal->commandCount == 0;
    }

    bool Stencil::draw_rectangle(const Matrix4& matrix, const Camera* camera) {
        RYO_ENSURE(valid(), false, "Invalid stencil");

        StencilCommand cmd = {};
        cmd.shape = _STENCIL_SHAPE_RECTANGLE;
        cmd.matrix = matrix;
        cmd.radius = 0.0f;
        if (camera) {
            cmd.camera = *camera;
            cmd.hasCamera = true;
        }

        return _stencil_command_submit(*internal->publicRenderer, internal, cmd);
    }

    bool Stencil::draw_rounded_rectangle(const Matrix4& matrix, float radius, const Camera* camera) {
        RYO_ENSURE(valid(), false, "Invalid stencil");

        StencilCommand cmd = {};
        cmd.shape = _STENCIL_SHAPE_ROUNDED_RECTANGLE;
        cmd.matrix = matrix;
        cmd.radius = radius;
        if (camera) {
            cmd.camera = *camera;
            cmd.hasCamera = true;
        }

        return _stencil_command_submit(*internal->publicRenderer, internal, cmd);
    }

    bool Stencil::draw_oval(const Matrix4& matrix, const Camera* camera) {
        RYO_ENSURE(valid(), false, "Invalid stencil");

        StencilCommand cmd = {};
        cmd.shape = _STENCIL_SHAPE_OVAL;
        cmd.matrix = matrix;
        cmd.radius = 0.0f;
        if (camera) {
            cmd.camera = *camera;
            cmd.hasCamera = true;
        }

        return _stencil_command_submit(*internal->publicRenderer, internal, cmd);
    }
}
