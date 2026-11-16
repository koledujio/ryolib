#include <ryolib/ryolib.h>

#if defined(RYO_APPLE_IOS)
#include <SDL3/SDL_main.h>
#endif

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

static const float MENU_TOP_MARGIN = 50.0f;
static const float MENU_FONT_SIZE = 32.0f;

struct MenuState {
    std::vector<MenuItem> items;
    unsigned int selectedIndex = 0;
    unsigned int scrollOffset = 0;
    int visibleItemCount = 14;
    float itemHeight = 44.0f;
    float contentScale = 1.0f;
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

static const float TOUCH_TAP_MAX_MOVE = 0.06f;      // max movement while tapping
static const float TOUCH_TAP_MAX_TIME = 0.35f;      // max press duration of a tap
static const float TOUCH_TAP_CLUSTER_TIME = 0.25f;  // taps closer together are one gesture
static const float TOUCH_SWIPE_MIN_DY = 0.20f;      // min vertical movement of a swipe
static const int TOUCH_BACK_TAP_COUNT = 3;          // taps that trigger the back gesture

enum MenuGesture {
    MENU_GESTURE_NONE,
    MENU_GESTURE_UP,
    MENU_GESTURE_DOWN,
    MENU_GESTURE_ENTER,
    MENU_GESTURE_BACK
};

struct MenuTouch {
    struct Finger {
        unsigned int id = 0;
        float startX = 0.0f;
        float startY = 0.0f;
        float x = 0.0f;
        float y = 0.0f;
        float startTime = 0.0f;
        bool swiped = false; // the swipe of this finger was already recognized
    };

    std::vector<Finger> fingers;
    std::vector<float> tapTimes; // times of the taps of the current gesture
    float time = 0.0f;           // seconds since startup, advanced once per frame
};

static void menu_touch_begin(MenuTouch& touch, const ryo::Event& event) {
    MenuTouch::Finger finger;
    finger.id = event.data.touch.fingerId;
    finger.startX = event.data.touch.x;
    finger.startY = event.data.touch.y;
    finger.x = event.data.touch.x;
    finger.y = event.data.touch.y;
    finger.startTime = touch.time;

    touch.fingers.push_back(finger);
}

static MenuGesture menu_touch_move(MenuTouch& touch, const ryo::Event& event) {
    for (auto& finger : touch.fingers) {
        if (finger.id != event.data.touch.fingerId)
            continue;

        finger.x = event.data.touch.x;
        finger.y = event.data.touch.y;

        // a swipe moves the selection once, repeating it requires lifting the finger
        if (finger.swiped)
            return MENU_GESTURE_NONE;

        float deltaY = finger.y - finger.startY;
        if (fabsf(deltaY) < TOUCH_SWIPE_MIN_DY)
            return MENU_GESTURE_NONE;

        finger.swiped = true;

        return deltaY <= 0.0f ? MENU_GESTURE_UP : MENU_GESTURE_DOWN;
    }

    return MENU_GESTURE_NONE;
}

static MenuGesture menu_touch_end(MenuTouch& touch, const ryo::Event& event) {
    for (size_t i = 0; i < touch.fingers.size(); i++) {
        if (touch.fingers[i].id != event.data.touch.fingerId)
            continue;

        int fingersDown = (int)(touch.fingers.size() - i);

        const MenuTouch::Finger finger = touch.fingers[i];
        touch.fingers.erase(touch.fingers.begin() + i);

        bool tapped = !finger.swiped &&
                      (touch.time - finger.startTime) <= TOUCH_TAP_MAX_TIME &&
                      fabsf(finger.x - finger.startX) <= TOUCH_TAP_MAX_MOVE &&
                      fabsf(finger.y - finger.startY) <= TOUCH_TAP_MAX_MOVE;

        if (!tapped)
            return MENU_GESTURE_NONE;

        if (fingersDown >= TOUCH_BACK_TAP_COUNT) {
            touch.tapTimes.clear();
            return MENU_GESTURE_BACK;
        }

        touch.tapTimes.push_back(touch.time);
        if ((int)touch.tapTimes.size() >= TOUCH_BACK_TAP_COUNT) {
            touch.tapTimes.clear();
            return MENU_GESTURE_BACK;
        }

        return MENU_GESTURE_NONE;
    }

    return MENU_GESTURE_NONE;
}

static MenuGesture menu_touch_update(MenuTouch& touch) {
    size_t expired = 0;
    while (expired < touch.tapTimes.size() && touch.time - touch.tapTimes[expired] > TOUCH_TAP_CLUSTER_TIME)
        expired++;

    if (expired == 0)
        return MENU_GESTURE_NONE;

    bool singleTap = (expired == 1 && touch.tapTimes.size() == 1);
    touch.tapTimes.erase(touch.tapTimes.begin(), touch.tapTimes.begin() + expired);

    return singleTap ? MENU_GESTURE_ENTER : MENU_GESTURE_NONE;
}

static void menu_layout_update(MenuState& menu, unsigned int screenHeight) {
    menu.visibleItemCount = std::max(1, (int)((screenHeight - MENU_TOP_MARGIN) / menu.itemHeight));

    if (menu.selectedIndex < menu.scrollOffset)
        menu.scrollOffset = menu.selectedIndex;
    else if (menu.selectedIndex >= menu.scrollOffset + (unsigned int)menu.visibleItemCount)
        menu.scrollOffset = menu.selectedIndex - menu.visibleItemCount + 1;
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
    float startY = screenHeight * 0.5f - MENU_TOP_MARGIN;

    int endIndex = fmin((int)menu.items.size(), menu.scrollOffset + menu.visibleItemCount);

    float textScale = menu.contentScale > 0.0f ? 1.0f / menu.contentScale : 1.0f;

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

        float textWidth = (float)item.textWidth * textScale;
        float textHeight = (float)item.textHeight * textScale;

        float textX = startX + textWidth * 0.5f + (isSelected ? 16.0f : 0.0f);
        ryo::Matrix4 textMatrix = ryo::Matrix4::translation(textX, rowY, 0.0f) * ryo::Matrix4::scaling(textWidth, textHeight, 1.0f);
        ryo::Color tint = ryo::Color{1.0f, 1.0f, 1.0f, 1.0f};
        renderer.draw_rect_texture(*item.texture, ryo::SAMPLER_TYPE_LINEAR, tint, textMatrix, &textSettings);
    }
}

int main(int argc, char* argv[]) {
    unsigned int width, height;
    width = 1280;
    height = 720;

    ryo::Application app;
    if (!app.valid()) {
        printf("Failed to initialize application: %s\n", ryo::error_get());
        return 0;
    }

    ryo::Window window(app, "Ryolib demonstration menu", width, height, ryo::WINDOW_FLAG_RESIZABLE);
    if (!window.valid()) {
        printf("Failed to create window: %s", ryo::error_get());
        return 0;
    }

    window.size_get(width, height);

    ryo::AudioEngine audioEngine;
    if (!audioEngine.valid()) {
        printf("Failed to create audio engine: %s\n", ryo::error_get());
    }

    ryo::RendererConfig config = {};
    config.nativeWindowHandle = window.nwh_get();
    config.width = width;
    config.height = height;
    config.contentScale = window.hidpi_scale_get();

    ryo::Renderer renderer(config);
    if (!renderer.valid()) {
        printf("Failed to create renderer: %s", ryo::error_get());
        return 0;
    }

    unsigned int pixelWidth, pixelHeight;
    if (!renderer.size_get(pixelWidth, pixelHeight)) {
        printf("Failed to get renderer size: %s\n", ryo::error_get());
        return 0;
    }

    float contentScale = height > 0 ? (float)pixelHeight / (float)height : 1.0f;
    if (contentScale < 1.0f)
        contentScale = 1.0f;

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
    fontConfig.fontSize = MENU_FONT_SIZE * contentScale;

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
    menuState.contentScale = contentScale;
    menu_layout_update(menuState, height);
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
    bool quitSupported = app.capabilities_get().quitSupported;
    MenuTouch menuTouch;

    auto enter_selected_demo = [&]() {
        if (menuState.selectedIndex < demos.size()) {
            activeDemo = demos[menuState.selectedIndex].get();
            if (!activeDemo->init(ctx)) {
                activeDemo = nullptr;
            }
        }
    };

    auto go_back = [&]() {
        if (activeDemo) {
            activeDemo->quit(ctx);
            activeDemo = nullptr;
        } else {
            running = false;
        }
    };

    while (running) {
        while (app.event_poll(windowEvent)) {
            if (windowEvent.type == ryo::EVENT_PAD_ADDED) {
                if (!gamepad) {
                    gamepad = new ryo::Gamepad(app, windowEvent.data.pad.id);
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

            if (windowEvent.type == ryo::EVENT_WINDOW_RESIZED) {
                unsigned int newWidth = windowEvent.data.window.width;
                unsigned int newHeight = windowEvent.data.window.height;

                // During iPad rotation SDL can deliver the resize notification
                // before UIKit has finished laying out the window. Query the
                // window after the event so the menu and renderer use the same
                // logical size as the actual view.
                //window.size_get(newWidth, newHeight);

                if (newWidth != width || newHeight != height) {
                    width = newWidth;
                    height = newHeight;

                    ctx.screenWidth = width;
                    ctx.screenHeight = height;
                    menuCamera.orthographicSize = height;
                    menu_layout_update(menuState, height);

                    if (!renderer.resize(width, height))
                        printf("Failed to resize renderer: %s\n", ryo::error_get());
                }
            }

            // touch gestures are handled before the active demo, the back gesture has
            // to work while a demo is running and the other gestures only apply to
            // the menu itself
            MenuGesture gesture = MENU_GESTURE_NONE;
            switch (windowEvent.type) {
                case ryo::EVENT_TOUCH_BEGIN:
                    menu_touch_begin(menuTouch, windowEvent);
                    break;

                case ryo::EVENT_TOUCH_MOVE:
                    gesture = menu_touch_move(menuTouch, windowEvent);
                    break;

                case ryo::EVENT_TOUCH_END:
                    gesture = menu_touch_end(menuTouch, windowEvent);
                    break;

                default:
                    break;
            }

            switch (gesture) {
                case MENU_GESTURE_UP:
                    if (activeDemo == nullptr)
                        menu_move_up(menuState);
                    break;

                case MENU_GESTURE_DOWN:
                    if (activeDemo == nullptr)
                        menu_move_down(menuState);
                    break;

                case MENU_GESTURE_ENTER:
                    if (activeDemo == nullptr)
                        enter_selected_demo();
                    break;

                case MENU_GESTURE_BACK:
                    go_back();
                    break;

                case MENU_GESTURE_NONE:
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
                                enter_selected_demo();
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
                                running = !quitSupported;
                                break;

                            case ryo::KEY_RETURN:
                                enter_selected_demo();
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

        // a single tap is only turned into the enter gesture once no other tap
        // followed it within the cluster time
        menuTouch.time += dt;
        if (menu_touch_update(menuTouch) == MENU_GESTURE_ENTER)
            enter_selected_demo();

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
