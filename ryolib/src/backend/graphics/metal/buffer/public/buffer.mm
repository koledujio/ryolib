#include "../../private.h"

namespace ryo {
    ResourcePool<id<MTLBuffer>, MAX_BUFFERS> _buffer_pool;

    Buffer buffer_create(Renderer renderer, const void *data, size_t size, BufferType type) {
        if (_buffer_pool.active_count >= MAX_BUFFERS) {
            error_set("Maximum number of buffers reached");
            return {};
        }

        RYO_ENSURE(renderer_is_valid(renderer), {}, "Invalid renderer handle");

        id<MTLDevice> device = _renderer_pool.items[renderer.index()].device;

        INDEX idx = _buffer_pool.allocate_index();
        GENID gen = _buffer_pool.generations[idx];

        id<MTLBuffer> mtlBuffer = [device newBufferWithBytes:data length:size options:MTLResourceStorageModeShared];

        _buffer_pool.items[idx] = mtlBuffer;

        return Buffer::pack(idx, gen);
    }

    void buffer_destroy(Buffer buffer) {
        RYO_ENSURE_VOID(buffer_is_valid(buffer), "Invalid buffer handle");

        INDEX idx = buffer.index();

        if (_buffer_pool.items[idx]) {
            [_buffer_pool.items[idx] release];
            _buffer_pool.items[idx] = nil;
        }

        _buffer_pool.free_index(idx);
    }

    bool buffer_is_valid(Buffer buffer) {
        INDEX idx = buffer.index();

        if (idx >= MAX_BUFFERS)
            return false;
        
        if (!_buffer_pool.is_valid(idx, buffer.gen()))
            return false;

        return _buffer_pool.items[idx] != nil;
    }
}
