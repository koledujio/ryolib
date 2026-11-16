#include "../../private.h"

namespace ryo {
    Texture::Texture(const Renderer& renderer, const void* pixels, unsigned int width, unsigned int height) {}
    Texture::Texture(InternalTexture* internal) {}
    Texture::~Texture() {}

    Texture::Texture(Texture&&) {}
    Texture& Texture::operator=(Texture&&) {return *this;}

    bool Texture::valid() const {return true;}
}