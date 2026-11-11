#include "../private.h"

namespace ryo {
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

        if (stencil->commands)
            free(stencil->commands);

        delete stencil;
    }

    Stencil::Stencil(const Renderer& renderer) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid renderer");

        InternalStencil* intern = new InternalStencil;
        intern->renderer = renderer.internal;
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
        if (this == &other)
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

        return _stencil_command_add(internal, cmd);
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

        return _stencil_command_add(internal, cmd);
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

        return _stencil_command_add(internal, cmd);
    }
}