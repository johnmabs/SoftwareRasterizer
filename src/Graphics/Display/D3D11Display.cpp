#include "D3D11Display.h"

#include <d3dcompiler.h>

#include <cstdint>
#include <cstring>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

D3D11Display::~D3D11Display()
{
    Shutdown();
}

bool D3D11Display::InitializeDevice(HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

    swapChainDesc.BufferCount = 2;

    swapChainDesc.BufferDesc.Format =
        DXGI_FORMAT_R8G8B8A8_UNORM;

    swapChainDesc.BufferUsage =
        DXGI_USAGE_RENDER_TARGET_OUTPUT;

    swapChainDesc.OutputWindow = hwnd;

    swapChainDesc.SampleDesc.Count = 1;

    swapChainDesc.Windowed = TRUE;

    swapChainDesc.SwapEffect =
        DXGI_SWAP_EFFECT_DISCARD;

    HRESULT result =
        D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            0,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &swapChainDesc,
            &m_swapChain,
            &m_device,
            nullptr,
            &m_context
        );

    if (FAILED(result))
    {
        return false;
    }

    ID3D11Texture2D* backBuffer = nullptr;

    result = m_swapChain->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(&backBuffer)
    );

    if (FAILED(result))
    {
        return false;
    }

    result = m_device->CreateRenderTargetView(
        backBuffer,
        nullptr,
        &m_renderTarget
    );

    backBuffer->Release();

    return SUCCEEDED(result);
}

bool D3D11Display::CreateVirtualScreenTexture(
    const VirtualFramebuffer& framebuffer
)
{
    D3D11_TEXTURE2D_DESC desc = {};

    desc.Width = framebuffer.GetWidth();
    desc.Height = framebuffer.GetHeight();

    desc.MipLevels = 1;
    desc.ArraySize = 1;

    desc.Format =
        DXGI_FORMAT_R8G8B8A8_UNORM;

    desc.SampleDesc.Count = 1;

    desc.Usage =
        D3D11_USAGE_DYNAMIC;

    desc.BindFlags =
        D3D11_BIND_SHADER_RESOURCE;

    desc.CPUAccessFlags =
        D3D11_CPU_ACCESS_WRITE;

    const HRESULT result =
        m_device->CreateTexture2D(
            &desc,
            nullptr,
            &m_virtualScreenTexture
        );

    return SUCCEEDED(result);
}

bool D3D11Display::CreateVirtualScreenSRV()
{
    D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};

    desc.Format =
        DXGI_FORMAT_R8G8B8A8_UNORM;

    desc.ViewDimension =
        D3D11_SRV_DIMENSION_TEXTURE2D;

    desc.Texture2D.MostDetailedMip = 0;
    desc.Texture2D.MipLevels = 1;

    const HRESULT result =
        m_device->CreateShaderResourceView(
            m_virtualScreenTexture,
            &desc,
            &m_virtualScreenSRV
        );

    return SUCCEEDED(result);
}

bool D3D11Display::CreatePointSampler()
{
    D3D11_SAMPLER_DESC desc = {};

    desc.Filter =
        D3D11_FILTER_MIN_MAG_MIP_POINT;

    desc.AddressU =
        D3D11_TEXTURE_ADDRESS_CLAMP;

    desc.AddressV =
        D3D11_TEXTURE_ADDRESS_CLAMP;

    desc.AddressW =
        D3D11_TEXTURE_ADDRESS_CLAMP;

    desc.MinLOD = 0.0f;
    desc.MaxLOD = D3D11_FLOAT32_MAX;

    const HRESULT result =
        m_device->CreateSamplerState(
            &desc,
            &m_pointSampler
        );

    return SUCCEEDED(result);
}

bool D3D11Display::CreateFullscreenShaders()
{
    ID3DBlob* vertexBlob = nullptr;
    ID3DBlob* pixelBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    HRESULT result =
        D3DCompileFromFile(
            L"shaders/FullscreenVS.hlsl",
            nullptr,
            nullptr,
            "main",
            "vs_5_0",
            0,
            0,
            &vertexBlob,
            &errorBlob
        );

    if (FAILED(result))
    {
        if (errorBlob)
        {
            OutputDebugStringA(
                static_cast<const char*>(
                    errorBlob->GetBufferPointer()
                    )
            );

            errorBlob->Release();
        }

        return false;
    }

    if (errorBlob)
    {
        errorBlob->Release();
        errorBlob = nullptr;
    }

    result =
        m_device->CreateVertexShader(
            vertexBlob->GetBufferPointer(),
            vertexBlob->GetBufferSize(),
            nullptr,
            &m_fullscreenVS
        );

    vertexBlob->Release();

    if (FAILED(result))
    {
        return false;
    }

    result =
        D3DCompileFromFile(
            L"shaders/FullscreenPS.hlsl",
            nullptr,
            nullptr,
            "main",
            "ps_5_0",
            0,
            0,
            &pixelBlob,
            &errorBlob
        );

    if (FAILED(result))
    {
        if (errorBlob)
        {
            OutputDebugStringA(
                static_cast<const char*>(
                    errorBlob->GetBufferPointer()
                    )
            );

            errorBlob->Release();
        }

        return false;
    }

    if (errorBlob)
    {
        errorBlob->Release();
        errorBlob = nullptr;
    }

    result =
        m_device->CreatePixelShader(
            pixelBlob->GetBufferPointer(),
            pixelBlob->GetBufferSize(),
            nullptr,
            &m_fullscreenPS
        );

    pixelBlob->Release();

    return SUCCEEDED(result);
}

bool D3D11Display::Initialize(
    HWND hwnd,
    const VirtualFramebuffer& framebuffer
)
{
    if (!InitializeDevice(hwnd))
    {
        return false;
    }

    if (!CreateVirtualScreenTexture(framebuffer))
    {
        return false;
    }

    if (!CreateVirtualScreenSRV())
    {
        return false;
    }

    if (!CreatePointSampler())
    {
        return false;
    }

    if (!CreateFullscreenShaders())
    {
        return false;
    }

    return true;
}

void D3D11Display::UploadFramebuffer(
    const VirtualFramebuffer& framebuffer
)
{
    D3D11_MAPPED_SUBRESOURCE mapped = {};

    const HRESULT result =
        m_context->Map(
            m_virtualScreenTexture,
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mapped
        );

    if (FAILED(result))
    {
        return;
    }

    const auto* source =
        reinterpret_cast<const std::uint8_t*>(
            framebuffer.GetData()
            );

    auto* destination =
        static_cast<std::uint8_t*>(
            mapped.pData
            );

    const std::size_t sourceRowSize =
        static_cast<std::size_t>(
            framebuffer.GetWidth()
            ) *
        sizeof(std::uint32_t);

    for (
        std::uint32_t y = 0;
        y < framebuffer.GetHeight();
        ++y
        )
    {
        std::memcpy(
            destination +
            static_cast<std::size_t>(y) *
            mapped.RowPitch,

            source +
            static_cast<std::size_t>(y) *
            sourceRowSize,

            sourceRowSize
        );
    }

    m_context->Unmap(
        m_virtualScreenTexture,
        0
    );
}

void D3D11Display::Present(
    const VirtualFramebuffer& framebuffer
)
{
    UploadFramebuffer(framebuffer);

    const float clearColor[] =
    {
        0.0f,
        0.0f,
        0.0f,
        1.0f
    };

    m_context->OMSetRenderTargets(
        1,
        &m_renderTarget,
        nullptr
    );

    m_context->ClearRenderTargetView(
        m_renderTarget,
        clearColor
    );

    D3D11_VIEWPORT viewport = {};

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    viewport.Width = 1280.0f;
    viewport.Height = 720.0f;

    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    m_context->RSSetViewports(
        1,
        &viewport
    );

    m_context->IASetInputLayout(nullptr);

    m_context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    m_context->VSSetShader(
        m_fullscreenVS,
        nullptr,
        0
    );

    m_context->PSSetShader(
        m_fullscreenPS,
        nullptr,
        0
    );

    m_context->PSSetShaderResources(
        0,
        1,
        &m_virtualScreenSRV
    );

    m_context->PSSetSamplers(
        0,
        1,
        &m_pointSampler
    );

    m_context->Draw(
        3,
        0
    );

    m_swapChain->Present(
        1,
        0
    );
}

void D3D11Display::Shutdown()
{
    if (m_context)
    {
        m_context->ClearState();
    }

    if (m_fullscreenPS)
    {
        m_fullscreenPS->Release();
        m_fullscreenPS = nullptr;
    }

    if (m_fullscreenVS)
    {
        m_fullscreenVS->Release();
        m_fullscreenVS = nullptr;
    }

    if (m_pointSampler)
    {
        m_pointSampler->Release();
        m_pointSampler = nullptr;
    }

    if (m_virtualScreenSRV)
    {
        m_virtualScreenSRV->Release();
        m_virtualScreenSRV = nullptr;
    }

    if (m_virtualScreenTexture)
    {
        m_virtualScreenTexture->Release();
        m_virtualScreenTexture = nullptr;
    }

    if (m_renderTarget)
    {
        m_renderTarget->Release();
        m_renderTarget = nullptr;
    }

    if (m_swapChain)
    {
        m_swapChain->Release();
        m_swapChain = nullptr;
    }

    if (m_context)
    {
        m_context->Release();
        m_context = nullptr;
    }

    if (m_device)
    {
        m_device->Release();
        m_device = nullptr;
    }
}