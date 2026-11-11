function(setup_stub_graphics_backend TARGET_NAME)
    file(GLOB_RECURSE STUB_GRAPHICS_SOURCES CONFIGURE_DEPENDS
        "src/backend/graphics/stub/*.cpp"
    )

    target_sources(${TARGET_NAME} PRIVATE ${STUB_GRAPHICS_SOURCES})
endfunction(setup_stub_graphics_backend)