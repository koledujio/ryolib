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

    set(PSP2CGC_DIR "${CMAKE_CURRENT_BINARY_DIR}/tools")
    set(PSP2CGC_EXE "${PSP2CGC_DIR}/psp2cgc.exe")

    if(CMAKE_HOST_WIN32)
        set(PSP2CGC_RUNNER "${PSP2CGC_EXE}")
    else()
        # psp2cgc is a Windows executable, so it needs Wine on macOS/Linux.
        # If provided use the -DWINE_EXECUTABLE=... value.
        if(NOT WINE_EXECUTABLE)
            find_program(WINE_EXECUTABLE wine)
        endif()

        if(NOT WINE_EXECUTABLE AND CMAKE_HOST_APPLE)
            # The macOS Wine packages ship the command-line binary inside the
            # application bundle instead of putting a "wine" shim in PATH.
            # Use CMAKE_HOST_APPLE: APPLE describes the target, which is not
            # Apple when cross-compiling for the Vita.
            file(GLOB _ryolib_wine_bundles
                "/Applications/Wine*.app/Contents/Resources/wine/bin/wine"
                "$ENV{HOME}/Applications/Wine*.app/Contents/Resources/wine/bin/wine"
            )
            if(_ryolib_wine_bundles)
                list(GET _ryolib_wine_bundles 0 WINE_EXECUTABLE)
            endif()
        endif()

        if(NOT WINE_EXECUTABLE)
            message(FATAL_ERROR
                "[ryolib] Wine is required to run psp2cgc.exe on this host.\n"
                "[ryolib] Install Wine or pass -DWINE_EXECUTABLE=/path/to/wine."
            )
        endif()

        message(STATUS "[ryolib] Using Wine: ${WINE_EXECUTABLE}")
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
        src/shaders/gxm/color_v.cg
        src/shaders/gxm/color_f.cg
        src/shaders/gxm/texture_v.cg
        src/shaders/gxm/texture_f.cg
        src/shaders/gxm/stencil_v.cg
        src/shaders/gxm/stencil_f.cg
    )
    set(GXP_FILES "")

    set(GXM_SHADER_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
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

    add_custom_target(ryolib_gxm_shaders ALL DEPENDS ${GXP_FILES})
    add_dependencies(${TARGET_NAME} ryolib_gxm_shaders)

    # Consumers (the example app and external projects) use this property to
    # find the compiled shaders when packaging them for the Vita.
    set_target_properties(${TARGET_NAME} PROPERTIES
        RYOLIB_GXM_SHADER_DIR "${GXM_SHADER_OUTPUT_DIR}"
    )
endfunction()