#include "../../private.h"

namespace ryo {
    Renderer::Renderer(const RendererConfig& config) {}
    Renderer::Renderer(const RendererConfig& config, const Renderer& renderer) {}
    Renderer::~Renderer() {}

    Renderer::Renderer(Renderer&&) {}
    Renderer& Renderer::operator=(Renderer&&) {return *this;}

    bool Renderer::valid() const {return true;}

    bool Renderer::resize(unsigned int width, unsigned int height) {return true;}
    bool Renderer::size_get(unsigned int& outWidth, unsigned int& outHeight) const {outWidth = 0; outHeight = 0; return false;}

    const Pipeline& Renderer::pipeline_builtin_get(BuiltinPipeline builtinPipeline) const {
        static const Pipeline invalidPipeline;

        return invalidPipeline;
    }
}