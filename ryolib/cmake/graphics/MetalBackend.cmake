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

    set(METAL_SHADERS
        src/shaders/metal/color.metal
        src/shaders/metal/texture.metal
        src/shaders/metal/stencil.metal
    )
    set(AIR_FILES "")

    foreach(SHADER IN LISTS METAL_SHADERS)
        get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
        set(AIR_FILE "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.air")

        add_custom_command(
            OUTPUT ${AIR_FILE}
            COMMAND xcrun -sdk macosx metal -c ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER} -o ${AIR_FILE}
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}
            COMMENT "Compiling ${SHADER_NAME}.metal to ${SHADER_NAME}.air"
        )
        list(APPEND AIR_FILES ${AIR_FILE})
    endforeach()

    set(METALLIB_OUTPUT "${CMAKE_BINARY_DIR}/shaders.metallib")
    add_custom_command(
        OUTPUT ${METALLIB_OUTPUT}
        COMMAND xcrun -sdk macosx metallib ${AIR_FILES} -o ${METALLIB_OUTPUT}
        DEPENDS ${AIR_FILES}
        COMMENT "Linking shaders into shaders.metallib"
    )

    add_custom_target(MetalShaders ALL DEPENDS ${METALLIB_OUTPUT})
    add_dependencies(${TARGET_NAME} MetalShaders)
endfunction()