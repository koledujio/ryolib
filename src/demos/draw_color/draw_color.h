#pragma once

#include "../../demo.h"
#include <cmath>

class DrawColorDemo : public Demo {
public:
    const char* title_get() const override {
        return "Draw Color";
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

        m_matrix = ryo::Matrix4::scaling(200.0f, 200.0f, 1.0f);

        return true;
    }

    void update(DemoContext& ctx, float dt) override {
        m_time += dt;
    }

    void render(DemoContext& ctx) override {
        RYColor clearColor = RYColorMake(0.0f, 0.0f, 0.0f, 1.0f);
        RYColor rectColor = RYColorMake(1.0f, sin(m_time), cos(m_time), 1.0f);

        [ctx.renderer drawPassBeginWithRenderTarget:nil clearColor:&clearColor];

        [ctx.renderer drawRectColor:rectColor matrix:m_matrix settings:&m_settings];

        [ctx.renderer drawPassEnd];
    }

private:
    float m_time = 0.0f;
    RYCamera m_camera;
    RYDrawSettings m_settings;
    ryo::Matrix4 m_matrix;
};
