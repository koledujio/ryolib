#pragma once

#include "../private.h"

#include "ryolib/templates.h"

#include "ryolib/graphics.h"

#import "RYD3D11GraphicsBackend.h"

#include <chrono>

#ifndef D3D11_NO_HELPERS
#define D3D11_NO_HELPERS
#endif

#ifndef interface
#define interface struct
#define RYO_UNDEF_INTERFACE 1
#endif

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>

#if RYO_UNDEF_INTERFACE
#undef interface
#undef RYO_UNDEF_INTERFACE
#endif

struct RYInternalRenderer {
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapChain = nullptr;

    int width = 0;
    int height = 0;
    float contentScale = 1.0f;
    ID3D11RenderTargetView* colorView = nullptr;

    ID3D11Texture2D* depthTexture = nullptr;
    ID3D11DepthStencilView* depthView = nullptr;

    ID3D11DepthStencilState* depthStates[3] = { nullptr, nullptr, nullptr };
    ID3D11DepthStencilState* stencilWriteState = nullptr;
    ID3D11DepthStencilState* stencilTestStates[3] = { nullptr, nullptr, nullptr };

    RYPipeline* defaultColorPipeline = nil;
    RYPipeline* defaultTexturePipeline = nil;
    RYPipeline* stencilPipeline = nil;

    RYBuffer* quadVertexBuffer = nil;
    RYBuffer* quadIndexBuffer = nil;
    RYBuffer* quadUVVertexBuffer = nil;
    RYBuffer* quadUVIndexBuffer = nil;

    ID3D11Buffer* vertexCB = nullptr;
    ID3D11Buffer* pixelCB = nullptr;

    ID3D11SamplerState* samplers[4] = { nullptr };
    ID3D11RasterizerState* rasterizers[3][2][2] = {{{ nullptr }}};

    bool inPass = false;
    bool inFrame = false;
    RYRenderTarget* currentTarget = nil;

    float deltaTime = 0.0f;
    std::chrono::steady_clock::time_point lastTime;
};

struct RYInternalRenderTarget {
    ID3D11RenderTargetView* colorView = nullptr;
    ID3D11DepthStencilView* depthView = nullptr;

    int width = 0;
    int height = 0;
};

struct RYInternalTexture {
    ID3D11Texture2D* texture = nullptr;
    ID3D11ShaderResourceView* srv = nullptr;
    int width = 0;
    int height = 0;
};

struct RYInternalBuffer {
    ID3D11Buffer* buffer = nullptr;
    RYBufferType type = RYBufferTypeVertex;
    size_t size = 0;
};

struct RYInternalPipeline {
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;
    ID3D11BlendState* blendState = nullptr;

    RYVertexLayoutType vertexLayout = RYVertexLayoutTypePositionUV;
};

enum RYStencilShapeType {
    _RY_STENCIL_SHAPE_RECTANGLE,
    _RY_STENCIL_SHAPE_ROUNDED_RECTANGLE,
    _RY_STENCIL_SHAPE_OVAL
};

struct RYStencilCommand {
    RYStencilShapeType shape;
    ryo::Matrix4 matrix;

    float radius;
    RYCamera camera;
    bool hasCamera;
};

struct RYStencilVertexUniforms {
    ryo::Matrix4 matrix;
};

struct RYStencilPixelUniforms {
    ryo::Vector2 scale;
    float radius;

    uint32_t shape;
};

struct RYInternalStencil {
    RYInternalRenderer* renderer = nullptr;
    RYInternalRenderTarget* renderTarget = nullptr;

    RYStencilCommand* commands = nullptr;
    size_t commandCount = 0;
    size_t commandCapacity = 0;
};

ryo::Matrix4 _draw_compute_camera_vp(const RYCamera& camera, float aspect);
void _draw_get_target_size(const RYInternalRenderer& renderer, int& width, int& height);
