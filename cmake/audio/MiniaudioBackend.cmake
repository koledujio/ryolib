function(setup_miniaudio_backend TARGET_NAME)
    file(GLOB_RECURSE MINIAUDIO_SOURCES CONFIGURE_DEPENDS
        "src/audio/miniaudio/*.cpp"
    )

    target_sources(${TARGET_NAME} PRIVATE ${MINIAUDIO_SOURCES})

    if(APPLE)
        set_source_files_properties("src/audio/miniaudio/vendor/miniaudio_impl.cpp"
            PROPERTIES COMPILE_OPTIONS "-xobjective-c++"
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