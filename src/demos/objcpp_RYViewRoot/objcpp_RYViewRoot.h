#pragma once

#include "../../demo.h"

class ObjcppRYViewRootDemo : public Demo {
public:
    const char* title_get() const override;
    bool init(DemoContext& ctx) override;
    void update(DemoContext& ctx, float dt) override;
    void render(DemoContext& ctx) override;
    void quit(DemoContext& ctx) override;

private:
#ifdef __OBJC__
    RYViewRoot *m_viewRoot = nil;
#else
    void *m_viewRoot = nullptr;
#endif
    RYCamera m_camera;
    RYDrawSettings m_settings;
    ryo::Matrix4 m_matrix;
};
