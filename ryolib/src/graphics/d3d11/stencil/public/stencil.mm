#include "../private.h"

#include <cstdlib>

static bool _stencil_command_add(RYInternalStencil *stencil, RYStencilCommand command) {
    if (stencil->commandCount >= stencil->commandCapacity) {
        const size_t newCapacity = stencil->commandCapacity == 0 ? 8 : stencil->commandCapacity + 8;

        RYStencilCommand *newCommands = static_cast<RYStencilCommand *>(realloc(stencil->commands, sizeof(RYStencilCommand) * newCapacity));
        if (!newCommands)
            return false;

        stencil->commands = newCommands;
        stencil->commandCapacity = newCapacity;
    }

    stencil->commands[stencil->commandCount++] = command;
    return true;
}

@implementation RYD3D11GraphicsBackend (Stencil)

- (void *)createStencilWithError:(RYError **)error {
    if (error) {
        *error = nil;
    }

    if (!_internal) {
        if (error) {
            *error = [[RYError alloc] initWithCode:RYErrorCodeGraphicsInitializationFailed
                                           message:"D3D11 backend is not initialized"];
        }
        return NULL;
    }

    RYInternalStencil *stencil = new RYInternalStencil;
    stencil->renderer = (RYInternalRenderer *)_internal;
    stencil->renderTarget = nullptr;
    stencil->commands = nullptr;
    stencil->commandCount = 0;
    stencil->commandCapacity = 0;

    return stencil;
}

- (void)destroyStencil:(void *)stencilHandle {
    if (!stencilHandle) return;

    RYInternalStencil *stencil = (RYInternalStencil *)stencilHandle;
    if (stencil->commands) {
        free(stencil->commands);
    }
    delete stencil;
}

- (BOOL)stencilIsEmpty:(void *)stencilHandle {
    if (!stencilHandle) return YES;
    RYInternalStencil *stencil = (RYInternalStencil *)stencilHandle;
    return stencil->commandCount == 0;
}

- (BOOL)clearStencil:(void *)stencilHandle {
    if (!stencilHandle) return NO;
    RYInternalStencil *stencil = (RYInternalStencil *)stencilHandle;
    stencil->commandCount = 0;
    return YES;
}

- (BOOL)addRectangleToStencil:(void *)stencilHandle
                       matrix:(ryo::Matrix4)matrix
                       camera:(const RYCamera *)camera {
    if (!stencilHandle) return NO;

    RYStencilCommand cmd = {};
    cmd.shape = _RY_STENCIL_SHAPE_RECTANGLE;
    cmd.matrix = matrix;
    cmd.radius = 0.0f;
    if (camera) {
        cmd.camera = *camera;
        cmd.hasCamera = true;
    }

    return _stencil_command_add((RYInternalStencil *)stencilHandle, cmd);
}

- (BOOL)addRoundedRectangleToStencil:(void *)stencilHandle
                              matrix:(ryo::Matrix4)matrix
                              radius:(float)radius
                              camera:(const RYCamera *)camera {
    if (!stencilHandle) return NO;

    RYStencilCommand cmd = {};
    cmd.shape = _RY_STENCIL_SHAPE_ROUNDED_RECTANGLE;
    cmd.matrix = matrix;
    cmd.radius = radius;
    if (camera) {
        cmd.camera = *camera;
        cmd.hasCamera = true;
    }

    return _stencil_command_add((RYInternalStencil *)stencilHandle, cmd);
}

- (BOOL)addOvalToStencil:(void *)stencilHandle
                  matrix:(ryo::Matrix4)matrix
                  camera:(const RYCamera *)camera {
    if (!stencilHandle) return NO;

    RYStencilCommand cmd = {};
    cmd.shape = _RY_STENCIL_SHAPE_OVAL;
    cmd.matrix = matrix;
    cmd.radius = 0.0f;
    if (camera) {
        cmd.camera = *camera;
        cmd.hasCamera = true;
    }

    return _stencil_command_add((RYInternalStencil *)stencilHandle, cmd);
}

@end
