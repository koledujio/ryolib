function(setup_stub_audio_backend TARGET_NAME)
    file(GLOB_RECURSE STUB_AUDIO_SOURCES CONFIGURE_DEPENDS
        "src/backend/audio/stub/*.cpp"
    )

    target_sources(${TARGET_NAME} PRIVATE ${STUB_AUDIO_SOURCES})
endfunction(setup_stub_audio_backend)