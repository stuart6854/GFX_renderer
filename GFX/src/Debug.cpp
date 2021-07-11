#include "GFX/Debug.h"

namespace gfx
{
    static DebugCallbackFn s_callback;

    void SetDebugCallback(DebugCallbackFn callback)
    {
        s_callback = callback;
    }

    void Log(DebugLevel level, const std::string& msg)
    {
        if (s_callback != nullptr)
            s_callback(level, msg);
    }

    void Trace(const std::string& msg)
    {
        Log(DebugLevel::eTrace, msg);
    }

    void Info(const std::string& msg)
    {
        Log(DebugLevel::eInfo, msg);
    }

    void Warn(const std::string& msg)
    {
        Log(DebugLevel::eWarn, msg);
    }

    void Error(const std::string& msg)
    {
        Log(DebugLevel::eError, msg);
    }
}
