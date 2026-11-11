function(setup_stb_image_backend TARGET_NAME)
    file(GLOB_RECURSE STB_IMAGE_SOURCES CONFIGURE_DEPENDS
        "src/backend/image/stb_image/stb_impl.cpp"
        "src/backend/image/stb_image/image.cpp"
    )

    target_sources(${TARGET_NAME} PRIVATE ${STB_IMAGE_SOURCES})
endfunction(setup_stb_image_backend)