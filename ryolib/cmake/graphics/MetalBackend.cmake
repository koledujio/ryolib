function(setup_metal_backend TARGET_NAME)
    file(GLOB_RECURSE METAL_SOURCES CONFIGURE_DEPENDS
        "src/backend/graphics/metal/*.mm"
    )

    target_sources(${TARGET_NAME} PRIVATE ${METAL_SOURCES})

    find_library(METAL_FRAMEWORK Metal REQUIRED)
    find_library(QUARTZ_FRAMEWORK QuartzCore REQUIRED)
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)

    target_link_libraries(${TARGET_NAME} PRIVATE
        ${METAL_FRAMEWORK}
        ${QUARTZ_FRAMEWORK}
        ${FOUNDATION_FRAMEWORK}
    )

    if(IOS)
        find_library(UIKIT_FRAMEWORK UIKit REQUIRED)
        target_link_libraries(${TARGET_NAME} PRIVATE ${UIKIT_FRAMEWORK})
    endif()

    set(METAL_SHADERS
        src/shaders/metal/color.metal
        src/shaders/metal/texture.metal
        src/shaders/metal/stencil.metal
    )
    set(AIR_FILES "")

    if(IOS)
        set(DEFAULT_SDK "iphoneos")
    else()
        set(DEFAULT_SDK "macosx")
    endif()
    if(CMAKE_OSX_SYSROOT)
        set(DEFAULT_SDK "${CMAKE_OSX_SYSROOT}")
    endif()

    set(COMPILE_METAL_SCRIPT "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/compile_metal.sh")

    foreach(SHADER IN LISTS METAL_SHADERS)
        get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
        set(AIR_FILE "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.air")

        add_custom_command(
            OUTPUT ${AIR_FILE}
            COMMAND "${COMPILE_METAL_SCRIPT}" "${DEFAULT_SDK}" compile "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}" "${AIR_FILE}"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}" "${COMPILE_METAL_SCRIPT}"
            COMMENT "Compiling ${SHADER_NAME}.metal to ${SHADER_NAME}.air"
            VERBATIM
        )
        list(APPEND AIR_FILES ${AIR_FILE})
    endforeach()

    set(METALLIB_OUTPUT "${CMAKE_BINARY_DIR}/shaders.metallib")
    add_custom_command(
        OUTPUT ${METALLIB_OUTPUT}
        COMMAND "${COMPILE_METAL_SCRIPT}" "${DEFAULT_SDK}" link "${METALLIB_OUTPUT}" ${AIR_FILES}
        DEPENDS ${AIR_FILES} "${COMPILE_METAL_SCRIPT}"
        COMMENT "Linking shaders into shaders.metallib"
        VERBATIM
    )

    add_custom_target(MetalShaders ALL DEPENDS ${METALLIB_OUTPUT})
    add_dependencies(${TARGET_NAME} MetalShaders)
endfunction()