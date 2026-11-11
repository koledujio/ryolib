#include "../private.h"

bool _renderer_buffers_init(RYMetalGraphicsBackend *backend, RYInternalRenderer &internalRenderer) {
    RYInternalRenderer *intern = &internalRenderer;
    bool fail = false;

    static const RYPositionVertex quadVertices[] = {
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        {-0.5f,  0.5f, 0.0f},
        { 0.5f,  0.5f, 0.0f}
    };

    static const uint32_t quadIndices[] = {
        0, 1, 2,
        2, 1, 3
    };

    void *quadVertexHandle = [backend createBufferWithData:quadVertices
                                                      size:sizeof(quadVertices)
                                                      type:RYBufferTypeVertex
                                                     error:NULL];
    if (!quadVertexHandle) {
        fail = true;
    } else {
        intern->quadVertexBuffer = [[RYBuffer alloc] initWithInternalBuffer:quadVertexHandle
                                                                   renderer:nil
                                                                       type:RYBufferTypeVertex
                                                                       size:sizeof(quadVertices)];
    }

    void *quadIndexHandle = [backend createBufferWithData:quadIndices
                                                     size:sizeof(quadIndices)
                                                     type:RYBufferTypeIndex
                                                    error:NULL];
    if (!quadIndexHandle) {
        fail = true;
    } else {
        intern->quadIndexBuffer = [[RYBuffer alloc] initWithInternalBuffer:quadIndexHandle
                                                                  renderer:nil
                                                                      type:RYBufferTypeIndex
                                                                      size:sizeof(quadIndices)];
    }

    static const RYPositionUVVertex quadVerticesUV[] = {
        { {-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f} },
        { { 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f} },
        { {-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f} },
        { { 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f} }
    };

    static const uint32_t quadIndicesUV[] = {
        0, 1, 2,
        2, 1, 3
    };

    void *quadVertexUVHandle = [backend createBufferWithData:quadVerticesUV
                                                       size:sizeof(quadVerticesUV)
                                                       type:RYBufferTypeVertex
                                                      error:NULL];
    if (!quadVertexUVHandle) {
        fail = true;
    } else {
        intern->quadVertexBufferUV = [[RYBuffer alloc] initWithInternalBuffer:quadVertexUVHandle
                                                                     renderer:nil
                                                                         type:RYBufferTypeVertex
                                                                         size:sizeof(quadVerticesUV)];
    }

    void *quadIndexUVHandle = [backend createBufferWithData:quadIndicesUV
                                                      size:sizeof(quadIndicesUV)
                                                      type:RYBufferTypeIndex
                                                     error:NULL];
    if (!quadIndexUVHandle) {
        fail = true;
    } else {
        intern->quadIndexBufferUV = [[RYBuffer alloc] initWithInternalBuffer:quadIndexUVHandle
                                                                    renderer:nil
                                                                        type:RYBufferTypeIndex
                                                                        size:sizeof(quadIndicesUV)];
    }

    return !fail;
}
