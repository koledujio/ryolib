cmake_minimum_required(VERSION 3.20)

project(ryolib_examples
    VERSION 0.1.0
    DESCRIPTION "Application for demonstrating examples of using ryolib"
    LANGUAGES C CXX
)

if(APPLE)
    enable_language(OBJC OBJCXX)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

# Add ryolib

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/.." ryolib_examples)

# App

include(CopyAssets)

set(EXAMPLE_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")

add_executable(RyolibExamplesApp
    src/main.cpp
)

target_link_libraries(RyolibExamplesApp PRIVATE ryolib::ryolib)

if(APPLE)
    set_target_properties(RyolibExamplesApp PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.ryolib.uitest"
        MACOSX_BUNDLE_BUNDLE_NAME "Ryolib Examples"
        MACOSX_BUNDLE_INFO_STRING "Application for demonstrating examples of using ryolib"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        MACOSX_BUNDLE_LONG_VERSION_STRING "1.0"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2026 adoravelk"
    )

    if(IOS)
        set_target_properties(RyolibExamplesApp PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ios/Info.plist.in"
            XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.ryolib.uitest"
        )
    else()
        set_target_properties(RyolibExamplesApp PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cmake/macos/Info.plist.in"
        )
    endif()
elseif(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set_property(TARGET RyolibExamplesApp PROPERTY WIN32_EXECUTABLE FALSE)
    else()
        set_property(TARGET RyolibExamplesApp PROPERTY WIN32_EXECUTABLE TRUE)
    endif()

    target_link_options(RyolibExamplesApp PRIVATE -static -static-libgcc -static-libstdc++)
endif()

ryolib_add_assets(RyolibExamplesApp ASSETS_DIR "${EXAMPLE_ASSETS_DIR}")

ryolib_package_app(RyolibExamplesApp
    VITA_APP_NAME "Ryolib Examples"
    VITA_TITLEID "KOLE00001"
    VITA_VERSION "01.00"
)
