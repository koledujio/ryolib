#include "../private.h"

namespace ryo {
    void _create_default_buffers(const Renderer& renderer) {
        PositionVertex quadVertices[4] = {
            { { -0.5f, -0.5f, 0.0f } },
            { {  0.5f, -0.5f, 0.0f } },
            { { -0.5f,  0.5f, 0.0f } },
            { {  0.5f,  0.5f, 0.0f } }
        };
        uint32_t quadIndices[6] = { 0, 1, 2, 2, 1, 3 };

        renderer.internal->quadVertexBuffer = new Buffer(renderer, quadVertices, sizeof(quadVertices), BUFFER_TYPE_VERTEX);
        renderer.internal->quadIndexBuffer = new Buffer(renderer, quadIndices, sizeof(quadIndices), BUFFER_TYPE_INDEX);

        PositionUVVertex quadUVVertices[4] = {
            { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } },
            { {  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f } },
            { { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f } },
            { {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f } }
        };
        uint32_t quadUVIndices[6] = { 0, 1, 2, 2, 1, 3 };

        renderer.internal->quadUVVertexBuffer = new Buffer(renderer, quadUVVertices, sizeof(quadUVVertices), BUFFER_TYPE_VERTEX);
        renderer.internal->quadUVIndexBuffer = new Buffer(renderer, quadUVIndices, sizeof(quadUVIndices), BUFFER_TYPE_INDEX);
    }
}