#include "../private.h"

namespace ryo {
    MTLVertexDescriptor* _pipeline_get_vertex_descriptor(VertexLayoutType layout) {
        switch (layout) {
            case VERTEX_LAYOUT_POSITION: {
                MTLVertexDescriptor* desc = [MTLVertexDescriptor vertexDescriptor];
                desc.attributes[0].format = MTLVertexFormatFloat3;
                desc.attributes[0].offset = offsetof(PositionVertex, position);
                desc.attributes[0].bufferIndex = 0;
                desc.layouts[0].stride = sizeof(PositionVertex);
                return desc;
            }
            case VERTEX_LAYOUT_POSITION_UV: {
                MTLVertexDescriptor* desc = [MTLVertexDescriptor vertexDescriptor];
                desc.attributes[0].format = MTLVertexFormatFloat3;
                desc.attributes[0].offset = offsetof(PositionUVVertex, position);
                desc.attributes[0].bufferIndex = 0;
                desc.attributes[1].format = MTLVertexFormatFloat2;
                desc.attributes[1].offset = offsetof(PositionUVVertex, uv);
                desc.attributes[1].bufferIndex = 0;
                desc.layouts[0].stride = sizeof(PositionUVVertex);
                return desc;
            }
        }
    }
}