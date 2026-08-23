function(setup_stub_text_backend TARGET_NAME)
    file(GLOB_RECURSE STUB_TEXT_SOURCES CONFIGURE_DEPENDS
        "src/backend/text/stub/*.cpp"
    )

    target_sources(${TARGET_NAME} PRIVATE ${STUB_TEXT_SOURCES})
endfunction(setup_stub_text_backend)