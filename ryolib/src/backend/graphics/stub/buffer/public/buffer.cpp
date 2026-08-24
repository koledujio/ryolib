#include "../../private.h"

namespace ryo {
    Buffer::Buffer(const Renderer& renderer, const void* data, size_t size, BufferType type) {}
    Buffer::~Buffer() {}

    Buffer::Buffer(Buffer&&) {}
    Buffer& Buffer::operator=(Buffer&&) {return *this;}

    bool Buffer::valid() const {return true;}
}