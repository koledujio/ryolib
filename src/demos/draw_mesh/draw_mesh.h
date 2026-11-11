#pragma once

#include "../../demo.h"
#include <cmath>

class DrawMeshDemo : public Demo {
public:
    const char* title_get() const override {
        return "Draw Mesh";
    }

    bool init(DemoContext& ctx) override {
        m_camera.projectionType = RYCameraProjectionTypePerspective;
        m_camera.fovY = 60.0f;
        m_camera.nearPlane = 0.1f;
        m_camera.farPlane = 100.0f;
        m_camera.position = {0.0f, 0.0f, 3.0f};
        m_camera.target = {0.0f, 0.0f, 0.0f};
        m_camera.up = {0.0f, 1.0f, 0.0f};

        m_settings.cullMode = RYCullModeBack;
        m_settings.windingOrder = RYWindingOrderCounterClockwise;
        m_settings.fillMode = RYFillModeSolid;
        m_settings.depthMode = RYDepthModeDisabled;
        m_settings.camera = &m_camera;
        m_settings.pipelineOverride = nil;
        m_settings.stencil = nil;

        uint8_t checkerboardPixels[16] = {
            255, 0, 255, 255,
            0, 0, 0, 255,
            0, 0, 0, 255,
            255, 0, 255, 255
        };  
        
        m_width = 2;
        m_height = 2;

        RYError *error = nil;
        m_texture = [RYTexture newTextureWithRenderer:ctx.renderer pixels:checkerboardPixels width:m_width height:m_height error:&error];
        if (!m_texture) {
            printf("Failed to load texture: %s\n", error ? error.message.UTF8String : "unknown");
            quit(ctx);
            return false;
        }

        m_matrix = ryo::Matrix4::scaling((float)m_width / (float)m_height * 720.0f, 720.0f, 1.0f);

        RYPositionUVVertex cubeVertices[] = {
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

        m_vertexBuffer = [RYBuffer newBufferWithRenderer:ctx.renderer data:cubeVertices size:sizeof(RYPositionUVVertex) * 8 type:RYBufferTypeVertex error:&error];
        if (!m_vertexBuffer) {
            printf("Failed to create vertex buffer: %s\n", error ? error.message.UTF8String : "unknown");
            quit(ctx);
            return false;
        }

        m_indexBuffer = [RYBuffer newBufferWithRenderer:ctx.renderer data:cubeIndices size:sizeof(uint32_t) * 36 type:RYBufferTypeIndex error:&error];
        if (!m_indexBuffer) {
            printf("Failed to create index buffer: %s\n", error ? error.message.UTF8String : "unknown");
            quit(ctx);
            return false;
        }

        return true;
    }

    void quit(DemoContext& ctx) override {
        if (m_texture != nil) {
            [m_texture release];
            m_texture = nil;
        }

        if (m_vertexBuffer != nil) {
            [m_vertexBuffer release];
            m_vertexBuffer = nil;
        }

        if (m_indexBuffer != nil) {
            [m_indexBuffer release];
            m_indexBuffer = nil;
        }
    }

    void update(DemoContext& ctx, float dt) override {
        m_time += dt;
    }

    void render(DemoContext& ctx) override {
        RYColor clearColor = RYColorMake(0.15f, 0.15f, 0.15f, 1.0f);
        RYColor textureColor = RYColorMake(1.0f, 1.0f, 1.0f, 1.0f);

        ryo::Matrix4 model = ryo::Matrix4::rotation(m_time, m_time * 0.7f, 0.0f);

        [ctx.renderer drawPassBeginWithRenderTarget:nil clearColor:&clearColor];

        [ctx.renderer drawMeshTextureWithVertexBuffer:m_vertexBuffer indexBuffer:m_indexBuffer indexCount:36 texture:m_texture samplerType:RYSamplerTypeNearest color:textureColor matrix:model settings:&m_settings];

        [ctx.renderer drawPassEnd];
    }

private:
    float m_time = 0.0f;
    RYCamera m_camera;
    RYDrawSettings m_settings;
    ryo::Matrix4 m_matrix;
    int m_width = 0, m_height = 0;
    RYTexture *m_texture = nil;
    RYBuffer *m_vertexBuffer = nil;
    RYBuffer *m_indexBuffer = nil;
};
