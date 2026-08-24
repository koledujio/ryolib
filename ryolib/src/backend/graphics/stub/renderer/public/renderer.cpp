#include "../../private.h"

namespace ryo {
    Renderer::Renderer(const RendererConfig& config) {}
    Renderer::Renderer(const RendererConfig& config, const Renderer& renderer) {}
    Renderer::~Renderer() {}

    Renderer::Renderer(Renderer&&) {}
    Renderer& Renderer::operator=(Renderer&&) {return *this;}

    bool Renderer::valid() const {return true;}

    bool Renderer::resize(unsigned int width, unsigned int height) {return true;}
}