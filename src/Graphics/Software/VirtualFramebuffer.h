#pragma once

#include <cstdint>
#include <vector>

#include "Color.h"

class VirtualFramebuffer
{
public:
     VirtualFramebuffer(
        std::uint32_t width,
        std::uint32_t height
    );

    void Clear(Color color);

    void PutPixel(
        std::int32_t x,
        std::int32_t y,
        Color color
    );

    [[nodiscard]]
    std::uint32_t GetWidth() const noexcept;

    [[nodiscard]]
    std::uint32_t GetHeight() const noexcept;

    [[nodiscard]]
    const std::uint32_t* GetData() const noexcept;

    [[nodiscard]]
    std::uint32_t* GetData() noexcept;

private:
    std::uint32_t m_width;
    std::uint32_t m_height;

    std::vector<std::uint32_t> m_pixels;
};