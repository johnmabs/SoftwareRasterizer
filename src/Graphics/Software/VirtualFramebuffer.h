#pragma once

#include <cstdint>
#include <vector>

class VirtualFramebuffer
{
public:
     VirtualFramebuffer(
        std::uint32_t width,
        std::uint32_t height
    );

    void Clear(std::uint32_t color);

    void PutPixel(
        std::int32_t x,
        std::int32_t y,
        std::uint32_t color
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