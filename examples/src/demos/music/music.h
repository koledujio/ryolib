#pragma once

#include "../../demo.h"
#include <cmath>

class MusicDemo : public Demo {
public:
    const char* title_get() const override {
        return "Music";
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
        fontConfig.fontSize = 30.0f;

        m_font = new ryo::Font(fontConfig);
        if (!m_font->valid()) {
            printf("Failed to load font: %s\n", ryo::error_get());
            quit(ctx);
            return 0;
        }

        const char* text = "Press X or ENTER to play/pause the music!";
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

        m_sound = new ryo::Sound(ctx.audioEngine, "assets/float-play.flac",
                                 ryo::SOUND_LOAD_TYPE_STREAM);
        if (!m_sound->valid()) {
            printf("Failed to load sound: %s\n", ryo::error_get());
            quit(ctx);
            return false;
        }

        m_sound->looping_set(true);
        m_playing = false;

        return true;
    }

    void on_event(DemoContext& ctx, const ryo::Event& event) override {
        switch (event.type) {
            case ryo::EVENT_PAD_BUTTON_DOWN:
                if (event.data.pad.button == ryo::PAD_BUTTON_DOWN) {
                    if (m_playing)
                        m_sound->pause();
                    else
                        m_sound->play();

                    m_playing = !m_playing;
                }
                
                break;
            
            case ryo::EVENT_KEY_DOWN:
                if (event.data.key.code == ryo::KEY_RETURN) {
                    if (m_playing)
                        m_sound->pause();
                    else
                        m_sound->play();

                    m_playing = !m_playing;
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

        if (m_font != nullptr) {
            delete m_font;
            m_font = nullptr;
        }

        if (m_sound != nullptr) {
            delete m_sound;
            m_sound = nullptr;
        }
    }

    void update(DemoContext& ctx, float dt) override {
        m_time += dt;
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
    float m_time = 0.0f;
    ryo::Pipeline* m_pipeline;
    ryo::Camera m_camera;
    ryo::DrawSettings m_settings;
    ryo::Font* m_font;
    int m_width, m_height;
    ryo::Texture* m_texture;
    ryo::Sound* m_sound;
    bool m_playing;
};
