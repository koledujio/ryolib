#include "../private.h"

namespace ryo {
    bool Renderer::draw_rect_texture(const Texture& texture, SamplerType samplerType, const Color& color, const Matrix4& matrix, const DrawSettings* settings) const {
        RYO_ENSURE(valid(), false, "Invalid renderer");
        RYO_ENSURE(texture.valid(), false, "Invalid texture");

        Matrix4 mvp = matrix;
        if (settings && settings->camera) {
            int width = 0, height = 0;
            _draw_get_target_size(*internal, width, height);
            mvp = _draw_compute_camera_vp(*settings->camera, (float)width / (float)height) * matrix;
        }
        Matrix4 mvpTransposed = mvp.transpose();

        float col[4] = {color.r, color.g, color.b, color.a};

        const Pipeline& pipeline = settings && settings->pipelineOverride
            ? *settings->pipelineOverride
            : *internal->defaultTexturePipeline;

        const Texture* textures[] = { &texture };
        SamplerType samplers[] = { samplerType };

        DrawCall call{
            pipeline,
            *internal->quadVertexBufferUV,
            sizeof(PositionUVVertex),
            *internal->quadIndexBufferUV,
            6,
            textures,
            samplers,
            1,
            mvpTransposed.m,
            sizeof(Matrix4),
            col,
            sizeof(col),
            settings ? settings->cullMode : CULL_MODE_NONE,
            settings ? settings->windingOrder : WINDING_ORDER_COUNTER_CLOCKWISE,
            settings ? settings->fillMode : FILL_MODE_SOLID,
            settings ? settings->depthMode : DEPTH_MODE_READ_WRITE,
            settings ? settings->stencil : nullptr
        };

        return draw_ex(call);
    }
}