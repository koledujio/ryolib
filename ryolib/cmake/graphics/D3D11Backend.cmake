include(graphics/CompileHLSL)

function(setup_d3d11_backend TARGET_NAME)
    file(GLOB_RECURSE D3D11_SOURCES CONFIGURE_DEPENDS
        "src/graphics/d3d11/*.cpp"
        "src/graphics/d3d11/*.m"
        "src/graphics/d3d11/*.mm"
    )

    target_sources(${TARGET_NAME} PRIVATE ${D3D11_SOURCES})

    target_compile_definitions(${TARGET_NAME} PRIVATE RYO_ENABLE_GRAPHICS_D3D11=1)

    target_compile_options(${TARGET_NAME} PRIVATE
        $<$<COMPILE_LANGUAGE:OBJCXX>:-Wno-template-body>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-template-body>
    )

    target_link_libraries(${TARGET_NAME} PRIVATE d3d11 dxgi d3dcompiler)

    set(HLSL_SHADERS
        src/shaders/d3d11/color_vs.hlsl
        src/shaders/d3d11/color_ps.hlsl
        src/shaders/d3d11/texture_vs.hlsl
        src/shaders/d3d11/texture_ps.hlsl
        src/shaders/d3d11/stencil_vs.hlsl
        src/shaders/d3d11/stencil_ps.hlsl
    )

    target_compile_hlsl(${TARGET_NAME} SHADERS ${HLSL_SHADERS})
endfunction()