#include "objcpp_RYViewRoot.h"

const char* ObjcppRYViewRootDemo::title_get() const {
    return "Objective C++ RYViewRoot";
}

bool ObjcppRYViewRootDemo::init(DemoContext& ctx) {
    unsigned int physX = 0;
    unsigned int physY = 0;

    if (!ctx.renderer.size_get(physX, physY))
        return false;
    
    ryo::Vector2 physRes = {static_cast<float>(physX), static_cast<float>(physY)};

    float contScale = 0;

    if (!ctx.renderer.content_scale_get(contScale))
        return false;

    m_viewRoot = [[RYViewRoot alloc] initWithRenderer:&ctx.renderer
                                   physicalResolution:physRes
                                         contentScale:contScale
                                          contentView:nil];
    
    m_viewRoot.backgroundColor = {1.0f, 0.0f, 0.0f, 1.0f};
    ryo::Color color = m_viewRoot.backgroundColor;
    
    if (!m_viewRoot)
        return false;

    m_camera.projectionType = ryo::CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC;
    m_camera.orthographicSize = static_cast<float>(physY);
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
    m_settings.pipelineOverride = nullptr;
    m_settings.stencil = nullptr;

    m_matrix = ryo::Matrix4::scaling(static_cast<float>(physX), static_cast<float>(physY), 1.0f);
    
    return true;
}

void ObjcppRYViewRootDemo::update(DemoContext& ctx, float dt) {
    if (m_viewRoot)
        [m_viewRoot update:dt];
}

void ObjcppRYViewRootDemo::render(DemoContext& ctx) {
    static ryo::Color textureColor = {1.0f, 1.0f, 1.0f, 1.0f};
    ryo::Color clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

    if (!m_viewRoot)
        return;

    [m_viewRoot layout];
    const ryo::Texture *texture = [m_viewRoot draw];

    ctx.renderer.draw_pass_begin(nullptr, &clearColor);

    if (texture && texture->valid())
        ctx.renderer.draw_rect_texture(*texture, ryo::SAMPLER_TYPE_NEAREST, textureColor, m_matrix, &m_settings);

    ctx.renderer.draw_pass_end();
}

void ObjcppRYViewRootDemo::quit(DemoContext& ctx) {
    if (m_viewRoot) {
        [m_viewRoot release];
        m_viewRoot = nil;
    }
}
