function(setup_stub_windowing_backend TARGET_NAME)
    file(GLOB_RECURSE STUB_WINDOWING_SOURCES CONFIGURE_DEPENDS
        "src/backend/windowing/stub/*.cpp"
    )

    target_sources(${TARGET_NAME} PRIVATE ${STUB_WINDOWING_SOURCES})
endfunction(setup_stub_windowing_backend)