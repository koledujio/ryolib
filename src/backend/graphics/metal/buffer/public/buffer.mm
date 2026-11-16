#include "../../private.h"

namespace ryo {
    Buffer::Buffer(const Renderer& renderer, const void *data, size_t size, BufferType type) {
        RYO_ENSURE_VOID(renderer.valid(), "Invalid renderer");
        RYO_ENSURE_VOID(!renderer.internal->inFrame, "Renderer is in a frame");

        id<MTLDevice> device = renderer.internal->device;
        id<MTLBuffer> mtlBuffer = [device newBufferWithBytes:data length:size options:MTLResourceStorageModeShared];

        InternalBuffer* intern = new InternalBuffer;
        intern->buffer = mtlBuffer;

        internal = intern;
    }

    Buffer::~Buffer() {
        if (!internal)
            return;

        if (internal->buffer)
            [internal->buffer release];

        delete internal;
    }

    Buffer::Buffer(Buffer&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Buffer& Buffer::operator=(Buffer&& other) {
        if (this == &other)
            return *this;
        
        if (internal) {
            if (internal->buffer != nil)
                [internal->buffer release];

            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Buffer::valid() const {
        return internal != nullptr && internal->buffer != nil;
    }
}
