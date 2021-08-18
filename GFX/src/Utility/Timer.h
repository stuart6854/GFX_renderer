#pragma once

#include <chrono>
#include <string>

namespace gfx
{
    using namespace std::chrono;

    class Timer
    {
    public:
        Timer();

        void Reset();

        auto ElapsedSeconds() -> float;

        auto ElapsedMillis() -> float;

    private:
        time_point<high_resolution_clock> m_start;
    };

    class ScopedTimer
    {
    public:
        ScopedTimer(const std::string& name);

        ~ScopedTimer();

    private:
        std::string m_name;
        Timer m_timer;
    };

}  // namespace gfx

#define GFX_SCOPED_TIMER(NAME) ScopedTimer timer__LINE__(NAME)