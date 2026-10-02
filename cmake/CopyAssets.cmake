function(target_copy_assets TARGET_NAME)
    file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/assets/*")

    get_target_property(IS_BUNDLE ${TARGET_NAME} MACOSX_BUNDLE)

    if(IOS OR IS_BUNDLE)
        set(BUNDLE_ASSETS "")
        foreach(ASSET_PATH IN LISTS ASSET_FILES)
            file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}/assets" "${ASSET_PATH}")
            get_filename_component(REL_DIR "${REL_PATH}" DIRECTORY)
            if(REL_DIR)
                set_source_files_properties("${ASSET_PATH}" PROPERTIES
                    MACOSX_PACKAGE_LOCATION "assets/${REL_DIR}"
                )
            else()
                set_source_files_properties("${ASSET_PATH}" PROPERTIES
                    MACOSX_PACKAGE_LOCATION "assets"
                )
            endif()
            list(APPEND BUNDLE_ASSETS "${ASSET_PATH}")
        endforeach()

        set_source_files_properties("${CMAKE_BINARY_DIR}/shaders.metallib" PROPERTIES
            GENERATED TRUE
            MACOSX_PACKAGE_LOCATION "."
        )
        list(APPEND BUNDLE_ASSETS "${CMAKE_BINARY_DIR}/shaders.metallib")

        target_sources(${TARGET_NAME} PRIVATE ${BUNDLE_ASSETS})

        if(TARGET MetalShaders)
            add_dependencies(${TARGET_NAME} MetalShaders)
        endif()
    else()
        set(ASSETS_SRC_DIR "${CMAKE_SOURCE_DIR}/assets")
        set(ASSETS_DST_DIR "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets")

        add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${ASSETS_SRC_DIR}"
                "${ASSETS_DST_DIR}"
            COMMENT "Copying assets directory next to the ${TARGET_NAME} executable..."
        )

        if(APPLE)
            add_custom_command(
                TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${CMAKE_BINARY_DIR}/shaders.metallib"
                    "$<TARGET_FILE_DIR:${TARGET_NAME}>/shaders.metallib"
                COMMENT "Copying shaders.metallib next to the ${TARGET_NAME} executable..."
            )
        endif()
    endif()
endfunction()