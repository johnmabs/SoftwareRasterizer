#pragma once

#include <cstdint>

class Color
{
public:
    constexpr Color(
        std::uint8_t r,
        std::uint8_t g,
        std::uint8_t b,
        std::uint8_t a = 255
    ) noexcept
        : m_value(
            static_cast<std::uint32_t>(r)
            |
            (static_cast<std::uint32_t>(g) << 8)
            |
            (static_cast<std::uint32_t>(b) << 16)
            |
            (static_cast<std::uint32_t>(a) << 24)
        )
    {
    }

    [[nodiscard]]
    constexpr std::uint32_t Value() const noexcept
    {
        return m_value;
    }

private:
    std::uint32_t m_value;
};

namespace Colors
{
    inline constexpr Color Black{ 0, 0, 0 };
    inline constexpr Color White{ 255, 255, 255 };
    inline constexpr Color Red{ 255, 0, 0 };
    inline constexpr Color Green{ 0, 255, 0 };
    inline constexpr Color Blue{ 0, 0, 255 };
}