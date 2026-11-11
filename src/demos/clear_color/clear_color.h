#pragma once

#include "../../demo.h"
#include <cmath>

class ClearColorDemo : public Demo {
public:
    const char* title_get() const override {
        return "Clear Color";
    }

    void update(DemoContext& ctx, float dt) override {
        m_time += dt;
    }

    void render(DemoContext& ctx) override {
        RYColor clearColor = RYColorMake(
            0.5f + 0.5f * sinf(m_time),
            0.5f + 0.5f * sinf(m_time + 2.094f),
            0.5f + 0.5f * sinf(m_time + 4.188f),
            1.0f
        );

        [ctx.renderer drawPassBeginWithRenderTarget:nil clearColor:&clearColor];
        [ctx.renderer drawPassEnd];
    }

private:
    float m_time = 0.0f;
};
