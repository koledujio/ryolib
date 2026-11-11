#include "../private.h"

#include <color_vs.h>
#include <color_ps.h>
#include <texture_vs.h>
#include <texture_ps.h>
#include <stencil_vs.h>
#include <stencil_ps.h>

_ShaderSource _pipeline_get_vertex_shader(RYBuiltinShader shader) {
    _ShaderSource source = {};

    switch (shader) {
        case RYBuiltinShaderColor:
            source = {
                (const char*)g_color_vs,
                sizeof(g_color_vs),
                "color_vs.hlsl",
                "vs_5_0"
            };
            break;
        case RYBuiltinShaderTexture:
            source = {
                (const char*)g_texture_vs,
                sizeof(g_texture_vs),
                "texture_vs.hlsl",
                "vs_5_0"
            };
            break;
        case RYBuiltinShaderStencil:
            source = {
                (const char*)g_stencil_vs,
                sizeof(g_stencil_vs),
                "stencil_vs.hlsl",
                "vs_5_0"
            };
            break;
    }

    return source;
}

_ShaderSource _pipeline_get_pixel_shader(RYBuiltinShader shader) {
    _ShaderSource source = {};

    switch (shader) {
        case RYBuiltinShaderColor:
            source = {
                (const char*)g_color_ps,
                sizeof(g_color_ps),
                "color_ps.hlsl",
                "ps_5_0"
            };
            break;
        case RYBuiltinShaderTexture:
            source = {
                (const char*)g_texture_ps,
                sizeof(g_texture_ps),
                "texture_ps.hlsl",
                "ps_5_0"
            };
            break;
        case RYBuiltinShaderStencil:
            source = {
                (const char*)g_stencil_ps,
                sizeof(g_stencil_ps),
                "stencil_ps.hlsl",
                "ps_5_0"
            };
            break;
    }

    return source;
}
