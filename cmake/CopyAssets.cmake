# ryolib_copy_assets(<target> [ASSETS_DIR <dir>])
#
# Makes the runtime files ryolib needs available to <target>:
#   - the compiled Metal shader library, when the Metal backend is used
#   - the optional ASSETS_DIR, exposed to the app as an "assets" directory
#
# The files are copied next to the target's executable, or embedded in a
# macOS/iOS application bundle.
function(ryolib_copy_assets TARGET_NAME)
    cmake_parse_arguments(ARG "" "ASSETS_DIR" "" ${ARGN})

    set(_metallib "")
    if(TARGET ryolib)
        get_target_property(_metallib ryolib RYOLIB_METALLIB)
        if(NOT _metallib)
            set(_metallib "")
        endif()
    endif()

    if(ARG_ASSETS_DIR AND NOT EXISTS "${ARG_ASSETS_DIR}")
        message(FATAL_ERROR
            "[ryolib] Asset directory '${ARG_ASSETS_DIR}' does not exist"
        )
    endif()

    get_target_property(IS_BUNDLE ${TARGET_NAME} MACOSX_BUNDLE)

    if(IOS OR IS_BUNDLE)
        set(BUNDLE_ASSETS "")

        if(ARG_ASSETS_DIR)
            file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS "${ARG_ASSETS_DIR}/*")

            foreach(ASSET_PATH IN LISTS ASSET_FILES)
                file(RELATIVE_PATH REL_PATH "${ARG_ASSETS_DIR}" "${ASSET_PATH}")
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
        endif()

        if(_metallib)
            set_source_files_properties("${_metallib}" PROPERTIES
                GENERATED TRUE
                MACOSX_PACKAGE_LOCATION "."
            )
            list(APPEND BUNDLE_ASSETS "${_metallib}")
        endif()

        if(BUNDLE_ASSETS)
            target_sources(${TARGET_NAME} PRIVATE ${BUNDLE_ASSETS})
        endif()

        if(TARGET ryolib_metal_shaders)
            add_dependencies(${TARGET_NAME} ryolib_metal_shaders)
        endif()
    else()
        if(ARG_ASSETS_DIR)
            add_custom_command(
                TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${ARG_ASSETS_DIR}"
                    "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets"
                COMMENT "Copying assets next to the ${TARGET_NAME} executable..."
            )
        endif()

        if(_metallib)
            add_custom_command(
                TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${_metallib}"
                    "$<TARGET_FILE_DIR:${TARGET_NAME}>/shaders.metallib"
                COMMENT "Copying shaders.metallib next to the ${TARGET_NAME} executable..."
            )
        endif()
    endif()
endfunction()
