cmake_minimum_required(VERSION 3.20)

project(ryolib
    VERSION 0.1.0
    DESCRIPTION "Cross-platform graphics, windowing, text, image and audio library"
    LANGUAGES C CXX
)

if(APPLE)
    enable_language(OBJC OBJCXX)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Options

if(NOT DEFINED RYO_DEBUG)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_ryolib_debug_default ON)
    else()
        set(_ryolib_debug_default OFF)
    endif()
endif()

option(RYO_DEBUG
    "Enable ryolib debug assertions (abort on invalid use)"
    ${_ryolib_debug_default}
)

option(RYO_SAFETY
    "Enable ryolib runtime safety checks (report an error and return instead of crashing)"
    ON
)

set(RYOLIB_OUTPUT_DIR "${CMAKE_BINARY_DIR}/dist" CACHE STRING
    "Directory for final application outputs (executables, bundles, VPKs)"
)

get_property(_ryolib_output_dir_type CACHE RYOLIB_OUTPUT_DIR PROPERTY TYPE)

if(_ryolib_output_dir_type STREQUAL "PATH"
   AND "${RYOLIB_OUTPUT_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    set(RYOLIB_OUTPUT_DIR "${CMAKE_BINARY_DIR}/dist" CACHE STRING
        "Directory for final application outputs (executables, bundles, VPKs)"
        FORCE
    )
endif()

# Library

set(RYOLIB_SOURCES
    src/math/matrix4.cpp
    src/math/vector3.cpp
    src/math/vector2.cpp

    src/debug/error/error.cpp
)

add_library(ryolib STATIC ${RYOLIB_SOURCES})
add_library(ryolib::ryolib ALIAS ryolib)

# PIC is unnecessary for a static library, and the Vita ELF loader cannot
# handle the R_ARM_BASE_PREL/GOT relocations that -fPIC generates.
if(NOT VITA)
    set_target_properties(ryolib PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
endif()

# Make sure consumers compile with at least the same C++ standard.
target_compile_features(ryolib PUBLIC cxx_std_17)

target_include_directories(ryolib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(RYO_DEBUG)
    target_compile_definitions(ryolib PRIVATE RYO_DEBUG)
endif()

if(RYO_SAFETY)
    target_compile_definitions(ryolib PRIVATE RYO_SAFETY)
endif()

if(APPLE)
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

    include("${VITASDK}/share/vita.cmake")

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
    message(FATAL_ERROR
        "[ryolib] Unsupported platform ${CMAKE_SYSTEM_NAME}.\n"
        "[ryolib] You're most likely building for Linux or BSD, none of which "
        "currently have a supported graphics backend.\n"
        "[ryolib] Currently supported graphics backends are: Metal, D3D11, GXM."
    )
endif()
