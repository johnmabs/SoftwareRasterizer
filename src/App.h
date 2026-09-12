#pragma once

#include <Windows.h>

#include <cstdint>

#include "Graphics/Display/D3D11Display.h"
#include "Graphics/Software/VirtualFramebuffer.h"

class App
{
public:
    App();

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

private:
    static constexpr std::uint32_t VirtualWidth = 320;
    static constexpr std::uint32_t VirtualHeight = 200;

    HWND m_hwnd = nullptr;

    VirtualFramebuffer m_framebuffer;

    D3D11Display m_display;
};