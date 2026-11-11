include(graphics/CompileHLSL)

function(setup_d3d11_backend TARGET_NAME)
    file(GLOB_RECURSE D3D11_SOURCES CONFIGURE_DEPENDS
        "src/backend/graphics/d3d11/*.cpp"
    )

    target_sources(${TARGET_NAME} PRIVATE ${D3D11_SOURCES})

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