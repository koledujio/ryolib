#include "../private.h"

namespace ryo {
    const char* _get_vertex_shader_filename(BuiltinShader shader) {
        switch (shader) {
            case BUILTIN_SHADER_COLOR:
                return "color_v.gxp";

            case BUILTIN_SHADER_TEXTURE:
                return "texture_v.gxp";

            default:
                return nullptr;
        }
    }

    const char* _get_fragment_shader_filename(BuiltinShader shader) {
        switch (shader) {
            case BUILTIN_SHADER_COLOR:
                return "color_f.gxp";

            case BUILTIN_SHADER_TEXTURE:
                return "texture_f.gxp";

            default:
                return nullptr;
        }
    }
}