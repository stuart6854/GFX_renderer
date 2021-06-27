#include "ExampleBase.h"

#include <fmt/format.h>

#include <chrono>

namespace example
{
    void ExampleBase::Run()
    {
        Init();

        using clock = std::chrono::high_resolution_clock;
        auto last = clock::now();
        auto delta = 0.0f;
        float accum = 0.0f;
        while (!m_window->ShouldClose())
        {
            {
                auto now = clock::now();
                using ms = std::chrono::duration<float, std::milli>;
                delta = std::chrono::duration_cast<ms>(now - last).count() / 1000.0f;
                last = now;
                accum += delta;
                if (accum >= 0.5f)
                {
                    accum = 0.0f;
                    m_window->SetTitle(fmt::format("{} - {:d}ms ({:f}s)", m_title, int(delta * 1000.0f), delta));
                }
            }
            m_window->PollEvents();

            Update_(delta);
            Render_();
        }
    }

    void ExampleBase::Init()
    {
        m_window = std::make_shared<Window>("ExampleBase", 1080, 720);
        m_input = &m_window->GetInput();

        Init_();
    }

}  // namespace example
