cmake_minimum_required(VERSION 3.20)
project(GfxLibraryTest C CXX OBJC OBJCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(VITA_APP_NAME "Ryolib UI Test")
set(VITA_TITLEID "KOLE00001")
set(VITA_VERSION "01.00")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CopyAssets)

set(RYO_SAFETY ON CACHE BOOL "" FORCE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(RYO_DEBUG ON CACHE BOOL "" FORCE)
else()
    set(RYO_DEBUG OFF CACHE BOOL "" FORCE)
endif()

add_subdirectory(ryolib)

add_executable(TestApp
    src/main.mm

    src/demos/objcpp_RYViewRoot/implementation.mm
    src/demos/objcpp_RYView/implementation.mm
)

target_link_libraries(TestApp PRIVATE ryolib)

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

    # choosing -static or ld here will use libobjc.a instead of libobjc.dll.a, crashing on startup
    target_link_options(TestApp PRIVATE -static-libgcc -static-libstdc++)

    # copy required runtime DLLs for the Objective-C runtime
    execute_process(
        COMMAND ${CMAKE_OBJC_COMPILER} -print-file-name=libobjc-4.dll
        OUTPUT_VARIABLE RYO_OBJC_RUNTIME_DLL
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(RYO_RUNTIME_DLLS "")
    if(EXISTS "${RYO_OBJC_RUNTIME_DLL}")
        list(APPEND RYO_RUNTIME_DLLS "${RYO_OBJC_RUNTIME_DLL}")
        get_filename_component(RYO_MINGW_LIB_DIR "${RYO_OBJC_RUNTIME_DLL}" DIRECTORY)

        find_file(RYO_WINPTHREAD_RUNTIME_DLL
            NAMES libwinpthread-1.dll
            PATHS
                "${RYO_MINGW_LIB_DIR}/../bin"
                "${RYO_MINGW_LIB_DIR}"
                "${CMAKE_SYSROOT}/bin"
                "${CMAKE_SYSROOT}/x86_64-w64-mingw32/bin"
            NO_DEFAULT_PATH
        )
        if(RYO_WINPTHREAD_RUNTIME_DLL)
            list(APPEND RYO_RUNTIME_DLLS "${RYO_WINPTHREAD_RUNTIME_DLL}")
        endif()

        file(GLOB RYO_GCC_RUNTIME_DLLS "${RYO_MINGW_LIB_DIR}/libgcc_s_*.dll")
        foreach(GCC_DLL ${RYO_GCC_RUNTIME_DLLS})
            list(APPEND RYO_RUNTIME_DLLS "${GCC_DLL}")
        endforeach()
    endif()

    if(RYO_RUNTIME_DLLS)
        add_custom_command(TARGET TestApp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${RYO_RUNTIME_DLLS}
                "$<TARGET_FILE_DIR:TestApp>"
            COMMENT "Copying MinGW runtime DLLs next to TestApp..."
        )
    endif()
elseif(VITA)
    include("${VITASDK}/share/vita.cmake")

    vita_create_self(ui_test.self TestApp)

    file(GLOB_RECURSE VITA_ASSETS
        RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/*"
    )

    set(VITA_VPK_FILES "")

    foreach(ASSET ${VITA_ASSETS})
        list(APPEND VITA_VPK_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/assets/${ASSET}" "assets/${ASSET}"
        )
    endforeach()

    file(GLOB_RECURSE VITA_SHADERS
        CONFIGURE_DEPENDS
        "${CMAKE_BINARY_DIR}/shaders/*"
    )

    foreach(SHADER ${VITA_SHADERS})
        if(IS_DIRECTORY "${SHADER}")
            continue()
        endif()

        get_filename_component(SHADER_NAME "${SHADER}" NAME)

        list(APPEND VITA_VPK_FILES
            "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME}" "shaders/${SHADER_NAME}"
        )
    endforeach()

    vita_create_vpk(ui_test.vpk ${VITA_TITLEID} ui_test.self
        VERSION ${VITA_VERSION}
        NAME ${VITA_APP_NAME}
        FILE ${VITA_VPK_FILES}
    )
endif()

if(NOT VITA)
    target_copy_assets(TestApp)
endif()
