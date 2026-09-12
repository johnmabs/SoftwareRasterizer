#include <Windows.h>

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
    HINSTANCE instance,
    HINSTANCE,
    LPSTR,
    int showCommand)
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

    ShowWindow(hwnd, showCommand);

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
            // Render();
        }
    }

    return 0;
}