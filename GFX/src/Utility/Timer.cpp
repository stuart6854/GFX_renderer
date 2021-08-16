#include "Timer.h"

#include "GFX/Debug.h"

namespace gfx
{
    Timer::Timer() { Reset(); }

    void Timer::Reset() { m_start = high_resolution_clock::now(); }

    auto Timer::ElapsedSeconds() -> float { return duration_cast<nanoseconds>(high_resolution_clock::now() - m_start).count() * 0.001f * 0.001f * 0.001f; }

    auto Timer::ElapsedMillis() -> float { return ElapsedSeconds() * 1000.0f; }

    ScopedTimer::ScopedTimer(const std::string& name) : m_name(name) {}

    ScopedTimer::~ScopedTimer()
    {
        const float time = m_timer.ElapsedMillis();
        GFX_TRACE("[TIMER] {} - {}ms", m_name, time);
    }

}  // namespace gfx