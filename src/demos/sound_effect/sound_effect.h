#pragma once

#include "../../demo.h"
#include <cmath>

class SoundEffectDemo : public Demo {
public:
    const char* title_get() const override {
        return "Sound Effect";
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

        RYError *error = nil;
        RYString *fontPath = [[RYString alloc] initWithUTF8String:"assets/lato-400-normal.ttf"];
        m_font = [RYFont newFontWithPath:fontPath size:30.0f error:&error];
        [fontPath release];
        if (!m_font) {
            printf("Failed to load font: %s\n", error ? error.message.UTF8String : "unknown");
            quit(ctx);
            return 0;
        }

        RYString *text = [[RYString alloc] initWithUTF8String:"Press X or ENTER to play the sound effect!"];
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

        if (m_texture == nullptr || !m_texture->valid()) {
            printf("Failed to create text texture: %s\n", error ? error.message.UTF8String : "unknown");
            quit(ctx);
            return false;
        }

        RYString *soundPath = [[RYString alloc] initWithUTF8String:"assets/mikudayo.mp3"];
        m_sound = [RYSound newSoundWithEngine:ctx.audioEngine path:soundPath loadType:RYSoundLoadTypeDecode error:&error];
        [soundPath release];

        if (!m_sound) {
            printf("Failed to load sound: %s\n", error.message.UTF8String);
            quit(ctx);
            return false;
        }

        return true;
    }

    void on_event(DemoContext& ctx, const ryo::Event& event) override {
        switch (event.type) {
            case ryo::EVENT_PAD_BUTTON_DOWN:
                if (event.data.pad.button == ryo::PAD_BUTTON_DOWN) {
                    [m_sound play];
                }
                break;
            
            case ryo::EVENT_KEY_DOWN:
                if (event.data.key.code == ryo::KEY_RETURN) {
                    [m_sound play];
                }
                break;

            default:
                break;
        }
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

        if (m_font != nil) {
            [m_font release];
            m_font = nil;
        }

        if (m_sound != nil) {
            [m_sound stop];
            [m_sound release];
            m_sound = nil;
        }
    }

    void render(DemoContext& ctx) override {
        ryo::Color clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        ryo::Color imageColor = {1.0f, 1.0f, 1.0f, 1.0f};

        ryo::Matrix4 matrix = ryo::Matrix4::scaling((float)m_width, (float)m_height, 1.0f);

        ctx.renderer.draw_pass_begin(nullptr, &clearColor);

        ctx.renderer.draw_rect_texture(*m_texture, ryo::SAMPLER_TYPE_LINEAR, imageColor, matrix, &m_settings);

        ctx.renderer.draw_pass_end();
    }

private:
    ryo::Pipeline* m_pipeline;
    ryo::Camera m_camera;
    ryo::DrawSettings m_settings;
    RYFont* m_font = nil;
    int m_width, m_height;
    ryo::Texture* m_texture;
#ifdef __OBJC__
    RYSound* m_sound = nil;
#else
    void* m_sound = nullptr;
#endif
};
