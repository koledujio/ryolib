#pragma once

#include "../private.h"

struct _ShaderSource {
    const char* data;
    SIZE_T size;
    const char* debugName;
    const char* profile;
};

ID3DBlob* _pipeline_compile_shader(const _ShaderSource shader);
void _pipeline_release(RYInternalPipeline* pipeline);
bool _pipeline_create_blend_state(ID3D11Device* device, RYBlendMode blendMode, ID3D11BlendState** result);
_ShaderSource _pipeline_get_vertex_shader(RYBuiltinShader shader);
_ShaderSource _pipeline_get_pixel_shader(RYBuiltinShader shader);
bool _pipeline_create_input_layout(const RYVertexLayoutType layout, const RYInternalRenderer& ir, RYInternalPipeline& pipeline, ID3DBlob* vertexBlob, HRESULT& hr);
