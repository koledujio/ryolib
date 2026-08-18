#include "../private.h"

#include <color_vs.h>
#include <color_ps.h>
#include <texture_vs.h>
#include <texture_ps.h>

namespace ryo {
    _ShaderSource _pipeline_get_vertex_shader(BuiltinShader shader) {
        _ShaderSource source = {};

//         if (strcmp(name, "color") == 0) {
//             source = {
//                 g_color_vs,
// #if GFX_HLSL_SOURCE_FALLBACK
//                 sizeof(g_color_vs) - 1,
// #else
//                 sizeof(g_color_vs),
// #endif
//                 "color_vs.hlsl",
//                 "vs_5_0"
//             };
//         } else if (strcmp(name, "texture") == 0) {
//             source = {
//                 g_texture_vs,
// #if GFX_HLSL_SOURCE_FALLBACK
//                 sizeof(g_texture_vs) - 1,
// #else
//                 sizeof(g_texture_vs),
// #endif
//                 "texture_vs.hlsl",
//                 "vs_5_0"
//             };
//         } else {
//             log(LOG_TYPE_ERROR, "Pipeline", "D3D11", "No D3D11 vertex shader defined for passed PipelineDesc.name!");
//             source.data = nullptr;
//         }

        switch (shader) { // non-fallback shaders not tested yet
            case BUILTIN_SHADER_COLOR:
                source = {
                    g_color_vs,
                    sizeof(g_color_vs),
                    "color_vs.hlsl",
                    "vs_5_0"
                };
                break;
            case BUILTIN_SHADER_TEXTURE:
                source = {
                    g_texture_vs,
                    sizeof(g_texture_vs),
                    "textyre_vs.hlsl",
                    "vs_5_0"
                };
                break;
        }

        return source;
    }

    _ShaderSource _pipeline_get_pixel_shader(BuiltinShader shader) {
        _ShaderSource source = {};

//         if (strcmp(name, "color") == 0) {
//             source = {
//                 g_color_ps,
// #if GFX_HLSL_SOURCE_FALLBACK
//                 sizeof(g_color_ps) - 1,
// #else
//                 sizeof(g_color_ps),
// #endif
//                 "color_ps.hlsl",
//                 "ps_5_0"
//             };
//         } else if (strcmp(name, "texture") == 0) {
//             source = {
//                 g_texture_ps,
// #if GFX_HLSL_SOURCE_FALLBACK
//                 sizeof(g_texture_ps) - 1,
// #else
//                 sizeof(g_texture_ps),
// #endif
//                 "texture_ps.hlsl",
//                 "ps_5_0"
//             };
//         } else {
//             log(LOG_TYPE_ERROR, "Pipeline", "D3D11", "No D3D11 pixel shader defined for passed PipelineDesc.name!");
//             source.data = nullptr;
//         }

        switch (shader) {
            case BUILTIN_SHADER_COLOR:
                source = {
                    g_color_ps,
                    sizeof(g_color_ps),
                    "color_ps.hlsl",
                    "ps_5_0"
                };
                break;
            case BUILTIN_SHADER_TEXTURE:
                source = {
                    g_texture_ps,
                    sizeof(g_texture_ps),
                    "texture_ps.hlsl",
                    "ps_5_0"
                };
                break;
        }

        return source;
    }
}