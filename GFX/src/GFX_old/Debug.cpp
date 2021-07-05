#include "GFX/Debug.h"

#include <vector>

namespace gfxOld
{
    static std::vector<DebugCallbackFn> s_callbacks;

    void RegisterDebugCallback(DebugCallbackFn callback)
    {
        s_callbacks.push_back(callback);
    }

    void Log(DebugLevel level, const std::string& msg)
    {
        for (const auto& fn : s_callbacks)
        {
            fn(level, msg);
        }
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
