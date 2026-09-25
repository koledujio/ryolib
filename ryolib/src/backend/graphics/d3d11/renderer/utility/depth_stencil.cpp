#include "../private.h"

namespace ryo {
    bool _renderer_create_depth_stencil_state(InternalRenderer& ir) {
        /*
         * Depth Stencil States
         * 0: Disabled
         * 1: Read-Only
         * 2: Read-Write
         */
        D3D11_DEPTH_STENCIL_DESC dsDesc = {};
        dsDesc.DepthEnable = FALSE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        dsDesc.StencilEnable = FALSE;
        dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
        dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        dsDesc.BackFace = dsDesc.FrontFace;
        HRESULT hr = 0;

        // 0
        dsDesc.DepthEnable = FALSE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        hr = ir.device->CreateDepthStencilState(&dsDesc, &ir.depthStates[0]);
        if (FAILED(hr)) {
            error_set("Failed to create disabled depth stencil state 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        // 1
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        hr = ir.device->CreateDepthStencilState(&dsDesc, &ir.depthStates[1]);
        if (FAILED(hr)) {
            error_set("Failed to create read-only depth stencil state 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        // 2
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        hr = ir.device->CreateDepthStencilState(&dsDesc, &ir.depthStates[2]);
        if (FAILED(hr)) {
            error_set("Failed to create read-write depth stencil state 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        // Stencil Write State
        D3D11_DEPTH_STENCIL_DESC writeDesc = {};
        writeDesc.DepthEnable = FALSE;
        writeDesc.StencilEnable = TRUE;
        writeDesc.StencilWriteMask = 0xFF;
        writeDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        writeDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        writeDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
        writeDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        writeDesc.BackFace = writeDesc.FrontFace;
        hr = ir.device->CreateDepthStencilState(&writeDesc, &ir.stencilWriteState);
        if (FAILED(hr)) {
            error_set("Failed to create stencil write stencil state 0x%08X", static_cast<unsigned int>(hr));
            return false;
        }

        // Stencil Test States
        for (int i = 0; i < 3; i++) {
            D3D11_DEPTH_STENCIL_DESC testDesc = {};

            switch (i) {
                case 0:
                    testDesc.DepthEnable = FALSE;
                    break;
                
                case 1:
                case 2:
                    testDesc.DepthEnable = TRUE;
                    testDesc.DepthWriteMask = i == 1 ? D3D11_DEPTH_WRITE_MASK_ZERO : D3D11_DEPTH_WRITE_MASK_ALL;
                    testDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
                    break;
            }

            testDesc.StencilEnable = TRUE;
            testDesc.StencilReadMask = 0xFF;
            testDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
            testDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
            testDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
            testDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
            testDesc.BackFace = testDesc.FrontFace;

            hr = ir.device->CreateDepthStencilState(&testDesc, &ir.stencilTestStates[i]);
            if (FAILED(hr)) {
                const char* stencilType = "Unknown";

                switch (i) {
                    case 0:
                        stencilType = "disabled";
                        break;
                    
                    case 1:
                        stencilType = "read-only";
                        break;
                    
                    case 2:
                        stencilType = "read-write";
                        break;
                }

                error_set("Failed to create stencil test %s depth stencil state 0x%08X", stencilType, static_cast<unsigned int>(hr));
                return false;
            }
        }

        return true;
    }
}