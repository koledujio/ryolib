#include "../private.h"

namespace ryo {
    MTLVertexDescriptor* _pipeline_get_vertex_descriptor(VertexLayoutType layout);
    void _pipeline_set_blend_mode(MTLRenderPipelineDescriptor* pipelineDesc, BlendMode blendMode);
    NSString* _pipeline_get_shader_name(BuiltinShader shader);
}