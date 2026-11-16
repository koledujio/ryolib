function(setup_stb_image_backend TARGET_NAME)
    file(GLOB_RECURSE STB_IMAGE_SOURCES CONFIGURE_DEPENDS
        "src/backend/image/stb_image/stb_impl.cpp"
        "src/backend/image/stb_image/image.cpp"
    )

    target_sources(${TARGET_NAME} PRIVATE ${STB_IMAGE_SOURCES})

    if(VITA)
        # stb_image defaults to thread_local storage, which the Vita toolchain
        # turns into emulated TLS relocations that vita-elf-create rejects.
        target_compile_definitions(${TARGET_NAME} PRIVATE STBI_NO_THREAD_LOCALS)
    endif()
endfunction(setup_stb_image_backend)