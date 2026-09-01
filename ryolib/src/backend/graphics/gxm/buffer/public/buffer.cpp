#include "../../private.h"

#include <cstring>

namespace ryo {
    Buffer::Buffer(const Renderer& renderer, const void* data, size_t size, BufferType type) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid renderer");
        RYO_ENSURE_VOID(data != nullptr, "Null buffer data");
        RYO_ENSURE_VOID(size > 0, "Invalid data size");

        size_t alignedSize = _memory_align_main(size);

        void* mem = nullptr;
        SceUID memId = -1;
        if (!_memory_alloc_gpu_r(
            type == BUFFER_TYPE_VERTEX ? "buffer_vertex" : "buffer_index",
            alignedSize,
            _MEMORY_TYPE_UNCACHE,
            mem,
            memId
        )) {
            error_set("Failed to allocate memory for buffer");
            return;
        }

        memcpy(mem, data, size);

        InternalBuffer* intern = new InternalBuffer;
        intern->mem = mem;
        intern->memId = memId;
        intern->size = alignedSize;
        intern->type = type;

        internal = intern;
    }

    Buffer::~Buffer() {
        if (!internal)
            return;

        if (internal->mem != nullptr && internal->memId >= 0)
            _memory_free_gpu(internal->mem, internal->memId);

        delete internal;
    }

    Buffer::Buffer(Buffer&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Buffer& Buffer::operator=(Buffer&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            if (internal->mem != nullptr && internal->memId >= 0)
                _memory_free_gpu(internal->mem, internal->memId);

            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;
        
        return *this;
    }

    bool Buffer::valid() const {
        return (internal != nullptr &&
                internal->mem != nullptr &&
                internal->memId >= 0 &&
                internal->size > 0);
    }
}