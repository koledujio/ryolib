#pragma once

#include "../private.h"

MTLVertexDescriptor* _pipeline_get_vertex_descriptor(RYVertexLayoutType layout);
void _pipeline_set_blend_mode(MTLRenderPipelineDescriptor* pipelineDesc, RYBlendMode blendMode);
NSString* _pipeline_get_shader_name(RYBuiltinShader shader);
