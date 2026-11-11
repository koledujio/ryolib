#include "../private.h"

ID3DBlob* _pipeline_compile_shader(const _ShaderSource shader) {
    ID3DBlob* bytecode = nullptr;
    ID3DBlob* errors = nullptr;

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG;
    flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

    HRESULT hr = D3DCompile(
        shader.data,
        shader.size,
        shader.debugName,
        nullptr,
        nullptr,
        "main",
        shader.profile,
        flags,
        0,
        &bytecode,
        &errors
    );

    if (FAILED(hr)) {
        if (errors) {
            errors->Release();
        }
        if (bytecode) {
            bytecode->Release();
        }
        return nullptr;
    }

    if (errors) {
        errors->Release();
    }

    return bytecode;
}
