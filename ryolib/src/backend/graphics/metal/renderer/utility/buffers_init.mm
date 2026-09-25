#include "../private.h"

namespace ryo {
    bool _renderer_buffers_init(Renderer& renderer, InternalRenderer& internalRenderer) {
        InternalRenderer* intern = &internalRenderer;
        bool fail = false;

        static const PositionVertex quadVertices[] = {
            {-0.5f, -0.5f, 0.0f},
            { 0.5f, -0.5f, 0.0f},
            {-0.5f,  0.5f, 0.0f},
            { 0.5f,  0.5f, 0.0f}
        };

        static const uint32_t quadIndices[] = {
            0, 1, 2,
            2, 1, 3
        };

        intern->quadVertexBuffer = new Buffer(renderer, quadVertices, sizeof(quadVertices), BUFFER_TYPE_VERTEX);
        if (!intern->quadVertexBuffer->valid()) {
            RYO_ASSERT(false, false, "Failed to create quad vertex buffer");
            delete intern->quadVertexBuffer;
            intern->quadVertexBuffer = nullptr;

            error_set("Failed to create vertex quad buffer %s", error_get());
            fail = true;
        }

        intern->quadIndexBuffer = new Buffer(renderer, quadIndices, sizeof(quadIndices), BUFFER_TYPE_INDEX);
        if (!intern->quadIndexBuffer->valid()) {
            RYO_ASSERT(false, false, "Failed to create quad index buffer");
            delete intern->quadIndexBuffer ;
            intern->quadIndexBuffer = nullptr;

            error_set("Failed to create index quad buffer %s", error_get());
            fail = true;
        }

        static const PositionUVVertex quadVerticesUV[] = {
            { {-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f} },
            { { 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f} },
            { {-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f} },
            { { 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f} }
        };

        static const uint32_t quadIndicesUV[] = {
            0, 1, 2,
            2, 1, 3
        };

        intern->quadVertexBufferUV = new Buffer(renderer, quadVerticesUV, sizeof(quadVerticesUV), BUFFER_TYPE_VERTEX);
        if (!intern->quadVertexBufferUV->valid()) {
            RYO_ASSERT(false, false, "Failed to create uv quad vertex buffer");
            delete intern->quadVertexBufferUV;
            intern->quadVertexBufferUV = nullptr;

            error_set("Failed to create uv vertex quad buffer %s", error_get());
            fail = true;
        }

        intern->quadIndexBufferUV = new Buffer(renderer, quadIndicesUV, sizeof(quadIndicesUV), BUFFER_TYPE_INDEX);
        if (!intern->quadIndexBufferUV->valid()) {
            RYO_ASSERT(false, false, "Failed to create uv quad index buffer");
            delete intern->quadIndexBufferUV;
            intern->quadIndexBufferUV = nullptr;

            error_set("Failed to create uv index quad buffer %s", error_get());
            fail = true;
        }

        return !fail;
    }
}