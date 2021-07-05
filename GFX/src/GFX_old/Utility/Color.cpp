//
// Created by stumi on 10/06/21.
//
#include "GFX/Utility/Color.h"

namespace gfxOld
{
    Color::Color() : m_r(0), m_g(0), m_b(0), m_a(1.0f) {}

    Color::Color(float r, float g, float b, float a) : m_r(r), m_g(g), m_b(b), m_a(a) {}

    void Color::Set(float r, float g, float b, float a)
    {
        m_r = r;
        m_g = g;
        m_b = b;
        m_a = a;
    }

    auto Color::ToArray() const -> std::array<float, 4> { return { m_r, m_g, m_b, m_a }; }

}  // namespace gfxOld