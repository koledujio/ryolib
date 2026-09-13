#include <ryolib/ryolib.h>

#include <cstddef>
#include <cstdio>

int main_vita() {
    FILE* log = fopen("ux0:data/ryolib.log", "a");

    ryo::Window window("Vita test", 960, 544);
    if (!window.valid()) {
        fprintf(log, "Failed to create window! %s\n", ryo::error_get());
        fflush(log);
        return 0;
    }

    fprintf(log, "Window created.\n");
    fflush(log);

    ryo::AudioEngine audioEngine;
    if (!audioEngine.valid()) {
        fprintf(log, "Failed to create audio enigne! %s\n", ryo::error_get());
        fflush(log);
        return 0;
    }

    fprintf(log, "Audio engine created.\n");
    fflush(log);

    ryo::SoundDesc soundDesc = {};
    soundDesc.path = "assets/mikudayo.mp3";

    ryo::Sound sound(audioEngine, soundDesc);
    if (!sound.valid()) {
        fprintf(log, "Failed to load sound! %s\n", ryo::error_get());
        fflush(log);
        return 0;
    }

    fprintf(log, "Sound created.\n");
    fflush(log);

    if (!sound.play()) {
        fprintf(log, "Failed to play sound! %s\n", ryo::error_get());
        fflush(log);
        return 0;
    }

    fprintf(log, "Sound played.\n");
    fflush(log);

    ryo::RendererConfig config = {};
    config.nativeWindowHandle = nullptr;
    config.width = 960;
    config.height = 544;

    ryo::Renderer renderer(config);
    if (!renderer.valid()) {
        fprintf(log, "Failed to create renderer! %s\n", ryo::error_get());
        fflush(log);
        return 0;
    }

    fprintf(log, "Renderer created.\n");
    fflush(log);

    int mikuWidth, mikuHeight;
    ryo::Texture* hatsuneMikuTexture = ryo::image_load_texture(renderer, "assets/hatsune-miku.png", mikuWidth, mikuHeight);
    if (hatsuneMikuTexture == nullptr || !hatsuneMikuTexture->valid()) {
        fprintf(log, "Failed to load texture! %s\n", ryo::error_get());
        fflush(log);
        return 0;
    }

    fprintf(log, "Texture loaded and created.\n");
    fflush(log);

    ryo::Camera camera2d = {};
    camera2d.projectionType = ryo::CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC;
    camera2d.orthographicSize = 720.0f;
    camera2d.nearPlane = -1.0f;
    camera2d.farPlane = 1.0f;
    camera2d.position = {0.0f, 0.0f, 1.0f};
    camera2d.target = {0.0f, 0.0f, 0.0f};
    camera2d.up = {0.0f, 1.0f, 0.0f};

    ryo::DrawSettings settings = {};
    settings.camera = &camera2d;
    settings.cullMode = ryo::CULL_MODE_NONE;
    settings.fillMode = ryo::FILL_MODE_SOLID;
    settings.pipelineOverride = nullptr;
    settings.windingOrder = ryo::WINDING_ORDER_COUNTER_CLOCKWISE;

    ryo::Matrix4 scaling = ryo::Matrix4::scaling(200.0f, 200.0f, 1.0f);

    ryo::Matrix4 miku = ryo::Matrix4::scaling(400.0f, 400.0f, 1.0f) * ryo::Matrix4::translation(-200.0f, 0.0f, 0.0f);
    
    ryo::Gamepad* gamepad = nullptr;
    int gamepadId = -1;

    ryo::Color clearColor = {1.0f, 1.0f, 1.0f, 1.0f};

    ryo::Event evt;

    bool shouldQuit = false;
    while (!shouldQuit) {
        while(window.event_poll(evt)) {
            switch(evt.type) {
                case ryo::EVENT_QUIT:
                    shouldQuit = true;
                    break;

                case ryo::EVENT_PAD_ADDED:
                    gamepad = new ryo::Gamepad(evt.data.pad.id);
                    if (!gamepad->valid()) {
                        delete gamepad;
                        gamepad = nullptr;
                    } else {
                        gamepadId = evt.data.pad.id;
                    }
                    break;
                
                case ryo::EVENT_PAD_REMOVED:
                    if (gamepadId == evt.data.pad.id) {
                        if (gamepad)
                            delete gamepad;
                        
                        gamepad = nullptr;
                        gamepadId = -1;
                    }
                    break;
                
                case ryo::EVENT_PAD_BUTTON_DOWN:
                    if (gamepadId == evt.data.pad.id) {
                        if (evt.data.pad.button == ryo::PAD_BUTTON_RIGHT) {
                            shouldQuit = true;
                        }
                    }
                    break;

                default:
                    break;
            }
        }

        renderer.frame_begin();

        renderer.draw_pass_begin(nullptr, &clearColor);

        if (!renderer.draw_rect_color(clearColor, scaling, &settings)) {
            fprintf(log, "Failed to draw colored rect: %s\n", ryo::error_get());
            fflush(log);
        }

        if (!renderer.draw_rect_texture(*hatsuneMikuTexture, ryo::SAMPLER_TYPE_LINEAR, clearColor, scaling, &settings)) {
            fprintf(log, "Failed to draw textured rect: %s\n", ryo::error_get());
            fflush(log);
        }

        renderer.draw_pass_end();

        renderer.frame_end();
    }

    fclose(log);

    if (gamepad)
        delete gamepad;

    return 0;
}

int main() {
// #if __vita__
//     return main_vita();
// #endif

    #if __vita__
    ryo::Window window("GFX Test", 960, 544);
    #else
    ryo::Window window("GFX Test", 1280, 720);
    #endif
    if (!window.valid()) {
        printf("[APP] Failed to create gfx window!\n");
        return 0;
    }

    ryo::AudioEngine audioEngine;
    if (!audioEngine.valid()) {
        printf("[APP] Failed to initialize audio engine!\n");
        return 0;
    }

    ryo::SoundDesc mikudayoDesc = {};
    mikudayoDesc.path = "assets/mikudayo.mp3";

    ryo::Sound mikudayo(audioEngine, mikudayoDesc);
    if (!mikudayo.valid()) {
        printf("[APP] Failed to load sound!\n");
        return 0;
    }

    ryo::SoundDesc floatplayDesc = {};
    floatplayDesc.path = "assets/float-play.flac";
    floatplayDesc.loadType = ryo::SOUND_LOAD_TYPE_STREAM;
    floatplayDesc.config.volume = 0.5f;
    floatplayDesc.config.looping = true;

    ryo::Sound floatplay(audioEngine, floatplayDesc);
    if (!floatplay.valid()) {
        printf("[APP] Failed to load sound!\n");
        return 0;
    }

    void* nwh = window.nwh_get();

    // if (nwh == nullptr) {
    //     printf("[APP] Failed to get window native handle\n");
    //     return 0;
    // }

    ryo::RendererConfig config = {};
    config.nativeWindowHandle = nwh;
    #if __vita__
    config.width = 960;
    config.height = 544;
    #else
    config.width = 1280;
    config.height = 720;
    #endif

    ryo::Renderer renderer(config);
    if (!renderer.valid()) {
        printf("[APP] Failed to create renderer!\n");
        return 0;
    }

    uint8_t checkerboardPixels[16] = {
        255, 0, 255, 255,
        0, 0, 0, 255,
        0, 0, 0, 255,
        255, 0, 255, 255
    };  
    ryo::Texture myTexture(renderer, checkerboardPixels, 2, 2);

    ryo::PositionUVVertex cubeVertices[] = {
        { {-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f} },
        { {0.5f, -0.5f, 0.5f}, {1.0f, 1.0f} },
        { {0.5f, 0.5f, 0.5f}, {1.0f, 0.0f} },
        { {-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f} },

        { {-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f} },
        { {0.5f, -0.5f, -0.5f}, {1.0f, 1.0f} },
        { {0.5f, 0.5f, -0.5f}, {1.0f, 0.0f} },
        { {-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f} },
    };

    uint32_t cubeIndices[] = {
        0, 1, 2,  2, 3, 0,
        1, 5, 6,  6, 2, 1,
        5, 4, 7,  7, 6, 5,
        4, 0, 3,  3, 7, 4,
        3, 2, 6,  6, 7, 3,
        4, 5, 1,  1, 0, 4
    };

    ryo::Buffer vbo(renderer, cubeVertices, sizeof(ryo::PositionUVVertex) * 8, ryo::BUFFER_TYPE_VERTEX);
    ryo::Buffer ibo(renderer, cubeIndices, sizeof(uint32_t) * 36, ryo::BUFFER_TYPE_INDEX);

    if (!vbo.valid()) {
        printf("[APP] Invalid vertex buffer object\n");
        return 0;
    }

    if (!ibo.valid()) {
        printf("[APP] Invalid index buffer object!\n");
        return 0;
    }

    ryo::FontConfig fontConfig;
    fontConfig.path = "assets/lato-400-normal.ttf";
    fontConfig.fontSize = 32.0f;

    ryo::Font font(fontConfig);
    if (!font.valid()) {
        printf("[APP] Invalid font!\n");
        return 0;
    }

    ryo::FontConfig jpFontConfig;
    jpFontConfig.path = "assets/notosansjp-regular.ttf";
    jpFontConfig.fontSize = 27.0f;

    ryo::Font jpFont(jpFontConfig);
    if (!jpFont.valid()) {
        printf("[APP] Invalid japanese font!\n");
        return 0;
    }

    font.fallback_add(jpFont);

    ryo::Color textColor = {0.0f, 0.0f, 0.0f, 1.0f};
    void* textPixels;
    int textWidth;
    int textHeight;
    if (!font.pixels_render_line("Hello, World! こんにちは！", textColor, textWidth, textHeight, textPixels)) {
        printf("[APP] Text rendering failed!\n");
        return 0;
    }

    ryo::Texture textTexture(renderer, textPixels, textWidth, textHeight);
    if (!textTexture.valid()) {
        printf("[APP] Text texture creation failed!\n");
        return 0;
    }

    font.pixels_free(textPixels);

    ryo::Camera camera3d = {};
    camera3d.projectionType = ryo::CAMERA_PROJECTION_TYPE_PERSPECTIVE;
    camera3d.fovY = 60.0f;
    camera3d.nearPlane = 0.1f;
    camera3d.farPlane = 100.0f;
    camera3d.position = { 0.0f, 0.0f, 3.0f};
    camera3d.target = { 0.0f, 0.0f, 0.0f };
    camera3d.up = { 0.0f, 1.0f, 0.0f };

    ryo::DrawSettings settings = {};
    settings.camera = &camera3d;
    settings.fillMode = ryo::FILL_MODE_SOLID;

    ryo::Camera camera2d = {};
    camera2d.projectionType = ryo::CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC;
    camera2d.orthographicSize = 720.0f;
    camera2d.nearPlane = -1.0f;
    camera2d.farPlane = 1.0f;
    camera2d.position = {0.0f, 0.0f, 1.0f};
    camera2d.target = {0.0f, 0.0f, 0.0f};
    camera2d.up = {0.0f, 1.0f, 0.0f};

    ryo::PipelineDesc texBlendDesc = {};
    texBlendDesc.shader = ryo::BUILTIN_SHADER_TEXTURE;
    texBlendDesc.blendMode = ryo::BLEND_MODE_ALPHA;
    texBlendDesc.vertexLayout = ryo::VERTEX_LAYOUT_POSITION_UV;

    ryo::Pipeline texBlend(renderer, texBlendDesc);
    if (!texBlend.valid()) {
        printf("[GFX] Failed to create texture blended pipeline!\n");
        return 0;
    }

    int mikuWidth, mikuHeight;
    ryo::Texture* hatsuneMikuTexture = ryo::image_load_texture(renderer, "assets/hatsune-miku.png", mikuWidth, mikuHeight);
    if (hatsuneMikuTexture == nullptr || !hatsuneMikuTexture->valid()) {
        return 0;
    }


    int tetoHeight, tetoWidth;
    ryo::Texture* tetoTexture = ryo::image_load_texture(renderer, "assets/kasane-teto.png", tetoWidth, tetoHeight);
    if (tetoTexture == nullptr || !tetoTexture->valid()) {
        FILE* log = fopen("ux0:data/ryolib-image.log", "a");
        fprintf(log, "%s", ryo::error_get());
        fflush(log);
        fclose(log);
        return 0;
    }
    int endTetoHeight = 720; // height of the window
    int endTetoWidth = ((float)tetoWidth / (float)tetoHeight) * 720; // height * aspect ratio of teto

    ryo::RenderTarget tetoTarget(renderer, tetoWidth, tetoHeight);
    if (!tetoTarget.valid()) {
        return 0;
    }

    ryo::Camera tetoCam = {};
    tetoCam.projectionType = ryo::CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC;
    tetoCam.orthographicSize = (float)tetoHeight;
    tetoCam.nearPlane = -1.0f;
    tetoCam.farPlane = 1.0f;
    tetoCam.position = {0.0f, 0.0f, 1.0f};
    tetoCam.target = {0.0f, 0.0f, 0.0f};
    tetoCam.up = {0.0f, 1.0f, 0.0f};
    
    ryo::DrawSettings tetoTargetSettings = {};
    tetoTargetSettings.camera = &tetoCam;
    tetoTargetSettings.fillMode = ryo::FILL_MODE_SOLID;

    ryo::DrawSettings settings2d = {};
    settings2d.camera = &camera2d;
    settings2d.fillMode = ryo::FILL_MODE_SOLID;
    settings2d.pipelineOverride = &texBlend;

    float rotationAngle = 0.0f;

    bool shouldClose = false;
    bool gamepadConnected = false;
    ryo::Gamepad* gamepad = nullptr;
    int gamepadId;
    bool buttonPressed = false;
    bool floatplayPlaying = false;
    float hatsuneFloatAmount = 0.0f;
    bool rtrigger = false;
    bool ltrigger = false;
    bool tetoDrawn = false;

    ryo::Event event;
    while (!shouldClose) {
        while (window.event_poll(event)) {
            switch (event.type) {
                case ryo::EVENT_QUIT:
                    shouldClose = true;
                    break;
                
                case ryo::EVENT_PAD_ADDED:
                    if (gamepadConnected) continue;
                    gamepadId = event.data.pad.id;
                    printf("Gamepad with id %d detected!\n", gamepadId);
                    gamepad = new ryo::Gamepad(gamepadId);
                    if (gamepad->valid()) {
                        gamepadConnected = true;
                        printf("Gamepad with id %d connected!\n", gamepadId);
                        printf("%s Connected!\n", gamepad->name_get());
                    }
                    break;
                
                case ryo::EVENT_PAD_REMOVED:
                    if (!gamepadConnected) continue;
                    if (event.data.pad.id != gamepadId) continue;
                    if (!gamepad->valid()) continue;
                    printf("Gamepad with id %d removed!\n", gamepadId);
                    delete gamepad;
                    gamepad = nullptr;
                    gamepadConnected = false;
                    break;
                
                case ryo::EVENT_PAD_BUTTON_DOWN:
                    if (!gamepadConnected) continue;
                    if (event.data.pad.id != gamepadId) continue;
                    if (!gamepad->valid()) continue;
                    if (event.data.pad.button == ryo::PAD_BUTTON_DOWN) {
                        printf("Down button pressed\n");
                        buttonPressed = true;
                    }
                    if (event.data.pad.button == ryo::PAD_BUTTON_RIGHT) {
                        printf("MIKUDAYO\n");
                        mikudayo.play();
                        hatsuneFloatAmount = 250.0f;
                    }
                    if (event.data.pad.button == ryo::PAD_BUTTON_UP) {
                        if (floatplayPlaying) {
                            floatplay.pause();
                        } else {
                            floatplay.play();
                        }
                        floatplayPlaying = !floatplayPlaying;
                    }
                    break;
                
                case ryo::EVENT_PAD_BUTTON_UP:
                    if (!gamepadConnected) continue;
                    if (event.data.pad.id != gamepadId) continue;
                    if (!gamepad->valid()) continue;
                    if (event.data.pad.button == ryo::PAD_BUTTON_DOWN) {
                        printf("PAD_PHYS_RPAD_DOWN unpressed\n");
                        buttonPressed = false;
                    }
                    break;
                
                case ryo::EVENT_PAD_AXIS_MOTION:
                    if (!gamepadConnected) continue;
                    if (event.data.pad.id != gamepadId) continue;
                    if (!gamepad->valid()) continue;
                    if (event.data.pad.axis == ryo::PAD_AXIS_TRIGGER_RIGHT) {
                        if (event.data.pad.axisValue > 0.5f && !rtrigger) {
                            gamepad->rumble_set(65535, 0, 500);
                            rtrigger = true;
                        } else if (event.data.pad.axisValue <= 0.5f && rtrigger) {
                            rtrigger = false;
                        }
                    } else if (event.data.pad.axis == ryo::PAD_AXIS_TRIGGER_LEFT) {
                        if (event.data.pad.axisValue > 0.5f && !ltrigger) {
                            gamepad->rumble_set(0, 65535, 500);
                            ltrigger = true;
                        } else if (event.data.pad.axisValue <= 0.5f && ltrigger) {
                            ltrigger = false;
                        }
                    }
                    break;
                
                case ryo::EVENT_KEY_DOWN:
                    if (event.data.key.code == ryo::KEY_SPACE && !event.data.key.isRepeat) {
                        printf("SPACE pressed!\n");
                    }
                    break;
                
                case ryo::EVENT_KEY_UP:
                    if (event.data.key.code == ryo::KEY_SPACE && !event.data.key.isRepeat) {
                        printf("SPACE unpressed!\n");
                    }
                    break;

                default:
                    break;
            }
        }

        rotationAngle += 1.5f * renderer.frame_time_get();

        renderer.frame_begin();

        if (!tetoDrawn) {
            ryo::Color w = {1.0f, 1.0f, 1.0f, 1.0f};
            ryo::Color tetoClear = {0.5f, 0.5f, 1.0f, 0.0f};

            renderer.draw_pass_begin(&tetoTarget, &tetoClear);

            ryo::Matrix4 tetoScale = ryo::Matrix4::scaling((float)tetoWidth, (float)tetoHeight, 1.0f);
            renderer.draw_rect_texture(*tetoTexture, ryo::SAMPLER_TYPE_LINEAR, w, tetoScale, &tetoTargetSettings);

            renderer.draw_pass_end();

            tetoDrawn = true;
        }

        ryo::Matrix4 rot = ryo::Matrix4::rotation(rotationAngle, rotationAngle * 0.7f, 0.0f);
        ryo::Matrix4 scale = ryo::Matrix4::scaling(1.0f, 1.0f, 1.0f);
        ryo::Matrix4 model = rot * scale;

        ryo::Matrix4 model2 = ryo::Matrix4::translation(-0.5f, 0.5f, 0.0f);

        ryo::Matrix4 scale3 = ryo::Matrix4::scaling(5.0f, 5.0f, 1.0f);
        ryo::Matrix4 translation4 = ryo::Matrix4::translation(0.0f, 0.0f, -1.0f);
        ryo::Matrix4 model3 = scale3 * translation4;

        ryo::Matrix4 model4 = ryo::Matrix4::translation(-0.5f, -0.5f, 0.0f);

        ryo::Matrix4 textScale = ryo::Matrix4::scaling((float)textWidth, (float)textHeight, 1.0f);
        float xPos = -640.0f + (textWidth / 2.0f);
        float yPos = 360.0f - (textHeight / 2.0f);
        ryo::Matrix4 textTranslate = ryo::Matrix4::translation(xPos, yPos, 0.0f);
        ryo::Matrix4 textModel = textTranslate * textScale;

        ryo::Matrix4 mikuScale = ryo::Matrix4::scaling((float)mikuWidth, (float)mikuHeight, 1.0f);
        float mikuXPos = -640.0f + (mikuWidth / 2.0f);
        float mikuYPos = -360.0f + (mikuHeight / 2.0f) + hatsuneFloatAmount;
        ryo::Matrix4 mikuTranslate = ryo::Matrix4::translation(mikuXPos, mikuYPos, 0.0f);
        ryo::Matrix4 mikuModel = mikuTranslate * mikuScale;

        ryo::Matrix4 tetoScale = ryo::Matrix4::scaling((float)endTetoWidth, (float)endTetoHeight, 1.0f);
        float tetoXPos = 640.0f - (endTetoWidth / 2.0f);
        float tetoYPos = 360.0f - (endTetoHeight / 2.0f);
        ryo::Matrix4 tetoTranslate = ryo::Matrix4::translation(tetoXPos, tetoYPos, 0.0f);
        ryo::Matrix4 tetoModel = tetoTranslate * tetoScale;
        
        ryo::Color white = {1.0f, 1.0f, 1.0f, 1.0f};
        ryo::Color red = {1.0, 0.0f, 0.0f, 1.0f};
        ryo::Color blue = {0.0, 0.0f, 1.0f, 1.0f};

        renderer.draw_pass_begin(nullptr, &white);

        renderer.draw_rect_color(red, model2, &settings);
        renderer.draw_mesh_texture(vbo, ibo, 36, myTexture,buttonPressed ? ryo::SAMPLER_TYPE_LINEAR : ryo::SAMPLER_TYPE_NEAREST, white, model, &settings);
        renderer.draw_rect_color(blue, model3, &settings);
        renderer.draw_rect_texture(myTexture, ryo::SAMPLER_TYPE_NEAREST, white, model4, &settings);

        renderer.draw_rect_texture(textTexture, ryo::SAMPLER_TYPE_LINEAR, white, textModel, &settings2d);
        renderer.draw_rect_texture(*hatsuneMikuTexture, ryo::SAMPLER_TYPE_LINEAR, white, mikuModel, &settings2d);
        renderer.draw_rect_texture(*tetoTarget.texture_color_get(), ryo::SAMPLER_TYPE_LINEAR, white, tetoModel, &settings2d);

        renderer.draw_pass_end();

        renderer.frame_end();

        if (hatsuneFloatAmount > 0.0f) {
            hatsuneFloatAmount -= renderer.frame_time_get() * 500;
            if (hatsuneFloatAmount < 0.0f) {
                hatsuneFloatAmount = 0.0f;
            }
        }
    }

    if (gamepad)
        delete gamepad;

    if (hatsuneMikuTexture)
        delete hatsuneMikuTexture;

    if (tetoTexture)
        delete tetoTexture;

    return 0;
}