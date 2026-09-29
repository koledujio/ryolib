#include <ryolib/ryolib.h>

#include "demo.h"
#include "demos/clear_color/clear_color.h"
#include "demos/draw_color/draw_color.h"
#include "demos/draw_image/draw_image.h"
#include "demos/draw_image_transparency/draw_image_transparency.h"
#include "demos/draw_texture/draw_texture.h"
#include "demos/draw_mesh/draw_mesh.h"
#include "demos/text/text.h"
#include "demos/text_fallback_font/text_fallback_font.h"
#include "demos/sound_effect/sound_effect.h"
#include "demos/music/music.h"
#include "demos/render_target/render_target.h"
#include "demos/depth_test/depth_test.h"
#include "demos/stencil_rectangle/stencil_rectangle.h"
#include "demos/stencil_rounded_rectangle/stencil_rounded_rectangle.h"
#include "demos/stencil_oval/stencil_oval.h"
#include "demos/stencil_render_target/stencil_render_target.h"
#include "demos/stencil_streched_rounded_rectangle/stencil_streched_rounded_rectangle.h"

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <cmath>

struct MenuItem {
    std::string title;
    ryo::Texture* texture = nullptr;
    int textWidth = 0;
    int textHeight = 0;
};

struct MenuState {
    std::vector<MenuItem> items;
    unsigned int selectedIndex = 0;
    unsigned int scrollOffset = 0;
    #if __vita__
    int visibleItemCount = 10;
    #else
    int visibleItemCount = 14;
    #endif
    float itemHeight = 44.0f;
};

static void menu_move_up(MenuState& menu) {
    if (menu.items.empty())
        return;

    if (menu.selectedIndex == 0) {
        menu.selectedIndex = (int)menu.items.size() - 1;
    } else {
        menu.selectedIndex--;
    }

    if (menu.selectedIndex < menu.scrollOffset) {
        menu.scrollOffset = menu.selectedIndex;
    } else if (menu.selectedIndex >= menu.scrollOffset + menu.visibleItemCount) {
        menu.scrollOffset = fmax(0, menu.selectedIndex - menu.visibleItemCount + 1);
    }
}

static void menu_move_down(MenuState& menu) {
    if (menu.items.empty())
        return;

    menu.selectedIndex++;
    if (menu.selectedIndex >= (int)menu.items.size()) {
        menu.selectedIndex = 0;
    }

    if (menu.selectedIndex >= menu.scrollOffset + menu.visibleItemCount) {
        menu.scrollOffset = menu.selectedIndex - menu.visibleItemCount + 1;
    } else if (menu.selectedIndex < menu.scrollOffset) {
        menu.scrollOffset = menu.selectedIndex;
    }
}

static void menu_init(MenuState& menu, const ryo::Renderer& renderer, const ryo::Font& font, const std::vector<std::string>& titles) {
    ryo::Color textColor = {1.0f, 1.0f, 1.0f, 1.0f};

    for (const auto& title : titles) {
        MenuItem item;
        item.title = title;

        void* pixels = nullptr;
        if (font.pixels_render_line(title.c_str(), textColor, item.textWidth, item.textHeight, pixels)) {
            item.texture = new ryo::Texture(renderer, pixels, item.textWidth, item.textHeight);
            font.pixels_free(pixels);
        }

        menu.items.push_back(item);
    }
}

static void menu_destroy(MenuState& menu) {
    for (const auto& item : menu.items) {
        if (item.texture)
            delete item.texture;
    }
    menu.items.clear();
}

static void menu_render(
    const MenuState& menu, 
    const ryo::Renderer& renderer, 
    const ryo::Pipeline& textPipeline,
    const ryo::DrawSettings& baseSettings,
    float screenWidth,
    float screenHeight
) {
    ryo::DrawSettings textSettings = baseSettings;
    textSettings.pipelineOverride = &textPipeline;

    float startX = -screenWidth * 0.5f + 100.0f;
    float startY = screenHeight * 0.5f - 100.0f;

    int endIndex = fmin((int)menu.items.size(), menu.scrollOffset + menu.visibleItemCount);

    for (int i = menu.scrollOffset; i < endIndex; ++i) {
        const auto& item = menu.items[i];
        if (!item.texture || !item.texture->valid())
            continue;

        float rowY = startY - (float)(i - menu.scrollOffset) * menu.itemHeight;
        bool isSelected = (i == menu.selectedIndex);

        if (isSelected) {
            ryo::Matrix4 barMatrix = ryo::Matrix4::translation(0.0f, rowY, 0.0f) * ryo::Matrix4::scaling(screenWidth, menu.itemHeight - 4.0f, 1.0f);
            ryo::Color highlightColor = {0.980f, 0.392f, 0.392f, 1.0f};
            renderer.draw_rect_color(highlightColor, barMatrix, &baseSettings);
        }

        float textX = startX + ((float)item.textWidth * 0.5f) + (isSelected ? 16.0f : 0.0f);
        ryo::Matrix4 textMatrix = ryo::Matrix4::translation(textX, rowY, 0.0f) * ryo::Matrix4::scaling((float)item.textWidth, (float)item.textHeight, 1.0f);
        ryo::Color tint = ryo::Color{1.0f, 1.0f, 1.0f, 1.0f};
        renderer.draw_rect_texture(*item.texture, ryo::SAMPLER_TYPE_LINEAR, tint, textMatrix, &textSettings);
    }
}

int main() {
    unsigned int width, height;
#if __vita__
    width = 960;
    height = 544;
#else
    width = 1280;
    height = 720;
#endif

    ryo::Window window("Ryolib demonstration menu", width, height);
    if (!window.valid()) {
        printf("Failed to create window: %s", ryo::error_get());
        return 0;
    }

    ryo::AudioEngine audioEngine;
    if (!audioEngine.valid()) {
        printf("Failed to create audio engine: %s\n", ryo::error_get());
    }

    ryo::RendererConfig config = {};
    config.nativeWindowHandle = window.nwh_get();
    config.width = width;
    config.height = height;

    ryo::Renderer renderer(config);
    if (!renderer.valid()) {
        printf("Failed to create renderer: %s", ryo::error_get());
        return 0;
    }

    ryo::Camera menuCamera = {};
    menuCamera.projectionType = ryo::CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC;
    menuCamera.orthographicSize = height;
    menuCamera.nearPlane = -1.0f;
    menuCamera.farPlane = 1.0f;
    menuCamera.position = {0.0f, 0.0f, 1.0f};
    menuCamera.target = {0.0f, 0.0f, 0.0f};
    menuCamera.up = {0.0f, 1.0f, 0.0f};
    
    ryo::DrawSettings menuDrawSettings = {};
    menuDrawSettings.cullMode = ryo::CULL_MODE_BACK;
    menuDrawSettings.windingOrder = ryo::WINDING_ORDER_COUNTER_CLOCKWISE;
    menuDrawSettings.fillMode = ryo::FILL_MODE_SOLID;
    menuDrawSettings.depthMode = ryo::DEPTH_MODE_DISABLED;
    menuDrawSettings.camera = &menuCamera;
    menuDrawSettings.pipelineOverride = nullptr;
    menuDrawSettings.stencil = nullptr;

    ryo::PipelineDesc texBlendDesc = {};
    texBlendDesc.shader = ryo::BUILTIN_SHADER_TEXTURE;
    texBlendDesc.blendMode = ryo::BLEND_MODE_ALPHA;
    texBlendDesc.vertexLayout = ryo::VERTEX_LAYOUT_POSITION_UV;

    ryo::Pipeline texBlend(renderer, texBlendDesc);
    if (!texBlend.valid()) {
        printf("Failed to create texture blended pipeline: %s\n", ryo::error_get());
        return 0;
    }

    ryo::Color blackColor = {0.0f, 0.0f, 0.0f, 1.0f};

    ryo::FontConfig fontConfig;
    fontConfig.path = "assets/lato-400-normal.ttf";
    fontConfig.fontSize = 32.0f;

    ryo::Font font(fontConfig);
    if (!font.valid()) {
        printf("Failed to load font: %s", ryo::error_get());
        return 0;
    }

    std::vector<std::unique_ptr<Demo>> demos;
    demos.push_back(std::make_unique<ClearColorDemo>());
    demos.push_back(std::make_unique<DrawColorDemo>());
    demos.push_back(std::make_unique<DrawImageDemo>());
    demos.push_back(std::make_unique<DrawImageTransparencyDemo>());
    demos.push_back(std::make_unique<DrawTextureDemo>());
    demos.push_back(std::make_unique<DrawMeshDemo>());
    demos.push_back(std::make_unique<TextDemo>());
    demos.push_back(std::make_unique<TextFallbackFontDemo>());
    demos.push_back(std::make_unique<SoundEffectDemo>());
    demos.push_back(std::make_unique<MusicDemo>());
    demos.push_back(std::make_unique<RenderTargetDemo>());
    demos.push_back(std::make_unique<DepthTestDemo>());
    demos.push_back(std::make_unique<StencilRectangleDemo>());
    demos.push_back(std::make_unique<StencilRoundedRectangleDemo>());
    demos.push_back(std::make_unique<StencilOvalDemo>());
    demos.push_back(std::make_unique<StencilRenderTargetDemo>());
    demos.push_back(std::make_unique<StencilStrechedRoundedRectangleDemo>());

    MenuState menuState;
    std::vector<std::string> titles;
    for (const auto& demo : demos) {
        titles.push_back(demo->title_get());
    }
    menu_init(menuState, renderer, font, titles);

    ryo::Gamepad* gamepad = nullptr;
    Demo* activeDemo = nullptr;

    DemoContext ctx = {
        window,
        renderer,
        audioEngine,
        gamepad,
        width,
        height,
        menuDrawSettings,
        texBlend,
        font
    };

    ryo::Event windowEvent;
    bool running = true;

    while (running) {
        while (ryo::Window::event_poll(windowEvent)) {
            if (windowEvent.type == ryo::EVENT_PAD_ADDED) {
                if (!gamepad) {
                    gamepad = new ryo::Gamepad(windowEvent.data.pad.id);
                    if (!gamepad->valid()) {
                        delete gamepad;
                        gamepad = nullptr;
                    }
                }
            } else if (windowEvent.type == ryo::EVENT_PAD_REMOVED) {
                if (gamepad) {
                    delete gamepad;
                    gamepad = nullptr;
                }
            }

            if (windowEvent.type == ryo::EVENT_QUIT) {
                if (activeDemo) {
                    activeDemo->quit(ctx);
                    activeDemo = nullptr;
                }
                running = false;
                break;
            }

            if (activeDemo != nullptr) {
                bool shouldExitDemo = false;
                if (windowEvent.type == ryo::EVENT_KEY_DOWN && windowEvent.data.key.code == ryo::KEY_ESCAPE) {
                    shouldExitDemo = true;
                } else if (windowEvent.type == ryo::EVENT_PAD_BUTTON_DOWN && windowEvent.data.pad.button == ryo::PAD_BUTTON_START) {
                    shouldExitDemo = true;
                }

                if (shouldExitDemo) {
                    activeDemo->quit(ctx);
                    activeDemo = nullptr;
                } else {
                    activeDemo->on_event(ctx, windowEvent);
                }
            } else {
                switch (windowEvent.type) {
                    case ryo::EVENT_PAD_BUTTON_DOWN:
                        if (!gamepad || !gamepad->valid())
                            continue;

                        switch (windowEvent.data.pad.button) {
                            case ryo::PAD_BUTTON_DPAD_DOWN:
                                menu_move_down(menuState);
                                break;
                            
                            case ryo::PAD_BUTTON_DPAD_UP:
                                menu_move_up(menuState);
                                break;
                            
                            case ryo::PAD_BUTTON_START:
                                running = false;
                                break;
                            
                            case ryo::PAD_BUTTON_DOWN:
                                if (menuState.selectedIndex < demos.size()) {
                                    activeDemo = demos[menuState.selectedIndex].get();
                                    if (!activeDemo->init(ctx)) {
                                        activeDemo = nullptr;
                                    }
                                }
                                break;
                            
                            default:
                                break;
                        }
                        break;
                    
                    case ryo::EVENT_KEY_DOWN:
                        switch (windowEvent.data.key.code) {
                            case ryo::KEY_DOWN:
                                menu_move_down(menuState);
                                break;
                            
                            case ryo::KEY_UP:
                                menu_move_up(menuState);
                                break;
                            
                            case ryo::KEY_ESCAPE:
                                running = false;
                                break;

                            case ryo::KEY_RETURN:
                                if (menuState.selectedIndex < demos.size()) {
                                    activeDemo = demos[menuState.selectedIndex].get();
                                    if (!activeDemo->init(ctx)) {
                                        activeDemo = nullptr;
                                    }
                                }
                                break;
                            
                            default:
                                break;
                        }
                        break;
                    
                    default:
                        break;
                }
            }
        }

        float dt = renderer.frame_time_get();

        renderer.frame_begin();

        if (activeDemo != nullptr) {
            activeDemo->update(ctx, dt);
            activeDemo->render(ctx);
        } else {
            renderer.draw_pass_begin(nullptr, &blackColor);
            menu_render(menuState, renderer, texBlend, menuDrawSettings, width, height);
            renderer.draw_pass_end();
        }

        renderer.frame_end();
    }

    if (activeDemo != nullptr) {
        activeDemo->quit(ctx);
        activeDemo = nullptr;
    }

    if (gamepad)
        delete gamepad;

    menu_destroy(menuState);
    return 0;
}