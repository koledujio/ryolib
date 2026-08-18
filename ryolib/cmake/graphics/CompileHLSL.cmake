find_program(FXC_EXECUTABLE fxc
    PATHS
    "C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64"
    "C:/Program Files (x86)/Windows Kits/10/bin/x64"
    "$ENV{DXSDK_DIR}/Utilities/bin/x64"
)

set(GFX_HLSL_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(target_compile_hlsl TARGET_NAME)
    cmake_parse_arguments(ARG "" "" "SHADERS" ${ARGN})

    set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_hlsl")
    file(MAKE_DIRECTORY ${GENERATED_DIR})

    foreach(SHADER_FILE ${ARG_SHADERS})
        get_filename_component(FILE_NAME ${SHADER_FILE} NAME_WE)

        if(FILE_NAME MATCHES ".*_vs$")
            set(PROFILE "vs_5_0")
        elseif(FILE_NAME MATCHES ".*_ps$")
            set(PROFILE "ps_5_0")
        else()
            message(FATAL_ERROR "[GFX HLSL] File '${FILE_NAME}' must end with '_vs' or '_ps'")
        endif()

        set(HEADER_OUTPUT "${GENERATED_DIR}/${FILE_NAME}.h")
        set(VAR_NAME "g_${FILE_NAME}")

        if(FXC_EXECUTABLE)
            target_compile_definitions(${TARGET_NAME}
                PRIVATE GFX_HLSL_SOURCE_FALLBACK=0
            )
            add_custom_command(
                OUTPUT ${HEADER_OUTPUT}
                COMMAND ${FXC_EXECUTABLE} /T ${PROFILE} /E main /Fh ${HEADER_OUTPUT} /Vn ${VAR_NAME} /O3 /nologo ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}
                DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}
                COMMENT "[HLSL] Compiling ${FILE_NAME}.hlsl -> ${FILE_NAME}.h"
            )
        else()
            target_compile_definitions(${TARGET_NAME}
                PRIVATE GFX_HLSL_SOURCE_FALLBACK=1
            )
            set(SHADER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}")
            add_custom_command(
                OUTPUT ${HEADER_OUTPUT}
                COMMAND ${CMAKE_COMMAND} -D SHADER_PATH=${SHADER_PATH} -D HEADER_OUTPUT=${HEADER_OUTPUT} -D VAR_NAME=${VAR_NAME} -P "${GFX_HLSL_CMAKE_DIR}/GenerateFallbackHLSL.cmake"
                DEPENDS ${SHADER_PATH}
                COMMENT "[HLSL] Generating fallback header for ${FILE_NAME}.hlsl"
            )
        endif()

        target_sources(${TARGET_NAME} PRIVATE ${HEADER_OUTPUT})
    endforeach()

    target_include_directories(${TARGET_NAME} PUBLIC ${GENERATED_DIR})
endfunction()