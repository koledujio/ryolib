#pragma once

#include <ryolib/ryolib.h>

struct DemoContext {
    RYWindow *window;
    RYRenderer *renderer;
    RYAudioEngine *audioEngine;
    RYGamepad *&gamepad;
    unsigned int screenWidth;
    unsigned int screenHeight;
    const RYDrawSettings& baseDrawSettings;
    RYPipeline *textPipeline;
    RYFont *font;
};

class Demo {
public:
    virtual ~Demo() = default;

    virtual const char* title_get() const = 0;
    virtual bool init(DemoContext& ctx) { return true; }
    virtual void on_event(DemoContext& ctx, const RYEvent& event) {}
    virtual void update(DemoContext& ctx, float dt) {}
    virtual void render(DemoContext& ctx) = 0;
    virtual void quit(DemoContext& ctx) {}
};
