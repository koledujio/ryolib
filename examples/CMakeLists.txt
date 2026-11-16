# Example application built with ryolib.
#
# It is only added by the top-level CMakeLists.txt when RYOLIB_BUILD_EXAMPLES
# is ON, which is the default when ryolib itself is the top-level project.

include(CopyAssets)

set(EXAMPLE_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")

add_executable(TestApp
    src/main.cpp
)

target_link_libraries(TestApp PRIVATE ryolib::ryolib)

if(IOS)
    set_target_properties(TestApp PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.ryolib.uitest"
        MACOSX_BUNDLE_BUNDLE_NAME "RyolibTest"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ios/Info.plist.in"
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.ryolib.uitest"
    )
elseif(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set_property(TARGET TestApp PROPERTY WIN32_EXECUTABLE FALSE)
    else()
        set_property(TARGET TestApp PROPERTY WIN32_EXECUTABLE TRUE)
    endif()

    target_link_options(TestApp PRIVATE -static -static-libgcc -static-libstdc++)
elseif(VITA)
    include("${VITASDK}/share/vita.cmake")

    set(VITA_APP_NAME "Ryolib UI Test")
    set(VITA_TITLEID "KOLE00001")
    set(VITA_VERSION "01.00")

    vita_create_self(ui_test.self TestApp)

    file(GLOB_RECURSE VITA_ASSETS
        RELATIVE "${EXAMPLE_ASSETS_DIR}"
        CONFIGURE_DEPENDS
        "${EXAMPLE_ASSETS_DIR}/*"
    )

    set(VITA_VPK_FILES "")

    foreach(ASSET ${VITA_ASSETS})
        list(APPEND VITA_VPK_FILES
            "${EXAMPLE_ASSETS_DIR}/${ASSET}" "assets/${ASSET}"
        )
    endforeach()

    get_target_property(_ryolib_gxm_shader_dir ryolib RYOLIB_GXM_SHADER_DIR)
    if(NOT _ryolib_gxm_shader_dir)
        set(_ryolib_gxm_shader_dir "${CMAKE_BINARY_DIR}/shaders")
    endif()

    file(GLOB_RECURSE VITA_SHADERS
        CONFIGURE_DEPENDS
        "${_ryolib_gxm_shader_dir}/*"
    )

    foreach(SHADER ${VITA_SHADERS})
        if(IS_DIRECTORY "${SHADER}")
            continue()
        endif()

        get_filename_component(SHADER_NAME "${SHADER}" NAME)

        list(APPEND VITA_VPK_FILES
            "${SHADER}" "shaders/${SHADER_NAME}"
        )
    endforeach()

    vita_create_vpk(ui_test.vpk ${VITA_TITLEID} ui_test.self
        VERSION ${VITA_VERSION}
        NAME ${VITA_APP_NAME}
        FILE ${VITA_VPK_FILES}
    )
endif()

# Copy the example assets next to the executable (or into the app bundle) and
# make sure the Metal shader library compiled by the ryolib target is available
# to the executable at runtime.
if(NOT VITA)
    ryolib_copy_assets(TestApp
        ASSETS_DIR "${EXAMPLE_ASSETS_DIR}"
    )
endif()
