#pragma once

#include "../../demo.h"
#include <cmath>

class StencilOvalDemo : public Demo {
public:
    const char* title_get() const override {
        return "Stencil Oval";
    }

    bool init(DemoContext& ctx) override {
        m_camera.projectionType = RYCameraProjectionTypeOrthographic;
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

        if (m_texture == nil) {
            printf("Failed to create texture: %s\n", error ? error.message.UTF8String : "unknown");
            quit(ctx);
            return false;
        }

        m_matrix = ryo::Matrix4::scaling((float)m_width / (float)m_height * 720.0f, 720.0f, 1.0f);
        m_stencilMatrix = ryo::Matrix4::scaling(450.0f, 450.0f, 1.0f);

        m_stencil = [RYStencil newStencilWithRenderer:ctx.renderer error:&error];
        if (!m_stencil) {
            printf("Failed to create stencil: %s\n", error ? error.message.UTF8String : "unknown");
            quit(ctx);
            return false;
        }

        [m_stencil drawOval:m_stencilMatrix camera:&m_camera];

        m_settings.cullMode = RYCullModeBack;
        m_settings.windingOrder = RYWindingOrderCounterClockwise;
        m_settings.fillMode = RYFillModeSolid;
        m_settings.depthMode = RYDepthModeDisabled;
        m_settings.camera = &m_camera;
        m_settings.pipelineOverride = nil;
        m_settings.stencil = m_stencil;

        return true;
    }

    void quit(DemoContext& ctx) override {
        if (m_texture != nil) {
            [m_texture release];
            m_texture = nil;
        }

        if (m_stencil != nil) {
            [m_stencil release];
            m_stencil = nil;
        }
    }

    void render(DemoContext& ctx) override {
        RYColor clearColor = RYColorMake(0.0f, 0.0f, 0.0f, 1.0f);
        RYColor imageColor = RYColorMake(1.0f, 1.0f, 1.0f, 1.0f);

        [ctx.renderer drawPassBeginWithRenderTarget:nil clearColor:&clearColor];

        [ctx.renderer drawRectTexture:m_texture samplerType:RYSamplerTypeLinear color:imageColor matrix:m_matrix settings:&m_settings];

        [ctx.renderer drawPassEnd];
    }

private:
    RYCamera m_camera;
    RYDrawSettings m_settings;
    ryo::Matrix4 m_matrix;
    ryo::Matrix4 m_stencilMatrix;
    int m_width = 0, m_height = 0;
    RYTexture *m_texture = nil;
    RYStencil *m_stencil = nil;
};
