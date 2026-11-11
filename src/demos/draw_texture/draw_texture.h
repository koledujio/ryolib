#pragma once

#include "../../demo.h"
#include <cmath>

class DrawTextureDemo : public Demo {
public:
    const char* title_get() const override {
        return "Draw Texture";
    }

    bool init(DemoContext& ctx) override {
        m_camera.projectionType = RYCameraProjectionTypeOrthographic;
        m_camera.orthographicSize = 720.0f;
        m_camera.nearPlane = -1.0f;
        m_camera.farPlane = 1.0f;
        m_camera.position = {0.0f, 0.0f, 1.0f};
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
            return false;
        }

        m_matrix = ryo::Matrix4::scaling((float)m_width / (float)m_height * 720.0f, 720.0f, 1.0f);

        return true;
    }

    void quit(DemoContext& ctx) override {
        if (m_texture != nil) {
            [m_texture release];
            m_texture = nil;
        }
    }

    void render(DemoContext& ctx) override {
        RYColor clearColor = RYColorMake(0.0f, 0.0f, 0.0f, 1.0f);
        RYColor textureColor = RYColorMake(1.0f, 1.0f, 1.0f, 1.0f);

        [ctx.renderer drawPassBeginWithRenderTarget:nil clearColor:&clearColor];

        [ctx.renderer drawRectTexture:m_texture samplerType:RYSamplerTypeNearest color:textureColor matrix:m_matrix settings:&m_settings];

        [ctx.renderer drawPassEnd];
    }

private:
    RYCamera m_camera;
    RYDrawSettings m_settings;
    ryo::Matrix4 m_matrix;
    int m_width = 0, m_height = 0;
    RYTexture *m_texture = nil;
};
