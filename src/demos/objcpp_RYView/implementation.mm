#include "objcpp_RYView.h"

const char* ObjcppRYViewDemo::title_get() const {
    return "Objective C++ RYView";
}

bool ObjcppRYViewDemo::init(DemoContext& ctx) {
    unsigned int physX = 0;
    unsigned int physY = 0;

    if (![ctx.renderer getSize:&physX height:&physY])
        return false;
    
    ryo::Vector2 physRes = {static_cast<float>(physX), static_cast<float>(physY)};

    float contScale = 0;

    if (![ctx.renderer getContentScale:&contScale])
        return false;
    
    m_view = [[RYView alloc] init];

    m_viewRoot = [[RYViewRoot alloc] initWithRenderer:ctx.renderer
                                   physicalResolution:physRes
                                         contentScale:contScale
                                          contentView:m_view];
    
    [m_view release];
    m_view = nil;
    
    if (!m_viewRoot)
        return false;

    m_viewRoot.backgroundColor = RYColorMake(1.0f, 0.0f, 0.0f, 1.0f);

    m_camera.projectionType = RYCameraProjectionTypeOrthographic;
    m_camera.orthographicSize = static_cast<float>(physY);
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

    m_matrix = ryo::Matrix4::scaling(static_cast<float>(physX), static_cast<float>(physY), 1.0f);
    
    return true;
}

void ObjcppRYViewDemo::update(DemoContext& ctx, float dt) {
    if (m_viewRoot)
        [m_viewRoot update:dt];
}

void ObjcppRYViewDemo::render(DemoContext& ctx) {
    static RYColor textureColor = RYColorMake(1.0f, 1.0f, 1.0f, 1.0f);
    RYColor clearColor = RYColorMake(0.0f, 0.0f, 0.0f, 1.0f);

    if (!m_viewRoot)
        return;

    [m_viewRoot layout];
    RYTexture *texture = [m_viewRoot draw];

    [ctx.renderer drawPassBeginWithRenderTarget:nil clearColor:&clearColor];

    if (texture)
        [ctx.renderer drawRectTexture:texture samplerType:RYSamplerTypeNearest color:textureColor matrix:m_matrix settings:&m_settings];

    [ctx.renderer drawPassEnd];
}

void ObjcppRYViewDemo::quit(DemoContext& ctx) {
    if (m_viewRoot) {
        [m_viewRoot release];
        m_viewRoot = nil;
    }
}
