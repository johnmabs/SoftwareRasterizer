#include "VirtualFramebuffer.h"

#include <algorithm>

VirtualFramebuffer::VirtualFramebuffer(
    std::uint32_t width,
    std::uint32_t height
)
    : m_width(width),
    m_height(height),
    m_pixels(
        static_cast<std::size_t>(width)*
        static_cast<std::size_t>(height)
    )
{
}

void VirtualFramebuffer::Clear(std::uint32_t color)
{
    std::fill(
        m_pixels.begin(),
        m_pixels.end(),
        color
    );
}

void VirtualFramebuffer::PutPixel(
    std::int32_t x,
    std::int32_t y,
    std::uint32_t color
)
{
    if (x < 0 || y < 0)
    {
        return;
    }

    if (x >= static_cast<std::int32_t>(m_width) ||
        y >= static_cast<std::int32_t>(m_height))
    {
        return;
    }

    const std::size_t index =
        static_cast<std::size_t>(y) * m_width +
        static_cast<std::size_t>(x);

    m_pixels[index] = color;
}

std::uint32_t VirtualFramebuffer::GetWidth() const noexcept
{
    return m_width;
}

std::uint32_t VirtualFramebuffer::GetHeight() const noexcept
{
    return m_height;
}

const std::uint32_t* VirtualFramebuffer::GetData() const noexcept
{
    return m_pixels.data();
}

std::uint32_t* VirtualFramebuffer::GetData() noexcept
{
    return m_pixels.data();
}