#pragma once

#include <GLFW/glfw3.h>

#include <array>

namespace example
{
    class Input
    {
    public:
        void NewFrame();

        bool OnKeyDown(uint32_t key);
        bool OnKeyHeld(uint32_t key);
        bool OnKeyUp(uint32_t key);

        void SetKeyState(uint32_t key, bool isDown);

    private:
        std::array<bool, GLFW_KEY_LAST> m_keys;
        std::array<bool, GLFW_KEY_LAST> m_keysLast;
    };
}  // namespace example
