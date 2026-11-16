function(setup_stub_image_backend TARGET_NAME)
    file(GLOB_RECURSE STUB_IMAGE_SOURCES CONFIGURE_DEPENDS
        "src/backend/image/stub/*.cpp"
    )

    target_sources(${TARGET_NAME} PRIVATE ${STUB_IMAGE_SOURCES})
endfunction(setup_stub_image_backend)