#include "App.h"

#include "Graphics/Display/D3D11Display.h"

App::App()
    : m_framebuffer(
        VirtualWidth,
        VirtualHeight
    )
{
}

LRESULT CALLBACK App::WindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
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

bool App::CreateAppWindow(
    HINSTANCE instance,
    int showCommand
)
{
    const wchar_t* className =
        L"SoftwareRasterizerWindow";

    WNDCLASS windowClass = {};

    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = className;
    windowClass.hCursor =
        LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClass(&windowClass))
    {
        return false;
    }

    m_hwnd = CreateWindowEx(
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

    if (!m_hwnd)
    {
        return false;
    }

    ShowWindow(
        m_hwnd,
        showCommand
    );

    return true;
}

bool App::Initialize(
    HINSTANCE instance,
    int showCommand
)
{
    if (!CreateAppWindow(
        instance,
        showCommand))
    {
        return false;
    }

    m_display =
        std::make_unique<D3D11Display>();

    if (!m_display->Initialize(
        m_hwnd,
        m_framebuffer))
    {
        return false;
    }

    return true;
}

int App::Run()
{
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
            m_framebuffer.Clear(
                0xFF000000
            );

            for (int y = 50; y < 150; ++y)
            {
                for (int x = 80; x < 240; ++x)
                {
                    m_framebuffer.PutPixel(
                        x,
                        y,
                        0xFF0000FF
                    );
                }
            }

            m_display->Present(
                m_framebuffer
            );
        }
    }

    return static_cast<int>(
        message.wParam
        );
}
