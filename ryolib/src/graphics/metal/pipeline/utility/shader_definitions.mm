#include "../private.h"

NSString* _pipeline_get_shader_name(RYBuiltinShader shader) {
    switch (shader) {
        case RYBuiltinShaderColor:
            return @"color";
        case RYBuiltinShaderTexture:
            return @"texture";
        case RYBuiltinShaderStencil:
            return @"stencil";
        default:
            return nil;
    }
}
