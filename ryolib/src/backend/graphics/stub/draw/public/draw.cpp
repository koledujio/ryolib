#include "../../private.h"

namespace ryo {
    void Renderer::draw_pass_begin(RenderTarget* renderTarget, const Color* clearColor) {}
    void Renderer::draw_pass_end() {}

    bool Renderer::draw_rect_color(const Color& color, const Matrix4& matrix, const DrawSettings* settings) const {return true;}
    bool Renderer::draw_rect_texture(const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings) const {return true;}
    bool Renderer::draw_mesh_texture(const Buffer& vertexBuffer, const Buffer& indexBuffer, uint32_t indexCount, const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings) const {return true;}
}