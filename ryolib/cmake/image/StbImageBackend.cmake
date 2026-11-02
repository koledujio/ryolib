function(setup_stb_image_backend TARGET_NAME)
    target_compile_definitions(${TARGET_NAME} PUBLIC RYO_ENABLE_IMAGE_STB=1)
endfunction(setup_stb_image_backend)
