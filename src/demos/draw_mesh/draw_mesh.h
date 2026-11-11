#pragma once

#include "../../demo.h"
#include <cmath>

class DrawMeshDemo : public Demo {
public:
    const char* title_get() const override {
        return "Draw Mesh";
    }

    bool init(DemoContext& ctx) override {
        m_camera.projectionType = ryo::CAMERA_PROJECTION_TYPE_PERSPECTIVE;
        m_camera.fovY = 60.0f;
        m_camera.nearPlane = 0.1f;
        m_camera.farPlane = 100.0f;
        m_camera.position = {0.0f, 0.0f, 3.0f};
        m_camera.target = {0.0f, 0.0f, 0.0f};
        m_camera.up = {0.0f, 1.0f, 0.0f};

        m_settings.cullMode = ryo::CULL_MODE_BACK;
        m_settings.windingOrder = ryo::WINDING_ORDER_COUNTER_CLOCKWISE;
        m_settings.fillMode = ryo::FILL_MODE_SOLID;
        m_settings.depthMode = ryo::DEPTH_MODE_DISABLED;
        m_settings.camera = &m_camera;
        m_settings.pipelineOverride = nullptr;
        m_settings.stencil = nullptr;

        uint8_t checkerboardPixels[16] = {
            255, 0, 255, 255,
            0, 0, 0, 255,
            0, 0, 0, 255,
            255, 0, 255, 255
        };  
        
        m_width = 2;
        m_height = 2;

        m_texture = new ryo::Texture(ctx.renderer, checkerboardPixels, m_width, m_height);
        if (!m_texture->valid()) {
            printf("Failed to load texture: %s\n", ryo::error_get());
            quit(ctx);
            return false;
        }

        m_matrix = ryo::Matrix4::scaling((float)m_width / (float)m_height * 720.0f, 720.0f, 1.0f);

        ryo::PositionUVVertex cubeVertices[] = {
            { {-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f} },
            { {0.5f, -0.5f, 0.5f}, {1.0f, 1.0f} },
            { {0.5f, 0.5f, 0.5f}, {1.0f, 0.0f} },
            { {-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f} },

            { {-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f} },
            { {0.5f, -0.5f, -0.5f}, {1.0f, 1.0f} },
            { {0.5f, 0.5f, -0.5f}, {1.0f, 0.0f} },
            { {-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f} },
        };

        uint32_t cubeIndices[] = {
            0, 1, 2,  2, 3, 0,
            1, 5, 6,  6, 2, 1,
            5, 4, 7,  7, 6, 5,
            4, 0, 3,  3, 7, 4,
            3, 2, 6,  6, 7, 3,
            4, 5, 1,  1, 0, 4
        };

        m_vertexBuffer = new ryo::Buffer(ctx.renderer, cubeVertices, sizeof(ryo::PositionUVVertex) * 8, ryo::BUFFER_TYPE_VERTEX);
        if (m_vertexBuffer == nullptr || !m_vertexBuffer->valid()) {
            printf("Failed to create vertex buffer: %s\n", ryo::error_get());
            quit(ctx);
            return false;
        }

        m_indexBuffer = new ryo::Buffer(ctx.renderer, cubeIndices, sizeof(uint32_t) * 36, ryo::BUFFER_TYPE_INDEX);
        if (m_indexBuffer == nullptr || !m_indexBuffer->valid()) {
            printf("Failed to create index buffer: %s\n", ryo::error_get());
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

        if (m_vertexBuffer != nullptr) {
            delete m_vertexBuffer;
            m_vertexBuffer = nullptr;
        }

        if (m_indexBuffer != nullptr) {
            delete m_indexBuffer;
            m_indexBuffer = nullptr;
        }
    }

    void update(DemoContext& ctx, float dt) override {
        m_time += dt;
    }

    void render(DemoContext& ctx) override {
        ryo::Color clearColor = {0.15f, 0.15f, 0.15f, 1.0f};
        ryo::Color textureColor = {1.0f, 1.0f, 1.0f, 1.0f};

        ryo::Matrix4 model = ryo::Matrix4::rotation(m_time, m_time * 0.7f, 0.0f);

        ctx.renderer.draw_pass_begin(nullptr, &clearColor);

        ctx.renderer.draw_mesh_texture(*m_vertexBuffer, *m_indexBuffer, 36, *m_texture, ryo::SAMPLER_TYPE_NEAREST, textureColor, model, &m_settings);

        ctx.renderer.draw_pass_end();
    }

private:
    float m_time = 0.0f;
    ryo::Camera m_camera;
    ryo::DrawSettings m_settings;
    ryo::Matrix4 m_matrix;
    int m_width, m_height;
    ryo::Texture* m_texture;
    ryo::Buffer* m_vertexBuffer;
    ryo::Buffer* m_indexBuffer;
};
