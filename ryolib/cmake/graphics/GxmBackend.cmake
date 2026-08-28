function(setup_gxm_backend TARGET_NAME)
    include("${VITASDK}/share/vita.cmake")

    file(GLOB_RECURSE GXM_SOURCES CONFIGURE_DEPENDS
        "src/backend/graphics/gxm/*.cpp"
    )

    target_sources(${TARGET_NAME} PRIVATE ${GXM_SOURCES})

    target_link_libraries(${TARGET_NAME} PRIVATE
        SceGxm_stub
        SceDisplay_stub
        SceCtrl_stub
    )

    set(PSP2CGC_DIR "${CMAKE_BINARY_DIR}/tools")
    set(PSP2CGC_EXE "${PSP2CGC_DIR}/psp2cgc.exe")

    if(CMAKE_HOST_WIN32)
        set(PSP2CGC_RUNNER "${PSP2CGC_EXE}")
    else()
        find_program(WINE_EXECUTABLE wine REQUIRED)
        set(PSP2CGC_RUNNER "${WINE_EXECUTABLE}" "${PSP2CGC_EXE}")
    endif()

    file(MAKE_DIRECTORY "${PSP2CGC_DIR}")

    if(NOT EXISTS "${PSP2CGC_EXE}")
        file(DOWNLOAD
            "https://github.com/SomeoneCoolWhoDoesThings/psp2cgc/releases/download/03.5700/psp2cgc.exe"
            "${PSP2CGC_EXE}"
            SHOW_PROGRESS
            STATUS DOWNLOAD_STATUS
        )

        list(GET DOWNLOAD_STATUS 0 DOWNLOAD_RESULT)

        if(NOT DOWNLOAD_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to download psp2cgc.exe")
        endif()
    endif()

    set(GXM_SHADERS
        src/shaders/gxm/color_f.cg
        src/shaders/gxm/color_v.cg
    )
    set(GXP_FILES "")

    set(GXM_SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")
    file(MAKE_DIRECTORY "${GXM_SHADER_OUTPUT_DIR}")

    set(GXM_SHADER_WORK_DIR "${CMAKE_CURRENT_BINARY_DIR}/gxm_shader_work")
    file(MAKE_DIRECTORY "${GXM_SHADER_WORK_DIR}")

    foreach(SHADER ${GXM_SHADERS})
        get_filename_component(NAME ${SHADER} NAME_WE)

        set(GXP "${GXM_SHADER_OUTPUT_DIR}/${NAME}.gxp")

        if(NAME MATCHES "_v$")
            set(PROFILE sce_vp_psp2)
        else()
            set(PROFILE sce_fp_psp2)
        endif()

        add_custom_command(
            OUTPUT "${GXP}"

            COMMAND "${CMAKE_COMMAND}" -E copy
                    "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}"
                    "${GXM_SHADER_WORK_DIR}/${NAME}.cg"

            COMMAND "${CMAKE_COMMAND}" -E chdir
                    "${GXM_SHADER_WORK_DIR}"
                    ${PSP2CGC_RUNNER}
                    -profile "${PROFILE}"
                    -o "${NAME}.gxp"
                    "${NAME}.cg"

            COMMAND "${CMAKE_COMMAND}" -E copy
                    "${GXM_SHADER_WORK_DIR}/${NAME}.gxp"
                    "${GXM_SHADER_OUTPUT_DIR}/${NAME}.gxp"

            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}"

            COMMENT "Compiling ${NAME}.cg -> ${NAME}.gxp"
        )

        list(APPEND GXP_FILES ${GXP})
    endforeach()

    add_custom_target(GxmShaders ALL DEPENDS ${GXP_FILES})
    add_dependencies(${TARGET_NAME} GxmShaders)
endfunction()