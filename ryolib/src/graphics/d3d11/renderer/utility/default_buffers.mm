#include "../private.h"

bool _create_default_buffers(RYD3D11GraphicsBackend* backend, RYInternalRenderer& internalRenderer) {
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

    void* quadVertexHandle = [backend createBufferWithData:quadVertices
                                                     size:sizeof(quadVertices)
                                                     type:RYBufferTypeVertex
                                                    error:nullptr];
    if (quadVertexHandle) {
        internalRenderer.quadVertexBuffer = [[RYBuffer alloc] initWithInternalBuffer:quadVertexHandle
                                                                           renderer:nil
                                                                               type:RYBufferTypeVertex
                                                                               size:sizeof(quadVertices)];
    }

    void* quadIndexHandle = [backend createBufferWithData:quadIndices
                                                    size:sizeof(quadIndices)
                                                    type:RYBufferTypeIndex
                                                   error:nullptr];
    if (quadIndexHandle) {
        internalRenderer.quadIndexBuffer = [[RYBuffer alloc] initWithInternalBuffer:quadIndexHandle
                                                                          renderer:nil
                                                                              type:RYBufferTypeIndex
                                                                              size:sizeof(quadIndices)];
    }

    static const RYPositionUVVertex quadUVVertices[] = {
        { {-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f} },
        { { 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f} },
        { {-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f} },
        { { 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f} }
    };

    static const uint32_t quadUVIndices[] = {
        0, 1, 2,
        2, 1, 3
    };

    void* quadUVVertexHandle = [backend createBufferWithData:quadUVVertices
                                                       size:sizeof(quadUVVertices)
                                                       type:RYBufferTypeVertex
                                                      error:nullptr];
    if (quadUVVertexHandle) {
        internalRenderer.quadUVVertexBuffer = [[RYBuffer alloc] initWithInternalBuffer:quadUVVertexHandle
                                                                             renderer:nil
                                                                                 type:RYBufferTypeVertex
                                                                                 size:sizeof(quadUVVertices)];
    }

    void* quadUVIndexHandle = [backend createBufferWithData:quadUVIndices
                                                      size:sizeof(quadUVIndices)
                                                      type:RYBufferTypeIndex
                                                     error:nullptr];
    if (quadUVIndexHandle) {
        internalRenderer.quadUVIndexBuffer = [[RYBuffer alloc] initWithInternalBuffer:quadUVIndexHandle
                                                                            renderer:nil
                                                                                type:RYBufferTypeIndex
                                                                                size:sizeof(quadUVIndices)];
    }

    return internalRenderer.quadVertexBuffer != nil &&
           internalRenderer.quadIndexBuffer != nil &&
           internalRenderer.quadUVVertexBuffer != nil &&
           internalRenderer.quadUVIndexBuffer != nil;
}
