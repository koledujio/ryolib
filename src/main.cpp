#include <ryolib/ryolib.h>

#include <cstddef>
#include <cstdio>

int main() {
    ryo::AudioEngine audioEngine = ryo::audio_engine_create();
    if (!ryo::audio_engine_is_valid(audioEngine)) {
        printf("[APP] Failed to initialize audio engine!\n");
        return 0;
    }

    ryo::SoundDesc mikudayoDesc = {};
    mikudayoDesc.path = "assets/mikudayo.mp3";

    ryo::Sound mikudayo = ryo::sound_load(audioEngine, mikudayoDesc);
    if (!ryo::sound_is_valid(mikudayo)) {
        printf("[APP] Failed to load sound!\n");
        return 0;
    }

    ryo::SoundDesc floatplayDesc = {};
    floatplayDesc.path = "assets/float play.flac";
    floatplayDesc.loadType = ryo::SOUND_LOAD_TYPE_STREAM;
    floatplayDesc.config.volume = 0.5f;
    floatplayDesc.config.looping = true;

    ryo::Sound floatplay = ryo::sound_load(audioEngine, floatplayDesc);
    if (!ryo::sound_is_valid(floatplay)) {
        printf("[APP] Failed to load sound!\n");
        return 0;
    }

    ryo::Window window = ryo::window_create("GFX Test", 1280, 720);
    if (!ryo::window_is_valid(window)) {
        printf("[APP] Failed to create gfx window!\n");
        return 0;
    }

    ryo::RendererConfig config = {};
    config.nativeWindowHandle = ryo::window_get_nwh(window);
    config.width = 1280;
    config.height = 720;

    ryo::Renderer renderer = ryo::renderer_create(config);
    if (!ryo::renderer_is_valid(renderer)) {
        printf("WARNING: Renderer invalid. Renderer's INDEX: %d.\n", renderer.index());
    }

    uint8_t checkerboardPixels[16] = {
        255, 0, 255, 255,
        0, 0, 0, 255,
        0, 0, 0, 255,
        255, 0, 255, 255
    };  
    ryo::Texture myTexture = ryo::texture_create(renderer, checkerboardPixels, 2, 2);

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

    ryo::Buffer vbo = ryo::buffer_create(renderer, cubeVertices, sizeof(ryo::PositionUVVertex) * 8, ryo::BUFFER_TYPE_VERTEX);
    ryo::Buffer ibo = ryo::buffer_create(renderer, cubeIndices, sizeof(uint32_t) * 36, ryo::BUFFER_TYPE_INDEX);

    ryo::FontConfig fontConfig;
    fontConfig.path = "assets/lato-400-normal.ttf";
    fontConfig.fontSize = 32.0f;

    ryo::Font font = ryo::font_create(fontConfig);
    if (!ryo::font_is_valid(font)) {
        printf("[APP] Invalid font!\n");
        return 0;
    }

    ryo::FontConfig jpFontConfig;
    jpFontConfig.path = "assets/notosansjp-regular.ttf";
    jpFontConfig.fontSize = 27.0f;

    ryo::Font jpFont = ryo::font_create(jpFontConfig);
    if (!ryo::font_is_valid(jpFont)) {
        printf("[APP] Invalid japanese font!\n");
        return 0;
    }

    ryo::font_add_fallback(font, jpFont);

    ryo::Color textColor = {0.0f, 0.0f, 0.0f, 1.0f};
    void* textPixels;
    int textWidth;
    int textHeight;
    if (!ryo::text_render_line(font, "Hello, World! こんにちは！", textColor, textWidth, textHeight, textPixels)) {
        printf("[APP] Text rendering failed!\n");
        return 0;
    }

    ryo::Texture textTexture = ryo::texture_create(renderer, textPixels, textWidth, textHeight);
    if (!ryo::texture_is_valid(textTexture)) {
        printf("[APP] Text texture creation failed!\n");
        return 0;
    }

    ryo::text_free_pixels(textPixels);

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

    ryo::Pipeline texBlend = ryo::pipeline_create(renderer, texBlendDesc);
    if (!ryo::pipeline_is_valid(texBlend)) {
        printf("[GFX] Failed to create texture blended pipeline!\n");
        return 0;
    }

    int mikuWidth, mikuHeight;
    ryo::Texture hatsuneMikuTexture = ryo::image_load_texture(renderer, "assets/hatsune miku.png", mikuWidth, mikuHeight);
    if (!ryo::texture_is_valid(hatsuneMikuTexture)) {
        return 0;
    }

    int tetoHeight, tetoWidth;
    ryo::Texture tetoTexture = ryo::image_load_texture(renderer, "assets/kasane teto.png", tetoWidth, tetoHeight);
    if (!ryo::texture_is_valid(tetoTexture)) {
        return 0;
    }
    int endTetoHeight = 720; // height of the window
    int endTetoWidth = ((float)tetoWidth / (float)tetoHeight) * 720; // height * aspect ratio of teto

    ryo::RenderTarget tetoTarget = ryo::render_target_create(renderer, tetoWidth, tetoHeight);
    if (!ryo::render_target_is_valid(tetoTarget)) {
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
    ryo::Gamepad gamepad; 
    ryo::GamepadId gamepadId;
    bool buttonPressed = false;
    bool floatplayPlaying = false;
    float hatsuneFloatAmount = 0.0f;
    bool rtrigger = false;
    bool ltrigger = false;
    bool tetoDrawn = false;

    ryo::Event event;
    while (!shouldClose) {
        ryo::window_poll_event(window, event);
        switch (event.type) {
            case ryo::EVENT_QUIT:
                shouldClose = true;
                break;
            
            case ryo::EVENT_PAD_ADDED:
                if (gamepadConnected) continue;
                gamepadId = event.data.pad.id;
                printf("Gamepad with id %d detected!\n", gamepadId);
                gamepad = ryo::gamepad_open(gamepadId);
                if (ryo::gamepad_is_valid(gamepad)) {
                    gamepadConnected = true;
                    printf("Gamepad with id %d connected!\n", gamepadId);
                    printf("%s Connected!\n", ryo::gamepad_get_name(gamepad));
                }
                break;
            
            case ryo::EVENT_PAD_REMOVED:
                if (!gamepadConnected) continue;
                if (event.data.pad.id != gamepadId) continue;
                if (!ryo::gamepad_is_valid(gamepad)) continue;
                printf("Gamepad with id %d removed!\n", gamepadId);
                ryo::gamepad_close(gamepad);
                gamepadConnected = false;
                break;
            
            case ryo::EVENT_PAD_BUTTON_DOWN:
                if (!gamepadConnected) continue;
                if (event.data.pad.id != gamepadId) continue;
                if (!ryo::gamepad_is_valid(gamepad)) continue;
                if (event.data.pad.button == ryo::PAD_BUTTON_DOWN) {
                    printf("Down button pressed\n");
                    buttonPressed = true;
                }
                if (event.data.pad.button == ryo::PAD_BUTTON_RIGHT) {
                    printf("MIKUDAYO\n");
                    ryo::sound_play(mikudayo);
                    hatsuneFloatAmount = 250.0f;
                }
                if (event.data.pad.button == ryo::PAD_BUTTON_UP) {
                    if (floatplayPlaying) {
                        ryo::sound_pause(floatplay);
                    } else {
                        ryo::sound_play(floatplay);
                    }
                    floatplayPlaying = !floatplayPlaying;
                }
                break;
            
            case ryo::EVENT_PAD_BUTTON_UP:
                if (!gamepadConnected) continue;
                if (event.data.pad.id != gamepadId) continue;
                if (!ryo::gamepad_is_valid(gamepad)) continue;
                if (event.data.pad.button == ryo::PAD_BUTTON_DOWN) {
                    printf("PAD_PHYS_RPAD_DOWN unpressed\n");
                    buttonPressed = false;
                }
                break;
            
            case ryo::EVENT_PAD_AXIS_MOTION:
                if (!gamepadConnected) continue;
                if (event.data.pad.id != gamepadId) continue;
                if (!ryo::gamepad_is_valid(gamepad)) continue;
                if (event.data.pad.axis == ryo::PAD_AXIS_TRIGGER_RIGHT) {
                    if (event.data.pad.axisValue > 0.5f && !rtrigger) {
                        ryo::gamepad_set_rumble(gamepad, 65535, 0, 500);
                        rtrigger = true;
                    } else if (event.data.pad.axisValue <= 0.5f && rtrigger) {
                        rtrigger = false;
                    }
                } else if (event.data.pad.axis == ryo::PAD_AXIS_TRIGGER_LEFT) {
                    if (event.data.pad.axisValue > 0.5f && !ltrigger) {
                        ryo::gamepad_set_rumble(gamepad, 0, 65535, 500);
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

        rotationAngle += 0.015f;

        ryo::frame_begin(renderer);

        if (!tetoDrawn) {
            ryo::Color w = {1.0f, 1.0f, 1.0f, 1.0f};
            ryo::Color tetoClear = {0.5f, 0.5f, 1.0f, 1.0f};

            ryo::draw_pass_begin(renderer, &tetoTarget, &tetoClear);

            ryo::Matrix4 tetoScale = ryo::matrix4_scaling((float)tetoWidth, (float)tetoHeight, 1.0f);
            ryo::draw_rect_texture(renderer, tetoTexture, ryo::SAMPLER_TYPE_LINEAR, w, tetoScale, &tetoTargetSettings);

            ryo::draw_pass_end(renderer);

            tetoDrawn = true;
        }

        ryo::Matrix4 rot = ryo::matrix4_rotation(rotationAngle, rotationAngle * 0.7f, 0.0f);
        ryo::Matrix4 scale = ryo::matrix4_scaling(1.0f, 1.0f, 1.0f);
        ryo::Matrix4 model = ryo::matrix4_multiply(rot, scale);

        ryo::Matrix4 model2 = ryo::matrix4_translation(-0.5f, 0.5f, 0.0f);

        ryo::Matrix4 scale3 = ryo::matrix4_scaling(5.0f, 5.0f, 1.0f);
        ryo::Matrix4 translation4 = ryo::matrix4_translation(0.0f, 0.0f, -1.0f);
        ryo::Matrix4 model3 = ryo::matrix4_multiply(scale3, translation4);

        ryo::Matrix4 model4 = ryo::matrix4_translation(-0.5f, -0.5f, 0.0f);

        ryo::Matrix4 textScale = ryo::matrix4_scaling((float)textWidth, (float)textHeight, 1.0f);
        float xPos = -640.0f + (textWidth / 2.0f);
        float yPos = 360.0f - (textHeight / 2.0f);
        ryo::Matrix4 textTranslate = ryo::matrix4_translation(xPos, yPos, 0.0f);
        ryo::Matrix4 textModel = ryo::matrix4_multiply(textTranslate, textScale);

        ryo::Matrix4 mikuScale = ryo::matrix4_scaling((float)mikuWidth, (float)mikuHeight, 1.0f);
        float mikuXPos = -640.0f + (mikuWidth / 2.0f);
        float mikuYPos = -360.0f + (mikuHeight / 2.0f) + hatsuneFloatAmount;
        ryo::Matrix4 mikuTranslate = ryo::matrix4_translation(mikuXPos, mikuYPos, 0.0f);
        ryo::Matrix4 mikuModel = ryo::matrix4_multiply(mikuTranslate, mikuScale);

        ryo::Matrix4 tetoScale = ryo::matrix4_scaling((float)endTetoWidth, (float)endTetoHeight, 1.0f);
        float tetoXPos = 640.0f - (endTetoWidth / 2.0f);
        float tetoYPos = 360.0f - (endTetoHeight / 2.0f);
        ryo::Matrix4 tetoTranslate = ryo::matrix4_translation(tetoXPos, tetoYPos, 0.0f);
        ryo::Matrix4 tetoModel = ryo::matrix4_multiply(tetoTranslate, tetoScale);
        
        ryo::Color white = {1.0f, 1.0f, 1.0f, 1.0f};
        ryo::Color red = {1.0, 0.0f, 0.0f, 1.0f};
        ryo::Color blue = {0.0, 0.0f, 1.0f, 1.0f};

        ryo::draw_pass_begin(renderer, nullptr, &white);

        ryo::draw_rect_color(renderer, red, model2, &settings);
        ryo::draw_mesh_texture(renderer, vbo, ibo, 36, myTexture,buttonPressed ? ryo::SAMPLER_TYPE_LINEAR : ryo::SAMPLER_TYPE_NEAREST, white, model, &settings);
        ryo::draw_rect_color(renderer, blue, model3, &settings);
        ryo::draw_rect_texture(renderer, myTexture, ryo::SAMPLER_TYPE_NEAREST, white, model4, &settings);

        ryo::draw_rect_texture(renderer, textTexture, ryo::SAMPLER_TYPE_LINEAR, white, textModel, &settings2d);
        ryo::draw_rect_texture(renderer, hatsuneMikuTexture, ryo::SAMPLER_TYPE_LINEAR, white, mikuModel, &settings2d);
        ryo::draw_rect_texture(renderer, ryo::render_target_get_color_texture(tetoTarget), ryo::SAMPLER_TYPE_LINEAR, white, tetoModel, &settings2d);

        ryo::draw_pass_end(renderer);

        ryo::frame_end(renderer);

        if (hatsuneFloatAmount > 0.0f) {
            hatsuneFloatAmount -= ryo::frame_get_delta_time(renderer) * 500;
            if (hatsuneFloatAmount < 0.0f) {
                hatsuneFloatAmount = 0.0f;
            }
        }
    }

    ryo::pipeline_destroy(texBlend);
    ryo::texture_destroy(textTexture);
    ryo::font_destroy(font);
    ryo::font_destroy(jpFont);
    ryo::buffer_destroy(vbo);
    ryo::buffer_destroy(ibo);
    ryo::texture_destroy(myTexture);
    ryo::texture_destroy(tetoTexture);
    ryo::render_target_destroy(tetoTarget);
    ryo::renderer_destroy(renderer);
    ryo::window_destroy(window);
    ryo::sound_destroy(floatplay);
    ryo::sound_destroy(mikudayo);
    ryo::audio_engine_destroy(audioEngine);

    return 0;
}