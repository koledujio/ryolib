#include "../private.h"

#include <d3dcompiler.h>

namespace ryo {
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
                error_set("Failed to compile shader '%s':\n%.*s", shader.debugName, static_cast<int>(errors->GetBufferSize()), static_cast<const char*>(errors->GetBufferPointer()));
                errors->Release();
            } else {
                error_set("Failed to compile shader '%s'", shader.debugName);
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
}