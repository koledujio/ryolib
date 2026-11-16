#pragma once

#include "../../demo.h"
#include <cmath>

class TextFallbackFontDemo : public Demo {
public:
    const char* title_get() const override {
        return "Text Fallback Fonts";
    }

    bool init(DemoContext& ctx) override {
        ryo::PipelineDesc pipelineDesc = {};
        pipelineDesc.shader = ryo::BUILTIN_SHADER_TEXTURE;
        pipelineDesc.blendMode = ryo::BLEND_MODE_ALPHA;
        pipelineDesc.vertexLayout = ryo::VERTEX_LAYOUT_POSITION_UV;

        m_pipeline = new ryo::Pipeline(ctx.renderer, pipelineDesc);
        if (!m_pipeline->valid()) {
            printf("Failed to create pipeline: %s\n", ryo::error_get());
            return false;
        }

        m_camera.projectionType = ryo::CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC;
        m_camera.orthographicSize = 720.0f;
        m_camera.nearPlane = -1.0f;
        m_camera.farPlane = 1.0f;
        m_camera.position = {0.0f, 0.0f, 1.0f};
        m_camera.target = {0.0f, 0.0f, 0.0f};
        m_camera.up = {0.0f, 1.0f, 0.0f};

        m_settings.cullMode = ryo::CULL_MODE_BACK;
        m_settings.windingOrder = ryo::WINDING_ORDER_COUNTER_CLOCKWISE;
        m_settings.fillMode = ryo::FILL_MODE_SOLID;
        m_settings.depthMode = ryo::DEPTH_MODE_DISABLED;
        m_settings.camera = &m_camera;
        m_settings.pipelineOverride = m_pipeline;
        m_settings.stencil = nullptr;

        ryo::FontConfig fontConfig = {};
        fontConfig.path = "assets/lato-400-normal.ttf";
        fontConfig.fontSize = 67.0f;

        m_font = new ryo::Font(fontConfig);
        if (!m_font->valid()) {
            printf("Failed to load font: %s\n", ryo::error_get());
            quit(ctx);
            return 0;
        }

        ryo::FontConfig fallbackFontConfig = {};
        fallbackFontConfig.path = "assets/notosansjp-regular.ttf";
        fallbackFontConfig.fontSize = 60.0f;

        m_fallbackFont = new ryo::Font(fallbackFontConfig);
        if (!m_fallbackFont->valid()) {
            printf("Failed to load font: %s\n", ryo::error_get());
            quit(ctx);
            return 0;
        }

        if (!m_font->fallback_add(*m_fallbackFont)) {
            printf("Failed to add fallback font: %s\n", ryo::error_get());
            quit(ctx);
            return 0;
        }

        const char* text = "Hello! こんにちは！";
        ryo::Color textColor = {1.0f, 1.0f, 1.0f, 1.0f};

        void* pixels = nullptr;
        if (!m_font->pixels_render_line(text, textColor, m_width, m_height, pixels)) {
            printf("Failed to render text line: %s\n", ryo::error_get());
            quit(ctx);
            return false;
        }

        m_texture = new ryo::Texture(ctx.renderer, pixels, m_width, m_height);
        m_font->pixels_free(pixels);
        if (m_texture == nullptr || !m_texture->valid()) {
            printf("Failed to create text texture: %s\n", ryo::error_get());
            quit(ctx);
            return false;
        }

        return true;
    }

    void quit(DemoContext& ctx) override {
        if (m_texture != nullptr) {
            delete m_texture;
            m_texture = nullptr;
        }

        if (m_pipeline != nullptr) {
            delete m_pipeline;
            m_pipeline = nullptr;
        }

        if (m_font != nullptr) {
            delete m_font;
            m_font = nullptr;
        }

        if (m_fallbackFont != nullptr) {
            delete m_fallbackFont;
            m_fallbackFont = nullptr;
        }
    }

    void update(DemoContext& ctx, float dt) override {
        m_time += dt;
    }

    void render(DemoContext& ctx) override {
        ryo::Color clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        ryo::Color imageColor = {1.0f, 1.0f, 1.0f, 1.0f};

        ryo::Matrix4 matrix = ryo::Matrix4::scaling((float)m_width + (float)m_width * 0.1f * sin(m_time), (float)m_height + (float)m_height * 0.1f * sin(m_time), 1.0f);

        ctx.renderer.draw_pass_begin(nullptr, &clearColor);

        ctx.renderer.draw_rect_texture(*m_texture, ryo::SAMPLER_TYPE_LINEAR, imageColor, matrix, &m_settings);

        ctx.renderer.draw_pass_end();
    }

private:
    float m_time = 0.0f;
    ryo::Pipeline* m_pipeline;
    ryo::Camera m_camera;
    ryo::DrawSettings m_settings;
    ryo::Font* m_font;
    ryo::Font* m_fallbackFont;
    int m_width, m_height;
    ryo::Texture* m_texture;
};
