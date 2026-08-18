#pragma once

#include "../private.h"

#include "ryolib/templates.h"

#include "ryolib/graphics.h"

#include <chrono>
#include <d3d11.h>
#include <dxgi.h>

namespace ryo {
    constexpr uint32_t MAX_RENDERERS = 8;
    constexpr uint32_t MAX_TEXTURES = 1024;
    constexpr uint32_t MAX_BUFFERS = 1024;
    constexpr uint32_t MAX_PIPELINES = 512;
    constexpr uint32_t MAX_RENDER_TARGETS = 256;

    struct _InternalRenderer {
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* context = nullptr;
        IDXGISwapChain* swapChain = nullptr;
        ID3D11RenderTargetView* renderTargetView = nullptr;
        ID3D11Texture2D* depthStencilTexture = nullptr;
        ID3D11DepthStencilView* depthStencilView = nullptr;
        ID3D11DepthStencilState* depthStencilState = nullptr;
        int width = 0;
        int height = 0;

        Pipeline defaultColorPipeline = { 0 };
        Pipeline defaultTexturePipeline = { 0 };

        bool inPass = false;
        RenderTarget currentTarget = { 0 };

        float deltaTime = 0.0f;
        std::chrono::steady_clock::time_point lastTime;
    };

    struct _InternalTexture {
        ID3D11Texture2D* texture = nullptr;
        ID3D11ShaderResourceView* srv = nullptr;
        int width = 0;
        int height = 0;
    };

    struct _InternalBuffer {
        ID3D11Buffer* buffer = nullptr;
        BufferType type = BUFFER_TYPE_VERTEX;
        size_t size = 0;
    };

    struct _InternalPipeline {
        ID3D11VertexShader* vertexShader = nullptr;
        ID3D11PixelShader* pixelShader = nullptr;
        ID3D11InputLayout* inputLayout = nullptr;
        ID3D11BlendState* blendState = nullptr;

        VertexLayoutType vertexLayout = VERTEX_LAYOUT_POSITION_UV;
    };

    struct _InternalRenderTarget {
        ID3D11Texture2D* colorTexture = nullptr;
        ID3D11RenderTargetView* colorView = nullptr;

        ID3D11Texture2D* depthTexture = nullptr;
        ID3D11DepthStencilView* depthView = nullptr;

        Texture colorTextureHandle = { 0 };
        Texture depthTextureHandle = { 0 };

        int width = 0;
        int height = 0;
    };

    extern ResourcePool<_InternalRenderer, MAX_RENDERERS> _renderer_pool;
    extern ResourcePool<_InternalTexture, MAX_TEXTURES> _texture_pool;
    extern ResourcePool<_InternalBuffer, MAX_BUFFERS> _buffer_pool;
    extern ResourcePool<_InternalPipeline, MAX_PIPELINES> _pipeline_pool;
    extern ResourcePool<_InternalRenderTarget, MAX_RENDER_TARGETS> _render_target_pool;
}
