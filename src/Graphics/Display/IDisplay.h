#pragma once

#include <Windows.h>

class VirtualFramebuffer;

class IDisplay
{
public:
    virtual ~IDisplay() = default;

    virtual bool Initialize(
        HWND hwnd,
        const VirtualFramebuffer& framebuffer
    ) = 0;

    virtual void Present(
        const VirtualFramebuffer& framebuffer
    ) = 0;
};