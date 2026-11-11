#pragma once

#include "../../demo.h"
#include <cmath>

class RenderTargetDemo : public Demo {
public:
    const char* title_get() const override {
        return "Render Target";
    }

    bool init(DemoContext& ctx) override {
        RYPipelineDesc pipelineDesc = {};
        pipelineDesc.shader = RYBuiltinShaderTexture;
        pipelineDesc.blendMode = RYBlendModeAlpha;
        pipelineDesc.vertexLayout = RYVertexLayoutTypePositionUV;

        RYError *pipeError = nil;
        m_pipeline = [RYPipeline newPipelineWithRenderer:ctx.renderer desc:&pipelineDesc error:&pipeError];
        if (!m_pipeline) {
            printf("Failed to create pipeline: %s\n", pipeError ? pipeError.message.UTF8String : "unknown");
            return false;
        }

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
        m_settings.pipelineOverride = m_pipeline;
        m_settings.stencil = nil;

        RYError *error = nil;
        RYString *fontPath = [[RYString alloc] initWithUTF8String:"assets/lato-400-normal.ttf"];
        m_font = [RYFont newFontWithPath:fontPath size:30.0f error:&error];
        [fontPath release];
        if (!m_font) {
            printf("Failed to load font: %s\n", error ? error.message.UTF8String : "unknown");
            quit(ctx);
            return 0;
        }

        RYString *text = [[RYString alloc] initWithUTF8String:"This text is being rendered off-screen to a render target, then the render target is drawn."];
        RYColor textColor = RYColorMake(1.0f, 1.0f, 1.0f, 1.0f);
        RYImage *image = [m_font renderLineToImage:text color:textColor error:&error];
        [text release];
        if (!image) {
            printf("Failed to render text line: %s\n", error ? error.message.UTF8String : "unknown");
            quit(ctx);
            return false;
        }

        m_width = image.width;
        m_height = image.height;
        m_texture = [image createTextureWithRenderer:ctx.renderer error:&error];
        [image release];

        if (m_texture == nil) {
            printf("Failed to create text texture: %s\n", error ? error.message.UTF8String : "unknown");
            quit(ctx);
            return false;
        }

        m_renderTarget = [RYRenderTarget newRenderTargetWithRenderer:ctx.renderer width:m_width height:m_height error:&error];
        if (!m_renderTarget) {
            printf("Failed to create render target: %s\n", error ? error.message.UTF8String : "unknown");
            quit(ctx);
            return false;
        }

        m_textDrawn = false;

        m_renderTargetCamera.projectionType = RYCameraProjectionTypeOrthographic;
        m_renderTargetCamera.orthographicSize = m_height;
        m_renderTargetCamera.nearPlane = -1.0f;
        m_renderTargetCamera.farPlane = 1.0f;
        m_renderTargetCamera.position = {0.0f, 0.0f, 1.0f};
        m_renderTargetCamera.target = {0.0f, 0.0f, 0.0f};
        m_renderTargetCamera.up = {0.0f, 1.0f, 0.0f};

        m_renderTargetSettings.cullMode = RYCullModeBack;
        m_renderTargetSettings.windingOrder = RYWindingOrderCounterClockwise;
        m_renderTargetSettings.fillMode = RYFillModeSolid;
        m_renderTargetSettings.depthMode = RYDepthModeDisabled;
        m_renderTargetSettings.camera = &m_renderTargetCamera;
        m_renderTargetSettings.pipelineOverride = m_pipeline;
        m_renderTargetSettings.stencil = nil;

        return true;
    }

    void quit(DemoContext& ctx) override {
        if (m_renderTarget != nil) {
            [m_renderTarget release];
            m_renderTarget = nil;
        }

        if (m_texture != nil) {
            [m_texture release];
            m_texture = nil;
        }

        if (m_pipeline != nil) {
            [m_pipeline release];
            m_pipeline = nil;
        }

        if (m_font != nil) {
            [m_font release];
            m_font = nil;
        }
    }

    void render(DemoContext& ctx) override {
        RYColor clearColor = RYColorMake(0.0f, 0.0f, 0.0f, 1.0f);
        RYColor imageColor = RYColorMake(1.0f, 1.0f, 1.0f, 1.0f);

        ryo::Matrix4 matrix = ryo::Matrix4::scaling((float)m_width, (float)m_height, 1.0f);

        [ctx.renderer drawPassBeginWithRenderTarget:m_renderTarget clearColor:&clearColor];

        [ctx.renderer drawRectTexture:m_texture samplerType:RYSamplerTypeLinear color:imageColor matrix:matrix settings:&m_renderTargetSettings];

        [ctx.renderer drawPassEnd];

        [ctx.renderer drawPassBeginWithRenderTarget:nil clearColor:&clearColor];

        [ctx.renderer drawRectTexture:m_renderTarget.colorTexture samplerType:RYSamplerTypeLinear color:imageColor matrix:matrix settings:&m_settings];

        [ctx.renderer drawPassEnd];
    }

private:
    RYPipeline *m_pipeline = nil;
    RYCamera m_camera;
    RYDrawSettings m_settings;
    RYCamera m_renderTargetCamera;
    RYDrawSettings m_renderTargetSettings;
    RYFont* m_font = nil;
    int m_width = 0, m_height = 0;
    RYTexture *m_texture = nil;
    RYRenderTarget *m_renderTarget = nil;
    bool m_textDrawn = false;
};
