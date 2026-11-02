#pragma once

#include "../../demo.h"
#include <cmath>

class StencilRoundedRectangleDemo : public Demo {
public:
    const char* title_get() const override {
        return "Stencil Rounded Rectangle";
    }

    bool init(DemoContext& ctx) override {
        m_camera.projectionType = ryo::CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC;
        m_camera.orthographicSize = 720.0f;
        m_camera.nearPlane = -1.0f;
        m_camera.farPlane = 1.0f;
        m_camera.position = {0.0f, 0.0f, 1.0f};
        m_camera.target = {0.0f, 0.0f, 0.0f};
        m_camera.up = {0.0f, 1.0f, 0.0f};

        RYError *error = nil;
        RYString *path = [[RYString alloc] initWithUTF8String:"assets/hatsune-miku.png"];
        RYImage *image = [RYImage newImageWithPath:path error:&error];
        [path release];
        if (!image) {
            printf("Failed to load image: %s\n", error ? error.message.UTF8String : "unknown");
            quit(ctx);
            return false;
        }

        m_width = image.width;
        m_height = image.height;
        m_texture = [image createTextureWithRenderer:ctx.renderer error:&error];
        [image release];

        if (m_texture == nullptr || !m_texture->valid()) {
            printf("Failed to create texture: %s\n", error ? error.message.UTF8String : "unknown");
            quit(ctx);
            return false;
        }

        m_matrix = ryo::Matrix4::scaling((float)m_width / (float)m_height * 720.0f, 720.0f, 1.0f);
        m_stencilMatrix = ryo::Matrix4::scaling(450.0f, 450.0f, 1.0f);

        m_stencil = new ryo::Stencil(ctx.renderer);
        if (m_stencil == nullptr || !m_stencil->valid()) {
            printf("Failed to create stencil: %s\n", ryo::error_get());
            quit(ctx);
            return false;
        }

        m_stencil->draw_rounded_rectangle(m_stencilMatrix, 0.2f, &m_camera);

        m_settings.cullMode = ryo::CULL_MODE_BACK;
        m_settings.windingOrder = ryo::WINDING_ORDER_COUNTER_CLOCKWISE;
        m_settings.fillMode = ryo::FILL_MODE_SOLID;
        m_settings.depthMode = ryo::DEPTH_MODE_DISABLED;
        m_settings.camera = &m_camera;
        m_settings.pipelineOverride = nullptr;
        m_settings.stencil = m_stencil;

        return true;
    }

    void quit(DemoContext& ctx) override {
        if (m_texture != nullptr) {
            delete m_texture;
            m_texture = nullptr;
        }

        if (m_stencil != nullptr) {
            delete m_stencil;
            m_stencil = nullptr;
        }
    }

    void render(DemoContext& ctx) override {
        ryo::Color clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        ryo::Color imageColor = {1.0f, 1.0f, 1.0f, 1.0f};

        ctx.renderer.draw_pass_begin(nullptr, &clearColor);

        ctx.renderer.draw_rect_texture(*m_texture, ryo::SAMPLER_TYPE_LINEAR, imageColor, m_matrix, &m_settings);

        ctx.renderer.draw_pass_end();
    }

private:
    ryo::Camera m_camera;
    ryo::DrawSettings m_settings;
    ryo::Matrix4 m_matrix;
    ryo::Matrix4 m_stencilMatrix;
    int m_width, m_height;
    ryo::Texture* m_texture;
    ryo::Stencil* m_stencil;
};
