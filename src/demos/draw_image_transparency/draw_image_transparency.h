#pragma once

#include "../../demo.h"
#include <cmath>

class DrawImageTransparencyDemo : public Demo {
public:
    const char* title_get() const override {
        return "Draw Image Transparency";
    }

    bool init(DemoContext& ctx) override {
        ryo::PipelineDesc pipelineDesc = {};
        pipelineDesc.shader = ryo::BUILTIN_SHADER_TEXTURE;
        pipelineDesc.blendMode = ryo::BLEND_MODE_ALPHA;
        pipelineDesc.vertexLayout = ryo::VERTEX_LAYOUT_POSITION_UV;

        m_pipeline = new ryo::Pipeline(ctx.renderer, pipelineDesc);
        if (!m_pipeline->valid()) {
            printf("Failed to create pipeline: %s\n", ryo::error_get());
            quit(ctx);
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

        m_texture = ryo::image_load_texture(ctx.renderer, "assets/kasane-teto.png", m_width, m_height);
        if (m_texture == nullptr || !m_texture->valid()) {
            printf("Failed to load texture: %s\n", ryo::error_get());
            quit(ctx);
            return false;
        }

        m_matrix = ryo::Matrix4::scaling((float)m_width / (float)m_height * 720.0f, 720.0f, 1.0f);

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
    }

    void update(DemoContext& ctx, float dt) override {
        m_time += dt;
    }

    void render(DemoContext& ctx) override {
        ryo::Color clearColor = {sin(m_time), cos(m_time), sin(m_time), 1.0f};
        ryo::Color imageColor = {1.0f, 1.0f, 1.0f, 1.0f};

        ctx.renderer.draw_pass_begin(nullptr, &clearColor);

        ctx.renderer.draw_rect_texture(*m_texture, ryo::SAMPLER_TYPE_LINEAR, imageColor, m_matrix, &m_settings);

        ctx.renderer.draw_pass_end();
    }

private:
    float m_time = 0.0f;
    ryo::Pipeline* m_pipeline;
    ryo::Camera m_camera;
    ryo::DrawSettings m_settings;
    ryo::Matrix4 m_matrix;
    int m_width, m_height;
    ryo::Texture* m_texture;
};
