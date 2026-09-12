#include <Windows.h>

#include <cstring>
#include <cstdint>

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>


#include "Graphics/Software/VirtualFramebuffer.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gContext = nullptr;
IDXGISwapChain* gSwapChain = nullptr;
ID3D11RenderTargetView* gRenderTarget = nullptr;

ID3D11Texture2D* gVirtualScreenTexture = nullptr;

ID3D11ShaderResourceView* gVirtualScreenSRV = nullptr;
ID3D11SamplerState* gPointSampler = nullptr;

ID3D11VertexShader* gFullscreenVS = nullptr;
ID3D11PixelShader* gFullscreenPS = nullptr;

bool InitializeD3D11(HWND hwnd)
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
            &gSwapChain,
            &gDevice,
            nullptr,
            &gContext
        );

    if (FAILED(result))
    {
        return false;
    }

    ID3D11Texture2D* backBuffer = nullptr;
    result = gSwapChain->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(&backBuffer)
    );

    if (FAILED(result))
    {
        return false;
    }

    result = gDevice->CreateRenderTargetView(
        backBuffer,
        nullptr,
        &gRenderTarget
    );

    backBuffer->Release();

    if (FAILED(result))
    {
        return false;
    }

    return true;
}
void ShutdownD3D11()
{

    if (gFullscreenPS)
    {
        gFullscreenPS->Release();
        gFullscreenPS = nullptr;
    }

    if (gFullscreenVS)
    {
        gFullscreenVS->Release();
        gFullscreenVS = nullptr;
    }

    if (gPointSampler)
    {
        gPointSampler->Release();
        gPointSampler = nullptr;
    }

    if (gVirtualScreenSRV)
    {
        gVirtualScreenSRV->Release();
        gVirtualScreenSRV = nullptr;
    }

    if (gVirtualScreenTexture)
    {
        gVirtualScreenTexture->Release();
        gVirtualScreenTexture = nullptr;
    }

    if (gRenderTarget)
    {
        gRenderTarget->Release();
        gRenderTarget = nullptr;
    }

    if (gSwapChain)
    {
        gSwapChain->Release();
        gSwapChain = nullptr;
    }

    if (gContext)
    {
        gContext->Release();
        gContext = nullptr;
    }

    if (gDevice)
    {
        gDevice->Release();
        gDevice = nullptr;
    }
}

bool CreateVirtualScreenTexture(
    std::uint32_t width,
    std::uint32_t height
)
{
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags =
        D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags =
        D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;

    HRESULT result =
        gDevice->CreateTexture2D(
            &desc,
            nullptr,
            &gVirtualScreenTexture
        );

    return SUCCEEDED(result);
}

bool CreateVirtualScreenSRV()
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    HRESULT result =
        gDevice->CreateShaderResourceView(
            gVirtualScreenTexture,
            &srvDesc,
            &gVirtualScreenSRV
        );

    return SUCCEEDED(result);
}

bool CreatePointSampler()
{
    D3D11_SAMPLER_DESC desc = {};

    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

    desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    desc.MinLOD = 0.0f;
    desc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT result =
        gDevice->CreateSamplerState(
            &desc,
            &gPointSampler
        );

    return SUCCEEDED(result);
}

bool CreateFullscreenShaders()
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

    result = gDevice->CreateVertexShader(
        vertexBlob->GetBufferPointer(),
        vertexBlob->GetBufferSize(),
        nullptr,
        &gFullscreenVS
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

    result = gDevice->CreatePixelShader(
        pixelBlob->GetBufferPointer(),
        pixelBlob->GetBufferSize(),
        nullptr,
        &gFullscreenPS
    );

    pixelBlob->Release();

    return SUCCEEDED(result);
}

void UploadFramebuffer(
    const VirtualFramebuffer& framebuffer
)
{
    D3D11_MAPPED_SUBRESOURCE mapped = {};

    HRESULT result =
        gContext->Map(
            gVirtualScreenTexture,
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mapped
        );

    if (FAILED(result))
    {
        return;
    }

    const std::uint8_t* source =
        reinterpret_cast<const std::uint8_t*>(
            framebuffer.GetData()
            );

    std::uint8_t* destination =
        static_cast<std::uint8_t*>(
            mapped.pData
            );

    const std::size_t sourceRowSize =
        static_cast<std::size_t>(
            framebuffer.GetWidth()
            ) * sizeof(std::uint32_t);

    for (std::uint32_t y = 0;
        y < framebuffer.GetHeight();
        ++y)
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

    gContext->Unmap(
        gVirtualScreenTexture,
        0
    );
}

void Render(
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

    gContext->OMSetRenderTargets(
        1,
        &gRenderTarget,
        nullptr
    );

    gContext->ClearRenderTargetView(
        gRenderTarget,
        clearColor
    );

    D3D11_VIEWPORT viewport = {};

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    viewport.Width = 1280.0f;
    viewport.Height = 720.0f;

    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    gContext->RSSetViewports(
        1,
        &viewport
    );

    gContext->IASetInputLayout(nullptr);

    gContext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    gContext->VSSetShader(
        gFullscreenVS,
        nullptr,
        0
    );

    gContext->PSSetShader(
        gFullscreenPS,
        nullptr,
        0
    );

    gContext->PSSetShaderResources(
        0,
        1,
        &gVirtualScreenSRV
    );

    gContext->PSSetSamplers(
        0,
        1,
        &gPointSampler
    );

    gContext->Draw(
        3,
        0
    );

    gSwapChain->Present(
        1,
        0
    );
}

LRESULT CALLBACK WindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(
        hwnd,
        message,
        wParam,
        lParam
    );
}

int WINAPI WinMain(
    __in HINSTANCE instance,
    __in_opt HINSTANCE,
    __in LPSTR,
    __in int showCommand)
{
    const wchar_t* className = L"SoftwareRasterizerWindow";

    WNDCLASS windowClass = {};
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = className;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&windowClass);

    HWND hwnd = CreateWindowEx(
        0,
        className,
        L"Software Rasterizer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1280,
        720,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (!hwnd)
    {
        return 0;
    }

    VirtualFramebuffer framebuffer(320, 200);

    ShowWindow(hwnd, showCommand);

    if (!InitializeD3D11(hwnd))
    {
        MessageBox(
            hwnd,
            L"Impossible d'initialiser DirectX 11.",
            L"Erreur",
            MB_OK
        );

        return 0;
    }

    if (!CreateVirtualScreenTexture(
        framebuffer.GetWidth(),
        framebuffer.GetHeight()))
    {
        MessageBox(
            hwnd,
            L"Impossible de créer la texture virtuelle.",
            L"Erreur",
            MB_OK
        );

        return 0;
    }

    if (!CreateVirtualScreenSRV())
    {
        return 0;
    }

    if (!CreatePointSampler())
    {
        return 0;
    }

    if (!CreateFullscreenShaders())
    {
        return 0;
    }

    MSG message = {};

    while (message.message != WM_QUIT)
    {
        if (PeekMessage(
            &message,
            nullptr,
            0,
            0,
            PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        else
        {
            // Update();

            framebuffer.Clear(0xFF000000);

            for (int y = 50; y < 150; ++y)
            {
                for (int x = 80; x < 240; ++x)
                {
                    framebuffer.PutPixel(
                        x,
                        y,
                        0xFF0000FF
                    );
                }
            }

            Render(framebuffer);
        }
    }

    ShutdownD3D11();
    return 0;
}