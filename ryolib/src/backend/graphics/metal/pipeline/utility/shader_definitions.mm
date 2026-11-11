#include "../private.h"

namespace ryo {
    NSString* _pipeline_get_shader_name(BuiltinShader shader) {
        switch (shader) {
            case BUILTIN_SHADER_COLOR:
                return @"color";

            case BUILTIN_SHADER_TEXTURE:
                return @"texture";
            
            case BUILTIN_SHADER_STENCIL:
                return @"stencil";
        }
    }
}