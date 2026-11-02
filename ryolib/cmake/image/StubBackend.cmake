function(setup_stub_image_backend TARGET_NAME)
    target_compile_definitions(${TARGET_NAME} PUBLIC RYO_ENABLE_IMAGE_STUB=1)
endfunction(setup_stub_image_backend)
