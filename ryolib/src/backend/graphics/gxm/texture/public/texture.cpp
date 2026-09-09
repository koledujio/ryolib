#include "../../private.h"

#include <cstring>

namespace ryo {
    Texture::Texture(const Renderer& renderer, const void* pixels, unsigned int width, unsigned int height) {
        const uint32_t size = width * height * 4;

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

        memcpy(mem, pixels, size);

        SceGxmTexture texture;

        int result = sceGxmTextureInitLinear(
            &texture,
            mem,
            SCE_GXM_TEXTURE_FORMAT_A8B8G8R8,
            width,
            height,
            0 // mip count
        );

        if (result < 0) {
            _memory_free_gpu(mem, memId);
            error_set("sceGxmTextureInitLinear failed 0x%08X", result);
            return;
        }

        InternalTexture* intern = new InternalTexture;
        intern->renderer = &renderer;
        intern->texture = texture;
        intern->mem = mem;
        intern->memId = memId;
        intern->width = width;
        intern->height = height;
    }

    Texture::Texture(InternalTexture* internal) {
        this->internal = internal;
    }

    Texture::~Texture() {
        if (!internal)
            return;

        if (internal->mem)
            _memory_free_gpu(internal->mem, internal->memId);

        delete internal;
    }

    Texture::Texture(Texture&&) {}
    Texture& Texture::operator=(Texture&&) {return *this;}

    bool Texture::valid() const {return true;}
}