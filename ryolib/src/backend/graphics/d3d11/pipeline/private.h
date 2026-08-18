#include "../private.h"

namespace ryo {
     struct _ShaderSource {
        const char* data;
        SIZE_T size;
        const char* debugName;
        const char* profile;
    };

    ID3DBlob* _pipeline_compile_shader(const _ShaderSource shader);
    void _pipeline_release(_InternalPipeline& pipeline);
    bool _pipeline_create_blend_state(ID3D11Device* device, BlendMode blendMode, ID3D11BlendState** result);
    _ShaderSource _pipeline_get_vertex_shader(BuiltinShader shader);
    _ShaderSource _pipeline_get_pixel_shader(BuiltinShader shader);
    bool _pipeline_create_input_layout(const VertexLayoutType layout, const _InternalRenderer& ir, _InternalPipeline& pipeline, ID3DBlob* vertexBlob, HRESULT& hr);
}