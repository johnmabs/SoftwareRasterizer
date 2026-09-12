#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>

#include <cstdint>

#include "Graphics/Software/VirtualFramebuffer.h"

class App
{
public:
    App();
    ~App();

    App(const App&) = delete;
    App& operator=(const App&) = delete;

    bool Initialize(
        HINSTANCE instance,
        int showCommand
    );

    int Run();

private:
    static LRESULT CALLBACK WindowProc(
        HWND hwnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );

    bool CreateAppWindow(
        HINSTANCE instance,
        int showCommand
    );

    bool InitializeD3D11();

    bool CreateVirtualScreenTexture();
    bool CreateVirtualScreenSRV();
    bool CreatePointSampler();
    bool CreateFullscreenShaders();

    void UploadFramebuffer();
    void Render();
    void Shutdown();

private:
    static constexpr std::uint32_t VirtualWidth = 320;
    static constexpr std::uint32_t VirtualHeight = 200;

    HWND m_hwnd = nullptr;

    VirtualFramebuffer m_framebuffer;

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