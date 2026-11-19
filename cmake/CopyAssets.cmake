# Asset staging and application packaging helpers.

include_guard(GLOBAL)

function(_ryolib_target_is_bundle TARGET_NAME OUT_VAR)
    get_target_property(_bundle ${TARGET_NAME} MACOSX_BUNDLE)

    if(IOS OR _bundle)
        set(${OUT_VAR} TRUE PARENT_SCOPE)
    else()
        set(${OUT_VAR} FALSE PARENT_SCOPE)
    endif()
endfunction()

function(_ryolib_stage_asset_dir TARGET_NAME SOURCE_DIR DESTINATION)
    _ryolib_target_is_bundle(${TARGET_NAME} _is_bundle)

    if(_is_bundle)
        file(GLOB_RECURSE _asset_files CONFIGURE_DEPENDS "${SOURCE_DIR}/*")

        foreach(_asset IN LISTS _asset_files)
            if(IS_DIRECTORY "${_asset}")
                continue()
            endif()

            file(RELATIVE_PATH _relative "${SOURCE_DIR}" "${_asset}")
            get_filename_component(_relative_dir "${_relative}" DIRECTORY)

            if(_relative_dir)
                set(_location "${DESTINATION}/${_relative_dir}")
            else()
                set(_location "${DESTINATION}")
            endif()

            set_source_files_properties("${_asset}" PROPERTIES
                MACOSX_PACKAGE_LOCATION "Resources/${_location}"
            )

            target_sources(${TARGET_NAME} PRIVATE "${_asset}")
        endforeach()
    elseif(NOT VITA)
        add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${SOURCE_DIR}"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/${DESTINATION}"
            COMMENT "Copying assets to $<TARGET_FILE_DIR:${TARGET_NAME}>/${DESTINATION}"
            VERBATIM
        )
    endif()
endfunction()

function(_ryolib_stage_asset_file TARGET_NAME SOURCE_FILE DESTINATION)
    _ryolib_target_is_bundle(${TARGET_NAME} _is_bundle)

    if(_is_bundle)
        get_filename_component(_location "${DESTINATION}" DIRECTORY)

        if(_location)
            set(_location "Resources/${_location}")
        else()
            set(_location "Resources")
        endif()

        set_source_files_properties("${SOURCE_FILE}" PROPERTIES
            MACOSX_PACKAGE_LOCATION "${_location}"
        )

        target_sources(${TARGET_NAME} PRIVATE "${SOURCE_FILE}")
    elseif(NOT VITA)
        get_filename_component(_destination_dir "${DESTINATION}" DIRECTORY)

        add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/${_destination_dir}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${SOURCE_FILE}"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/${DESTINATION}"
            COMMENT "Copying ${SOURCE_FILE} to $<TARGET_FILE_DIR:${TARGET_NAME}>/${DESTINATION}"
            VERBATIM
        )
    endif()
endfunction()

function(_ryolib_stage_runtime_shaders TARGET_NAME)
    set(_metallib "")

    if(TARGET ryolib)
        get_target_property(_metallib ryolib RYOLIB_METALLIB)

        if(NOT _metallib)
            set(_metallib "")
        endif()
    endif()

    if(NOT _metallib)
        return()
    endif()

    get_target_property(_already_staged ${TARGET_NAME} RYOLIB_RUNTIME_SHADERS_STAGED)
    if(_already_staged)
        return()
    endif()

    _ryolib_target_is_bundle(${TARGET_NAME} _is_bundle)

    if(_is_bundle)
        set_source_files_properties("${_metallib}" PROPERTIES
            GENERATED TRUE
            MACOSX_PACKAGE_LOCATION "Resources"
        )

        target_sources(${TARGET_NAME} PRIVATE "${_metallib}")
    elseif(NOT VITA)
        add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_metallib}"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>/shaders.metallib"
            COMMENT "Copying shaders.metallib to $<TARGET_FILE_DIR:${TARGET_NAME}>"
            VERBATIM
        )
    endif()

    if(TARGET ryolib_metal_shaders)
        add_dependencies(${TARGET_NAME} ryolib_metal_shaders)
    endif()

    set_target_properties(${TARGET_NAME} PROPERTIES
        RYOLIB_RUNTIME_SHADERS_STAGED TRUE
    )
endfunction()

function(_ryolib_package_vita TARGET_NAME OUTPUT_NAME APP_NAME TITLEID VERSION)
    if(NOT VITA_ELF_CREATE OR NOT VITA_MAKE_FSELF
       OR NOT VITA_MKSFOEX OR NOT VITA_PACK_VPK)
        message(FATAL_ERROR
            "[ryolib] The Vita packaging tools are not available. Configure "
            "with the Vita toolchain file."
        )
    endif()

    if(NOT TITLEID)
        message(FATAL_ERROR
            "[ryolib] ryolib_package_app() requires VITA_TITLEID for Vita builds"
        )
    endif()

    if(NOT APP_NAME)
        set(APP_NAME "${OUTPUT_NAME}")
    endif()

    if(NOT VERSION)
        set(VERSION "01.00")
    endif()

    if(NOT VERSION MATCHES "^[0-9][0-9]\\.[0-9][0-9]$")
        message(FATAL_ERROR
            "[ryolib] Invalid VITA_VERSION '${VERSION}', expected ##.##"
        )
    endif()

    set(_work_dir "${CMAKE_CURRENT_BINARY_DIR}/vita")
    set(_velf "${_work_dir}/${OUTPUT_NAME}.velf")
    set(_self "${_work_dir}/${OUTPUT_NAME}.self")
    set(_sfo "${_work_dir}/${OUTPUT_NAME}_param.sfo")
    set(_vpk "${RYOLIB_OUTPUT_DIR}/${OUTPUT_NAME}.vpk")

    add_custom_command(
        OUTPUT "${_velf}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_work_dir}"
        COMMAND "${VITA_ELF_CREATE}" "$<TARGET_FILE:${TARGET_NAME}>" "${_velf}"
        DEPENDS "$<TARGET_FILE:${TARGET_NAME}>"
        COMMENT "Converting ${OUTPUT_NAME} to a Vita ELF..."
        VERBATIM
    )

    add_custom_command(
        OUTPUT "${_self}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_work_dir}"
        COMMAND "${VITA_MAKE_FSELF}" -c -s "${_velf}" "${_self}"
        DEPENDS "${_velf}"
        COMMENT "Creating ${OUTPUT_NAME}.self..."
        VERBATIM
    )

    add_custom_command(
        OUTPUT "${_sfo}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_work_dir}"
        COMMAND "${VITA_MKSFOEX}"
            -s "TITLE_ID=${TITLEID}"
            -s "APP_VER=${VERSION}"
            "${APP_NAME}"
            "${_sfo}"
        DEPENDS "${_velf}"
        COMMENT "Generating param.sfo for ${OUTPUT_NAME}..."
        VERBATIM
    )

    # Directories are packed as-is so new shaders are picked up without
    # reconfiguring.
    set(_vpk_args "")
    set(_vpk_depends "")

    get_target_property(_asset_dirs ${TARGET_NAME} RYOLIB_ASSET_DIRS)
    if(_asset_dirs)
        foreach(_entry IN LISTS _asset_dirs)
            string(FIND "${_entry}" "=>" _separator)
            string(SUBSTRING "${_entry}" 0 ${_separator} _source)
            math(EXPR _destination_start "${_separator}+2")
            string(SUBSTRING "${_entry}" ${_destination_start} -1 _destination)

            list(APPEND _vpk_args -a "${_source}=${_destination}")

            file(GLOB_RECURSE _asset_files CONFIGURE_DEPENDS "${_source}/*")
            list(APPEND _vpk_depends ${_asset_files})
        endforeach()
    endif()

    get_target_property(_asset_files ${TARGET_NAME} RYOLIB_ASSET_FILES)
    if(_asset_files)
        foreach(_entry IN LISTS _asset_files)
            string(FIND "${_entry}" "=>" _separator)
            string(SUBSTRING "${_entry}" 0 ${_separator} _source)
            math(EXPR _destination_start "${_separator}+2")
            string(SUBSTRING "${_entry}" ${_destination_start} -1 _destination)

            list(APPEND _vpk_args -a "${_source}=${_destination}")
            list(APPEND _vpk_depends "${_source}")
        endforeach()
    endif()

    # Pack the whole shader directory so new shaders are included.
    get_target_property(_shader_dir ryolib RYOLIB_GXM_SHADER_DIR)
    if(NOT _shader_dir)
        set(_shader_dir "${CMAKE_BINARY_DIR}/shaders")
    endif()

    list(APPEND _vpk_args -a "${_shader_dir}=shaders")

    get_target_property(_shader_files ryolib RYOLIB_GXM_SHADERS)
    if(_shader_files)
        list(APPEND _vpk_depends ${_shader_files})
    endif()

    add_custom_command(
        OUTPUT "${_vpk}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RYOLIB_OUTPUT_DIR}"
        COMMAND "${VITA_PACK_VPK}"
            -s "${_sfo}"
            -b "${_self}"
            ${_vpk_args}
            "${_vpk}"
        DEPENDS "${_sfo}" "${_self}" ${_vpk_depends}
        COMMENT "Building ${OUTPUT_NAME}.vpk..."
        VERBATIM
    )

    add_custom_target(${TARGET_NAME}-vita ALL DEPENDS "${_vpk}")
    add_dependencies(${TARGET_NAME}-vita ${TARGET_NAME})

    if(TARGET ryolib_gxm_shaders)
        add_dependencies(${TARGET_NAME}-vita ryolib_gxm_shaders)
    endif()
endfunction()

function(ryolib_add_assets TARGET_NAME)
    cmake_parse_arguments(ARG "" "ASSETS_DIR;DESTINATION" "FILES" ${ARGN})

    if(NOT TARGET ${TARGET_NAME})
        message(FATAL_ERROR
            "[ryolib] ryolib_add_assets: unknown target '${TARGET_NAME}'"
        )
    endif()

    if(NOT ARG_ASSETS_DIR AND NOT ARG_FILES)
        message(FATAL_ERROR
            "[ryolib] ryolib_add_assets: pass ASSETS_DIR, FILES, or both"
        )
    endif()

    set(_destination "assets")

    if(ARG_DESTINATION)
        set(_destination "${ARG_DESTINATION}")
        string(REGEX REPLACE "^(\\./)+" "" _destination "${_destination}")
        string(REGEX REPLACE "/+$" "" _destination "${_destination}")

        if(_destination STREQUAL "" OR _destination STREQUAL ".")
            set(_destination "assets")
        endif()
    endif()

    if(ARG_ASSETS_DIR)
        if(NOT EXISTS "${ARG_ASSETS_DIR}" OR NOT IS_DIRECTORY "${ARG_ASSETS_DIR}")
            message(FATAL_ERROR
                "[ryolib] Asset directory '${ARG_ASSETS_DIR}' does not exist"
            )
        endif()

        get_filename_component(_absolute
            "${ARG_ASSETS_DIR}"
            ABSOLUTE
            BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
        )

        get_target_property(_existing ${TARGET_NAME} RYOLIB_ASSET_DIRS)
        list(FIND _existing "${_absolute}=>${_destination}" _index)

        if(_index EQUAL -1)
            set_property(TARGET ${TARGET_NAME} APPEND PROPERTY
                RYOLIB_ASSET_DIRS "${_absolute}=>${_destination}"
            )

            _ryolib_stage_asset_dir(
                ${TARGET_NAME} "${_absolute}" "${_destination}"
            )
        endif()
    endif()

    foreach(_file IN LISTS ARG_FILES)
        if(NOT EXISTS "${_file}" OR IS_DIRECTORY "${_file}")
            message(FATAL_ERROR
                "[ryolib] Asset file '${_file}' does not exist or is a directory"
            )
        endif()

        get_filename_component(_absolute
            "${_file}"
            ABSOLUTE
            BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        get_filename_component(_name "${_absolute}" NAME)

        if(_destination STREQUAL "" OR _destination STREQUAL ".")
            set(${_file_destination} "${_name}" PARENT_SCOPE)
        else()
            set(${_file_destination} "${_destination}/${_name}" PARENT_SCOPE)
        endif()

        get_target_property(_existing ${TARGET_NAME} RYOLIB_ASSET_FILES)
        list(FIND _existing "${_absolute}=>${_file_destination}" _index)

        if(_index EQUAL -1)
            set_property(TARGET ${TARGET_NAME} APPEND PROPERTY
                RYOLIB_ASSET_FILES "${_absolute}=>${_file_destination}"
            )

            _ryolib_stage_asset_file(
                ${TARGET_NAME} "${_absolute}" "${_file_destination}"
            )
        endif()
    endforeach()
endfunction()

function(ryolib_package_app TARGET_NAME)
    cmake_parse_arguments(ARG ""
        "OUTPUT_NAME;ASSETS_DIR;VITA_APP_NAME;VITA_TITLEID;VITA_VERSION"
        ""
        ${ARGN}
    )

    if(NOT TARGET ${TARGET_NAME})
        message(FATAL_ERROR
            "[ryolib] ryolib_package_app: unknown target '${TARGET_NAME}'"
        )
    endif()

    if(NOT RYOLIB_OUTPUT_DIR)
        set(RYOLIB_OUTPUT_DIR "${CMAKE_BINARY_DIR}/dist")
    elseif(NOT IS_ABSOLUTE "${RYOLIB_OUTPUT_DIR}")
        set(RYOLIB_OUTPUT_DIR "${CMAKE_BINARY_DIR}/${RYOLIB_OUTPUT_DIR}")
    endif()

    if(ARG_OUTPUT_NAME)
        set_target_properties(${TARGET_NAME} PROPERTIES
            OUTPUT_NAME "${ARG_OUTPUT_NAME}"
        )
    else()
        get_target_property(ARG_OUTPUT_NAME ${TARGET_NAME} OUTPUT_NAME)

        if(NOT ARG_OUTPUT_NAME)
            set(ARG_OUTPUT_NAME "${TARGET_NAME}")
        endif()
    endif()

    # The Vita executable is only an intermediate for the VPK.
    if(VITA)
        set(_runtime_output_dir "${CMAKE_CURRENT_BINARY_DIR}/vita")
    else()
        set(_runtime_output_dir "${RYOLIB_OUTPUT_DIR}")
    endif()

    # Avoid Debug/Release subdirectories with multi-config generators.
    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${_runtime_output_dir}"
    )

    foreach(_config IN LISTS CMAKE_CONFIGURATION_TYPES)
        string(TOUPPER "${_config}" _CONFIG)
        set_target_properties(${TARGET_NAME} PROPERTIES
            "RUNTIME_OUTPUT_DIRECTORY_${_CONFIG}" "${_runtime_output_dir}"
        )
    endforeach()

    if(ARG_ASSETS_DIR)
        ryolib_add_assets(${TARGET_NAME} ASSETS_DIR "${ARG_ASSETS_DIR}")
    endif()

    _ryolib_stage_runtime_shaders(${TARGET_NAME})

    if(VITA)
        _ryolib_package_vita(
            ${TARGET_NAME}
            "${ARG_OUTPUT_NAME}"
            "${ARG_VITA_APP_NAME}"
            "${ARG_VITA_TITLEID}"
            "${ARG_VITA_VERSION}"
        )
    endif()
endfunction()
