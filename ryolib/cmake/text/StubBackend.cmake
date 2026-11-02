function(setup_stub_text_backend TARGET_NAME)
    target_compile_definitions(${TARGET_NAME} PUBLIC RYO_ENABLE_TEXT_STUB=1)
endfunction(setup_stub_text_backend)
