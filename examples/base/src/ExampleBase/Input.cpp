#include "Input.h"

namespace example
{
    void Input::NewFrame()
    {
        m_keysLast = m_keys;
    }

    bool Input::OnKeyDown(const uint32_t key) { return m_keys[key] == true && m_keysLast[key] == false; }

    bool Input::OnKeyHeld(const uint32_t key) { return m_keys[key] == true && m_keysLast[key] == true; }

    bool Input::OnKeyUp(const uint32_t key) { return m_keys[key] == false && m_keysLast[key] == true; }

    void Input::SetKeyState(const uint32_t key, const bool isDown) { m_keys[key] = isDown; }

}  // namespace example
