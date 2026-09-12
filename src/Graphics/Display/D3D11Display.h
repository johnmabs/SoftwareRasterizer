#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>

#include "../Software/VirtualFramebuffer.h"

class D3D11Display
{
public:
    D3D11Display() = default;
    ~D3D11Display();

    D3D11Display(const D3D11Display&) = delete;
    D3D11Display& operator=(const D3D11Display&) = delete;

    bool Initialize(
        HWND hwnd,
        const VirtualFramebuffer& framebuffer
    );

    void Present(
        const VirtualFramebuffer& framebuffer
    );

private:
    bool InitializeDevice(HWND hwnd);

    bool CreateVirtualScreenTexture(
        const VirtualFramebuffer& framebuffer
    );

    bool CreateVirtualScreenSRV();
    bool CreatePointSampler();
    bool CreateFullscreenShaders();

    void UploadFramebuffer(
        const VirtualFramebuffer& framebuffer
    );

    void Shutdown();

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;

    IDXGISwapChain* m_swapChain = nullptr;
    ID3D11RenderTargetView* m_renderTarget = nullptr;

    ID3D11Texture2D* m_virtualScreenTexture = nullptr;
    ID3D11ShaderResourceView* m_virtualScreenSRV = nullptr;

    ID3D11SamplerState* m_pointSampler = nullptr;

    ID3D11VertexShader* m_fullscreenVS = nullptr;
    ID3D11PixelShader* m_fullscreenPS = nullptr;
};