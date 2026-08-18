function(setup_miniaudio_backend TARGET_NAME)
    file(GLOB_RECURSE MINIAUDIO_SOURCES CONFIGURE_DEPENDS
        "src/backend/audio/miniaudio/*.cpp"
    )

    target_sources(${TARGET_NAME} PRIVATE ${MINIAUDIO_SOURCES})
endfunction(setup_miniaudio_backend)