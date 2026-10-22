cmake_minimum_required(VERSION 3.20)
project(ryolib C CXX OBJC OBJCXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(RYOLIB_CORE_SOURCES
    src/math/matrix4.cpp
    src/math/vector3.cpp
    src/math/vector2.cpp

    src/debug/error/error.cpp

    src/objcpp/RYObject.mm
    src/objcpp/RYArray.mm
    src/ui/RYView.mm
    src/ui/RYViewRoot.mm
)

add_library(ryolib STATIC ${RYOLIB_CORE_SOURCES})

if(NOT APPLE)
    target_link_libraries(ryolib PRIVATE objc)
endif()

target_include_directories(ryolib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

option(RYO_DEBUG "Enable ryolib debug checks" OFF)
option(RYO_SAFETY "Enable optional crash checks, may decrease performance, but in some cases can prevent crashing." ON)

if (RYO_DEBUG)
    target_compile_definitions(ryolib PRIVATE RYO_DEBUG)
endif()

if (RYO_SAFETY)
    target_compile_definitions(ryolib PRIVATE RYO_SAFETY)
endif()

if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(IOS)
        target_compile_definitions(ryolib PUBLIC RYO_APPLE_IOS=1)
    else()
        target_compile_definitions(ryolib PUBLIC RYO_APPLE_MAC=1)
    endif()

    include(graphics/MetalBackend)
    setup_metal_backend(ryolib)

    include(windowing/SDLBackend)
    setup_sdl_backend(ryolib)

    include(text/SDLBackend)
    setup_sdl_ttf_backend(ryolib)

    include(image/StbImageBackend)
    setup_stb_image_backend(ryolib)

    include(audio/MiniaudioBackend)
    setup_miniaudio_backend(ryolib)
elseif(WIN32)
    target_compile_definitions(ryolib PUBLIC RYO_WIN32=1)

    include(graphics/D3D11Backend)
    setup_d3d11_backend(ryolib)

    include(windowing/SDLBackend)
    setup_sdl_backend(ryolib)

    include(text/SDLBackend)
    setup_sdl_ttf_backend(ryolib)

    include(image/StbImageBackend)
    setup_stb_image_backend(ryolib)

    include(audio/MiniaudioBackend)
    setup_miniaudio_backend(ryolib)
elseif(VITA)
    target_compile_definitions(ryolib PUBLIC RYO_VITA=1)

    include (${VITASDK}/share/vita.cmake)

    target_link_libraries(ryolib PRIVATE
        pthread
        SceLibKernel_stub
    )

    include(graphics/GxmBackend)
    setup_gxm_backend(ryolib)

    include(windowing/SDLBackend)
    setup_sdl_backend(ryolib)

    include(text/SDLBackend)
    setup_sdl_ttf_backend(ryolib)

    include(image/StbImageBackend)
    setup_stb_image_backend(ryolib)

    include(audio/SDLMixerBackend)
    setup_sdl_mixer_backend(ryolib)
else()
    message(FATAL_ERROR "[ryolib] Unsupported platform ${CMAKE_SYSTEM_NAME}")
    message(FATAL_ERROR "[ryolib] You're most likely building for Linux or BSD, none of which currently have a supported graphics backend.")
    message(FATAL_ERROR "[ryolib] Currently supported graphics backends are: Metal, D3D11.")
endif()
