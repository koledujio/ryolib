#include "../private.h"

MTLVertexDescriptor* _pipeline_get_vertex_descriptor(RYVertexLayoutType layout) {
    switch (layout) {
        case RYVertexLayoutTypePosition: {
            MTLVertexDescriptor* desc = [MTLVertexDescriptor vertexDescriptor];
            desc.attributes[0].format = MTLVertexFormatFloat3;
            desc.attributes[0].offset = offsetof(RYPositionVertex, position);
            desc.attributes[0].bufferIndex = 0;
            desc.layouts[0].stride = sizeof(RYPositionVertex);
            return desc;
        }
        case RYVertexLayoutTypePositionUV: {
            MTLVertexDescriptor* desc = [MTLVertexDescriptor vertexDescriptor];
            desc.attributes[0].format = MTLVertexFormatFloat3;
            desc.attributes[0].offset = offsetof(RYPositionUVVertex, position);
            desc.attributes[0].bufferIndex = 0;
            desc.attributes[1].format = MTLVertexFormatFloat2;
            desc.attributes[1].offset = offsetof(RYPositionUVVertex, uv);
            desc.attributes[1].bufferIndex = 0;
            desc.layouts[0].stride = sizeof(RYPositionUVVertex);
            return desc;
        }
        default:
            return nil;
    }
}
