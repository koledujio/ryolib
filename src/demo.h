#pragma once

#include <ryolib/ryolib.h>

struct DemoContext {
    ryo::Window& window;
    ryo::Renderer& renderer;
    RYAudioEngine *audioEngine;
    ryo::Gamepad*& gamepad;
    unsigned int screenWidth;
    unsigned int screenHeight;
    const ryo::DrawSettings& baseDrawSettings;
    const ryo::Pipeline& textPipeline;
    RYFont *font;
};

class Demo {
public:
    virtual ~Demo() = default;

    virtual const char* title_get() const = 0;
    virtual bool init(DemoContext& ctx) { return true; }
    virtual void on_event(DemoContext& ctx, const ryo::Event& event) {}
    virtual void update(DemoContext& ctx, float dt) {}
    virtual void render(DemoContext& ctx) = 0;
    virtual void quit(DemoContext& ctx) {}
};
