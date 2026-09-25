#include "../../private.h"

#include <cstring>

namespace ryo {
    Texture::Texture(const Renderer& renderer, const void* pixels, unsigned int width, unsigned int height) {
        const uint32_t alignedWidth = (width + 7) & ~7;
        const uint32_t stride = alignedWidth * 4;
        const uint32_t size = stride * height;

        void* mem = nullptr;
        SceUID memId = -1;

        if (!_memory_alloc_gpu_rw(
            "texture",
            size,
            _MEMORY_TYPE_UNCACHE,
            mem,
            memId
        )) {
            error_set("Failed to allocate gpu memory: %s", error_get());
            return;
        }

        const uint8_t* src = static_cast<const uint8_t*>(pixels);
        uint8_t* dst = static_cast<uint8_t*>(mem);
        for (unsigned int y = 0; y < height; ++y) {
            memcpy(dst + y * stride, src + y * width * 4, width * 4);
        }

        SceGxmTexture texture;

        int result = sceGxmTextureInitLinearStrided(
            &texture,
            mem,
            SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
            width,
            height,
            stride
        );

        if (result < 0) {
            _memory_free_gpu(mem, memId);
            error_set("sceGxmTextureInitLinearStrided failed 0x%08X", result);
            return;
        }

        InternalTexture* intern = new InternalTexture;
        intern->renderer = &renderer;
        intern->texture = texture;
        intern->mem = mem;
        intern->memId = memId;
        intern->width = width;
        intern->height = height;

        internal = intern;
    }

    Texture::Texture(InternalTexture* internal) {
        this->internal = internal;
    }

    Texture::~Texture() {
        if (!internal)
            return;

        if (internal->mem != nullptr && internal->memId >= 0) {
            if (internal->renderer && internal->renderer->valid() && internal->renderer->internal && internal->renderer->internal->context) {
                sceGxmFinish(internal->renderer->internal->context);
            }
            _memory_free_gpu(internal->mem, internal->memId);
        }

        delete internal;
    }

    Texture::Texture(Texture&& other) : internal(other.internal) {
        other.internal = nullptr;
    }

    Texture& Texture::operator=(Texture&& other) {
        if (this == &other)
            return *this;

        if (internal) {
            if (internal->mem != nullptr && internal->memId >= 0) {
                if (internal->renderer && internal->renderer->valid() && internal->renderer->internal && internal->renderer->internal->context) {
                    sceGxmFinish(internal->renderer->internal->context);
                }
                _memory_free_gpu(internal->mem, internal->memId);
            }

            delete internal;
        }

        internal = other.internal;
        other.internal = nullptr;

        return *this;
    }

    bool Texture::valid() const {
        return internal != nullptr &&
               internal->mem != nullptr;
    }
}