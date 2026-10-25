function(setup_miniaudio_backend TARGET_NAME)
    target_compile_definitions(${TARGET_NAME} PUBLIC RYO_ENABLE_AUDIO_MINIAUDIO=1)

    if(APPLE)
        set_source_files_properties("src/audio/miniaudio/MiniaudioImplementation.c"
            PROPERTIES COMPILE_OPTIONS "-xobjective-c"
        )
        find_library(AVFOUNDATION_FRAMEWORK AVFoundation REQUIRED)
        find_library(COREAUDIO_FRAMEWORK CoreAudio REQUIRED)
        find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox REQUIRED)
        target_link_libraries(${TARGET_NAME} PRIVATE
            ${AVFOUNDATION_FRAMEWORK}
            ${COREAUDIO_FRAMEWORK}
            ${AUDIOTOOLBOX_FRAMEWORK}
        )
    endif()
endfunction(setup_miniaudio_backend)