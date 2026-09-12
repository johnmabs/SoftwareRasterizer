#include <Windows.h>

#include "App.h"

int WINAPI WinMain(
    __in HINSTANCE instance,
    __in_opt HINSTANCE,
    __in LPSTR,
    __in int showCommand
)
{
    App app;

    if (!app.Initialize(
        instance,
        showCommand))
    {
        MessageBox(
            nullptr,
            L"Application initialization failed.",
            L"Error",
            MB_OK | MB_ICONERROR
        );

        return 1;
    }

    return app.Run();
}