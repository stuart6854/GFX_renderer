//
// Created by stumi on 10/06/21.
//

#ifndef PERSONAL_RENDERER_COLOR_H
#define PERSONAL_RENDERER_COLOR_H

#include <array>

namespace gfx
{
    class Color
    {
    public:
        Color();
        Color(float r, float g, float b, float a = 1.0f);

        auto GetR() const -> float { return m_r; }
        auto GetG() const -> float { return m_g; }
        auto GetB() const -> float { return m_b; }
        auto GetA() const -> float { return m_a; }

        void Set(float r, float g, float b, float a = 1.0f);

        auto ToArray() const -> std::array<float, 4>;

    private:
        float m_r;
        float m_g;
        float m_b;
        float m_a;
    };

}  // namespace gfx

#endif  // PERSONAL_RENDERER_COLOR_H
